#include "osios.h"

#include <QCoreApplication>
#include <QSettings>
#include <QTimer>
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <QHash>
#include <QHashIterator>
#include <QDateTime>
#include <QScopedPointer>

#include "ibootstrapsplashscreen.h"
#include "iosiosdhtbootstrapclient.h"
#include "iosiosclient.h"
#include "osioscommon.h"
#include "timelineserializer.h"
#include "osiosdht.h"
#include "osiosdhtpeer.h"
#include "osiossettings.h"
#include "timelinenodetypes/profilecreationannounce_aka_genesistimelinenode.h"
#include "iosioscopycatclient.h"

#define OSIOS_TIMELINE_FILENAME "timeline.bin"

#define OSIOS_TIMELINE_NODE_CRYPTOGRAPHIC_HASH_NEIGHBOR_VERIFICATION_TIMEOUT_MILLISECONDS 5000

//this define below doesn't matter, but needs to be CONSISTENT (so the hash matches) on both the sender of the timeline node and the receiver/crypto-hash-verifier. I decided to let it keep the profile name in it so that the algorithms have just a bit more data to work with (and this hopefully avoids collisions a tad)
#define WHETHER_OR_NOT_TO_HAVE_PROFILE_NAME_IN_TIMELINE_NODE_WHEN_COMPUTING_CRYPTOGRAPHIC_HASH TimelineNodeByteArrayContainsProfileNameEnum::TimelineNodeByteArrayDoesContainProfileName

//TODOoptional: COW on the timeline nodes (the seraialized bytes, that is) could be achieved with a timeline node called "import state from doc x, mutate with empty character" (or perhaps these two are separated), effectively incrementing the reference count (although delete is not in plans anyways)
//TODOoptional: in addition to (or perhaps integrated INTO) the red/green "bar of color", i could create reproduceable patterns of color using the latest timeline node hash, so that i can look for that pattern on each of my monitors/nodes and see that they are all 3 synchronized. it's mildly cryptographic verification performed by a human (since doing so in an extreme manner would require you to sit all day every day typing in things on a calculator)
//Head just exploded: a cli front-end can copycat a gui front-end and vice versa? Why not :-P?
Osios::Osios(QObject *parent)
    : QObject(parent)
    , m_Arguments(QCoreApplication::arguments())
    //TODOreq: pending refactor: , m_ProfileName(profileName)
    , m_LastSerializedTimelineIndex(-1)
    , m_LocalPersistenceDevice(0)
    , m_OsiosDht(new OsiosDht(this, this))
{
    qRegisterMetaType<ListOfDhtPeerAddressesAndPorts>("ListOfDhtPeerAddressesAndPorts"); //TODOoptional: only register once and make re-entrant + thread safe, although maybe who cares since re-registering a type fails sanely (nothing happens) and the register attempt is already thread safe...
    qRegisterMetaType<TimelineNode>("TimelineNode");

    m_Arguments.removeFirst(); //filepath

    QString theSettingsFilepath_OrEmptyStringIfNoneSpecified;
    if(!parseOptionalSettingsFilepathArgDidntReturnParseError(&theSettingsFilepath_OrEmptyStringIfNoneSpecified))
    {
        //TODOreq: error in non-fatal way xD
        qFatal("Failed to parse settings file from arguments");
        return;
    }
    OsiosSettings::setSettingsFilenameApplicationWide(theSettingsFilepath_OrEmptyStringIfNoneSpecified);
}
Osios::~Osios()
{
    if(m_LocalPersistenceDevice)
    {
        //update the [probably, but not necessarily, changed] serialized number of timeline nodes at the beginning of the file (better, use a mutation append-only strategy)
        m_LocalPersistenceDevice->seek(0);
        m_LocalPersistenceStream << m_TimelineNodes.size();
        m_LocalPersistenceDevice->flush();
    }
    qDeleteAll(m_TimelineNodes);
}
QList<TimelineNode> Osios::timelineNodes() const
{
    return m_TimelineNodes;
}
bool Osios::parseOptionalSettingsFilepathArgDidntReturnParseError(QString *theSettingsFilepathOutput)
{
    //the settings file is able to not be specified, but parse errors return false
    int systemWideSettingsFileArgindex = m_Arguments.indexOf("--system-wide-settings-file"); //eh system wide is kinda incorrect, since technically it's user-wide if run by user, system wide if run by root, etc (just like QSettings says), but fuck it for now and I don't want people to erroneously use this [undocumented for now] arg instead of --profile. They might confuse one for the other
    if(systemWideSettingsFileArgindex > -1)
    {
        if((systemWideSettingsFileArgindex+1) >= m_Arguments.size())
        {
            usageAndQuit();
            return false;
        }
        *theSettingsFilepathOutput = m_Arguments.at(systemWideSettingsFileArgindex+1);
        m_Arguments.removeAt(systemWideSettingsFileArgindex);
        m_Arguments.removeAt(systemWideSettingsFileArgindex);
        if(m_Arguments.indexOf("--system-wide-settings-file") != -1)
        {
            usageAndQuit();
            return false;
        }
        return true;
    }
    return true;
}
bool Osios::parseOptionalLocalServerPortFromArgsDidntReturnParseError(quint16 *portToParseInto)
{
    //the port is able to not be specified, but parse errors return false
    int listenPortArgVindex = m_Arguments.indexOf("--listen-port");
    if(listenPortArgVindex > -1)
    {
        if((listenPortArgVindex+1) >= m_Arguments.size())
        {
            usageAndQuit();
            return false;
        }
        bool convertOk = false;
        *portToParseInto = m_Arguments.at(listenPortArgVindex+1).toUShort(&convertOk);
        if(!convertOk)
        {
            *portToParseInto = 0;
            usageAndQuit();
            return false;
        }
        m_Arguments.removeAt(listenPortArgVindex);
        m_Arguments.removeAt(listenPortArgVindex);
        if(m_Arguments.indexOf("--listen-port") != -1)
        {
            usageAndQuit();
            return false;
        }
        return true;
    }
    return true;
}
bool Osios::parseOptionalBootstrapAddressesAndPortsFromArgsDidntReturnParseError(ListOfDhtPeerAddressesAndPorts *listOfDhtPeersAddressesAndPortsToParseInto)
{
    //they can provide zero or more boostrap nodes, only parse errors return false
    int addBootstrapNodeArgIndex = m_Arguments.indexOf("--add-bootstrap-node");
    while(addBootstrapNodeArgIndex > -1)
    {
        if((addBootstrapNodeArgIndex+1) >= m_Arguments.size())
        {
            usageAndQuit();
            return false;
        }
        QUrl userInputParser = QUrl::fromUserInput(m_Arguments.at(addBootstrapNodeArgIndex+1));
        if(!userInputParser.isValid())
        {
            usageAndQuit();
            return false;
        }
        DhtPeerAddressAndPort dhtPeerAddressAndPort;
        dhtPeerAddressAndPort.first = userInputParser.host();
        dhtPeerAddressAndPort.second = userInputParser.port();

        (*listOfDhtPeersAddressesAndPortsToParseInto).append(dhtPeerAddressAndPort);

        m_Arguments.removeAt(addBootstrapNodeArgIndex); //add bootstrap arg
        m_Arguments.removeAt(addBootstrapNodeArgIndex); //host:port

        addBootstrapNodeArgIndex = m_Arguments.indexOf("--add-bootstrap-node");
    }
    return true;
}
bool Osios::checkCopycatShouldBeEnabledEnableItAsWell()
{
    //check it's enabled
    QString nameOfNeighborToCopycatActionsTimeline_OrEmptyStringIfNone; //aka "monitor clone" mode
    int copyCatArgIndex = m_Arguments.indexOf("--copycat");
    if(copyCatArgIndex != -1)
    {
        if((copyCatArgIndex+1) >= m_Arguments.size())
        {
            usageAndQuit();
            return true; //although it's an error, we still want to indicate that copycat mode was specified to the caller
        }
        nameOfNeighborToCopycatActionsTimeline_OrEmptyStringIfNone = m_Arguments.at(copyCatArgIndex+1); //TODOreq: sanitize this and other cli args
        m_Arguments.removeAt(copyCatArgIndex);
        m_Arguments.removeAt(copyCatArgIndex);
        if(m_Arguments.indexOf("--copycat") != -1)
        {
            usageAndQuit();
            return true; //see above for justification of true
        }
    }

    bool copycatModeEnabled = !nameOfNeighborToCopycatActionsTimeline_OrEmptyStringIfNone.isEmpty();

    //enable it. this is our copycat equivalent of beginUsingProfileNameInOsios (and some of the rest of our caller's code, some gui init stuff, but not all of it ofc)
    if(copycatModeEnabled)
    {
        //copycat mode (for now I'm NOT going to disable input on the node that is going into copycat mode (this one). inputting anything using kb/mouse will lead to undefined results (i think that disabling input may be a waste of my efforts, since the contents will USUALLY not be seen [directly] and only used for screen splicing. to polish the monitor clone mode, however, (low priority), i should disable the user input. hell maybe just doing mainmenu.setReadOnly would do it??? rofl. it will seem weird, but i will deal with it for now, to be using profile b when copycatting profile a (b's timeline log will mirror a's in that case (unless i disconnect the actionOccured signal))
        //connect(m_Osios, SIGNAL(timelineNodeReceivedFromDhtPeer(OsiosDhtPeer*, TimelineNode)), m_MainWindow, SLOT(synthesizeEventUsingTimelineNodeReceivedFromDhtPeer(OsiosDhtPeer*,TimelineNode))); //TODOoptimization: only emit the signal for the copycat peer (take t

        IOsiosClient *frontEnd = profileAgnosticUiSetup(); //copycat has no "my profile", there is only the one he's copycatting

        if(m_OsiosDhtBootstrapClient->mainUiHasCopycatAbility())
        {
            //disable recording of user input from ui. if a user in front of the "clone monitor" changes/interacts-with the ui, it won't be recorded (and future timeline node synthesis is undefined!). we don't want to record copycatted actions. yea we just connected it a few lines up, whatever
            disconnect(frontEnd->asQObject(), SIGNAL(actionOccurred(TimelineNode)), this, SLOT(recordMyAction(TimelineNode)));

            IOsiosCopycatClient *copycatClient = m_OsiosDhtBootstrapClient->mainUiAsCopycatClient();
            copycatClient->setCopycatModeEnabled(true);
            m_NameOfNeighborToCopycatActionsTimeline_OrEmptyStringIfNone = nameOfNeighborToCopycatActionsTimeline_OrEmptyStringIfNone; //setCopycatModeEnabledForUsername(nameOfNeighborToCopycatActionsTimeline_OrEmptyStringIfNone); //can only be copycatting one person at a time (in the normal mode. i'm going to make hacks building on top of this that lets me copycat 2 others and splice screens etc xD)
            connect(this, SIGNAL(timelineNodeReceivedFromCopycatTarget(TimelineNode)), copycatClient->asQObject(), SLOT(synthesizeEventUsingTimelineNodeReceivedFromCopycatTarget(TimelineNode)));
        }
        else
        {
            qFatal("Back-end requested copycat mode, but front-end does not support it");
        }
    }

    return copycatModeEnabled;
}
IOsiosClient *Osios::profileAgnosticUiSetup() //used by both copycat mode and normal mode
{
    IOsiosClient *frontEnd = m_OsiosDhtBootstrapClient->createMainUi();
    //m_MainWindow = new OsiosMainWindow(m_Osios);
    connecToAndFromFrontendSignalsAndSlots(frontEnd);
    m_OsiosDhtBootstrapClient->presentMainUi();
    return frontEnd;
}
void Osios::connecToAndFromFrontendSignalsAndSlots(IOsiosClient *frontEnd)
{
    connect(frontEnd->asQObject(), SIGNAL(actionOccurred(TimelineNode)), this, SLOT(recordMyAction(TimelineNode)));

    connect(this, SIGNAL(connectionColorChanged(int)), frontEnd->asQObject(), SLOT(changeConnectionColor(int)));
    connect(this, SIGNAL(notificationAvailable(QString,OsiosNotificationLevels::OsiosNotificationLevelsEnum)), frontEnd->asQObject(), SIGNAL(presentNotificationRequested(QString,OsiosNotificationLevels::OsiosNotificationLevelsEnum)));
}
void Osios::beginUsingProfileNameInOsios(const QString &profileName)
{
    //kind of like logging in. we are already bootstrapped and the profile creation node already sent, but this means we're proceeding forward into the main menu using a chosen profile name. This used to be in my constructor, since bootstrap took place after profile selection. For now it's only safe to call once per app instance, but in the future I'd have an equivalent "stopUsingProfileInOsios" if they for example went back to the profile manager (File->Log Out (or "Choose Different User")

    QScopedPointer<QSettings> settings(OsiosSettings::newSettings()); //Note: don't use QObject parenting for cleanup of OsiosSettings because this class will be alive for really long periods of time, meaning lots of settings objects could be created on the heap if I did that xD
    settings->beginGroup(PROFILES_GOUP_SETTINGS_KEY);
    settings->beginGroup(profileName);
    QString dataDir = settings->value(OSIOS_DATA_DIR_SETTINGS_KEY).toString(); //TODOoptional: sanitize empty/invalid filename etc
    settings->endGroup();
    settings->endGroup();
    QString fileName = appendSlashIfNeeded(dataDir) + OSIOS_TIMELINE_FILENAME;
    m_ProfileName = profileName;
    qDebug() << "Current profile:" << profileName;
    qDebug() << "File path of timeline for current profile:" << fileName;
    bool newUser = !QFile::exists(fileName);
    if(m_LocalPersistenceDevice)
        delete m_LocalPersistenceDevice;
    m_LocalPersistenceDevice = new QFile(fileName, this);
    if(!m_LocalPersistenceDevice->open(QIODevice::ReadWrite))
    {
        qDebug() << "Failed to open local persistence device:" << fileName;
        //constructor, guh: emit notificationAvailable(Fatal);
        //TODOoptional: better handling
        return;
    }
    m_LocalPersistenceStream.setDevice(m_LocalPersistenceDevice);

    //double check where(when) profile creation timeline node is sent to dht. should be right when ok button of create profile dialog is pressed, since we are already bootstrapped. TODOreq: should i wait for confirmation of that first one? NAWWW don't reinvent the dht's crytpo verification process over and over ya nub. the profile creation dialog should send osios a signal to actually create the profile, not do it himself
    if(newUser) //TODOreq: move to profile creation dialog (and keep using interfaces ofc), which is post-bootstrap now. Perhaps the interface emits that profile creation is requested, and we still handle it in this class. HOWEVER, putting it here gives us "lazy creation" of the profile node (but timeline.bin is already created anyways so... (and shit wtf no i can't put it in create dialog unless i open the timeline.bin file in there as well :-/)
    {
        m_LocalPersistenceStream << static_cast<qint32>(1); //new user, so no timeline nodes, so write 1 size placeholder into file, which is rewound to and updated in our destructor. we write 1 instead of 0 because the 1 we are talking about is the very next statement. we also want the file header to be accurate for the call to readInAllMyPreviouslySerializedLocallyEntries. yes it's inefficient to write and re-read a moment later like that, but idfc (and sheeit prolly still in kernel memory anyways so fuggit)

        //Hack: usually the front-end tells us about timeline nodes via action occured. this is one exception
        ProfileCreationAnnounce_aka_GenesisTimelineNode *profileCreationAnnounceTimelineNode = new ProfileCreationAnnounce_aka_GenesisTimelineNode(profileName);
        recordMyAction(profileCreationAnnounceTimelineNode); //TODOreq: since there is no "catch up" message/mode-of-connecting, and since we are in constructor, calling my usual recordMyAction() method will work as far as getting the action to serialize to disk, but won't ever get it to propagate to neighbor since the dht is neither instantiated nor bootstrapping at this point!
    }

    //read in all previously serialized entries, which also puts the write cursor at the end for appending
    readInAllMyPreviouslySerializedLocallyEntries(); //TODOoptimization: once HELLO (+sync) is finished, there may be an opportunity to not have to re-read timeline nodes that were read during sync process

    int timelineSize = m_TimelineNodes.size();
    qDebug() << "Read in " << QString::number(timelineSize) << " previously serialized entries in the timeline"; //TODOreq: the deserialized number is accurate, but the timeline list widget is empty. this does kind of relate to state, since, i also want to re-open tabs that were open in last session, but now i'm getting off-topic
    if(timelineSize == 0)
    {
        //TODOoptional: error
    }

    QTimer *diskFlushTimer = new QTimer(this);
    connect(diskFlushTimer, SIGNAL(timeout()), this, SLOT(flushDiskBuffer()));
    diskFlushTimer->start(2*60*1000); //2 mins

    QTimer *checkRecentlyGeneratedTimelineNodesAwaitingCryptographicVerificationFromMoreNeighborsForTimedOutTimelineNodesTimer = new QTimer(this);
    connect(checkRecentlyGeneratedTimelineNodesAwaitingCryptographicVerificationFromMoreNeighborsForTimedOutTimelineNodesTimer, SIGNAL(timeout()), this, SLOT(checkRecentlyGeneratedTimelineNodesAwaitingCryptographicVerificationFromMoreNeighborsForTimedOutTimelineNodes()));
    checkRecentlyGeneratedTimelineNodesAwaitingCryptographicVerificationFromMoreNeighborsForTimedOutTimelineNodesTimer->start(OSIOS_TIMELINE_NODE_CRYPTOGRAPHIC_HASH_NEIGHBOR_VERIFICATION_TIMEOUT_MILLISECONDS/2 /* check every 2.5 seconds if the any of the timeline nodes haven't been verified in 5 seconds. our worst case scenario is now 7.5 seconds xD */);
}
void Osios::readInAllMyPreviouslySerializedLocallyEntries()
{
    if(!m_LocalPersistenceStream.atEnd())
    {
        qint32 numTimelineNodes;
        //if(m_LocalPersistenceStream.device()->bytesAvailable() < sizeof(numTimelineNodes)) -- not network so yea
        //    return;
        m_LocalPersistenceStream >> numTimelineNodes;
        int currentIndex = m_TimelineNodes.size()-1;
        for(int i = 0; i < numTimelineNodes; ++i)
        {
            ITimelineNode *serializedTimelineNode = deserializeNextTimelineNode(TimelineNodeByteArrayContainsProfileNameEnum::TimelineNodeByteArrayDoesNotContainProfileName);
            //m_LocalPersistenceStream >> serializedTimelineNode;
            m_TimelineNodes.append(serializedTimelineNode);
            ++currentIndex;
            m_LastSerializedTimelineIndex = ++currentIndex;
        }
    }
}
ITimelineNode *Osios::deserializeNextTimelineNode(TimelineNodeByteArrayContainsProfileNameEnum::TimelineNodeByteArrayContainsProfileNameEnumActual whetherOrNotTheByteArrayHasProfileNameInIt_IfYouGotItFromNetworkThenYesItDoesButIfFromDiskThenNoItDoesnt)
{
    //was tempted to put if(!m_LocalPersistenceDevice) return 0; here, but I'm pretty damn sure it'd have to be instantiated if we got this far (the error would show up during testing). Plus, returning zero doesn't do much unless we check it in the caller too xD

    //we have to return a pointer because we have to instantiate the derived type
    return TimelineSerializer::peekInstantiateAndDeserializeNextTimelineNodeFromIoDevice(m_LocalPersistenceDevice, whetherOrNotTheByteArrayHasProfileNameInIt_IfYouGotItFromNetworkThenYesItDoesButIfFromDiskThenNoItDoesnt);
}
//batch write, but really doesn't make much difference since we flush every X seconds anyways
void Osios::serializeMyTimelineAdditionsLocally()
{
    int timelineSize = m_TimelineNodes.size();
    while((m_LastSerializedTimelineIndex+1) < timelineSize)
    {
        TimelineNode timelineNode = m_TimelineNodes.at(++m_LastSerializedTimelineIndex);
        serializeTimelineActionLocally(timelineNode);
    }
}
void Osios::serializeTimelineActionLocally(TimelineNode action)
{
    //we side-step the "write profile name in action?" logic when reading/writing to/from disk, but that what we want since they aren't wrapped in byte arrays when stored locally (right?), guh maybe splitting the protocol like this is a terrible idea
    m_LocalPersistenceStream << *action;
}
void Osios::flushDiskBuffer()
{
    if(!m_LocalPersistenceDevice)
        return;
    if(!m_LocalPersistenceDevice->flush())
    {
        emit notificationAvailable(tr("Failed to flush to the local persistence device (ex: hard drive)!"), OsiosNotificationLevels::FatalNotificationLevel);
    }
    //TODOreq: still need to do "sync" system call (and notification if that fails), but i don't think there's a portable way to do it :(
}
void Osios::checkRecentlyGeneratedTimelineNodesAwaitingCryptographicVerificationFromMoreNeighborsForTimedOutTimelineNodes()
{
    QHashIterator<CryptographicHashAndTimeoutTimestamp> recentlyGeneratedTimelineNodesAndTheirTimeoutTimestampsIterator(m_RecentlyGeneratedTimelineNodesAndTheirTimeoutTimestamps);
    while(recentlyGeneratedTimelineNodesAndTheirTimeoutTimestampsIterator.hasNext())
    {
        recentlyGeneratedTimelineNodesAndTheirTimeoutTimestampsIterator.next();
        if(recentlyGeneratedTimelineNodesAndTheirTimeoutTimestampsIterator.value() <= QDateTime::currentMSecsSinceEpoch())
        {
            //TODomb: retry? remove from list so the same error isn't emitted for the same timeline node in 5 more seconds?
            emit notificationAvailable(tr("Failed to replicate a timeline node to ") + QString::number(OSIOS_NUM_NEIGHBORS_TO_WAIT_FOR_VERIFICATION_FROM_BEFORE_CONSIDERING_A_TIMELINE_NODE_VERIFIED) + tr(" neighbors within ") + QString::number(OSIOS_TIMELINE_NODE_CRYPTOGRAPHIC_HASH_NEIGHBOR_VERIFICATION_TIMEOUT_MILLISECONDS/1000) + tr(" seconds!!"), OsiosNotificationLevels::ErrorNotificationLevel);
            if(m_OsiosDht->dhtState() != OsiosDhtStates::BootstrappingForFirstTimeState)
            {
                m_OsiosDht->setDhtState(OsiosDhtStates::FellBelowMinBootstrapNodesState);
            }
        }
    }
}
void Osios::handleDhtStateChanged(OsiosDhtStates::OsiosDhtStatesEnum newDhtState)
{
    switch(newDhtState)
    {
        case OsiosDhtStates::InitialIdleNoConnectionsState:
        {
            emit connectionColorChanged(Qt::black); //TO DOnereq(nop): disabling dht- (wait no you can't it's the whole purpose of the app). err was going to say that if the dht is disabled then black should always be shown...
            emit notificationAvailable(tr("DHT state changed to Stopped"));
        }
            break;

        case OsiosDhtStates::BootstrappingForFirstTimeState:
        {
            emit connectionColorChanged(Qt::yellow);
            emit notificationAvailable(tr("DHT state changed to Bootstrapping..."));
        }
            break;

        case OsiosDhtStates::BootstrappedState:
        {
            emit connectionColorChanged(Qt::green);
            emit notificationAvailable(tr("DHT state changed to Bootstrapped"));
            emit bootstrapped(); //to let dht bootstrap splash know to close itself, and we listen to it [the first time it's emitted only] to create/present the varios UI etc after the splash
        }
            break;

        case OsiosDhtStates::CleanlyDisconnectingOutgoingNotAcceptingnewIncomingConnectionsState:
        {
            emit connectionColorChanged(Qt::cyan);
            emit notificationAvailable(tr("DHT is starting to try to cleanly shut down..."));
        }
            break;

        case OsiosDhtStates::FellBelowMinBootstrapNodesState:
        {
            emit connectionColorChanged(Qt::red);
            emit notificationAvailable(tr("The DHT changed states to indicate that we are NOT connected to at least ") + QString::number(OSIOS_DHT_MIN_PEERS_CONNECTED_FOR_BOOTSTRAP_TO_BE_CONSIDERED_COMPLETE) + tr(" healthy peers!"), OsiosNotificationLevels::ErrorNotificationLevel);
        }
            break;
        default /*wat*/:
        {
            emit connectionColorChanged(Qt::red);
            emit notificationAvailable(tr("The DHT changed to an unknown state!"), OsiosNotificationLevels::FatalNotificationLevel);
        }
            break;
    }
}
void Osios::cyryptoNeighborReplicationVerificationStep1aOfX_WeReceiver_storeAndHashNeighborsActionAndRespondWithHash(OsiosDhtPeer *osiosDhtPeer, TimelineNode action)
{
    //nothing too difficult here, but what 'profile' to use for the serialization is in question. is a dht and it's set of nodes associated with a profile? or should all of the incoming dhts be global to every local profile?
    //what i mean by "what profile" is basically I need a dht node id, and ip address isn't good enough. it needs to reflect their profile name perhaps. another way to solve this is to use public/private keys and to use the public keys as identifiers (this has other gains as well ofc, but I'm simply looking for simplest)

    serializeNeighborActionLocally(osiosDhtPeer, action); //TODOreq: use the same period flushing code as our own serializeMyTimelineAdditionsLocally (this is why a io thread is nice (however i'm not as thread safe as i could be dommit))

    cyryptoNeighborReplicationVerificationStep1bOfX_WeReceiver_hashNeighborsActionAndRespondWithHash(osiosDhtPeer, action);


    //TO DOnemb(solved-at-end-of-paragraph): don't copycat the timeline node until the timeline node originator/sender tells us that our cryptographic response ([async-begin] sent one statement above) -- either that or I'd need some kind of copycat-step-undo functionality *gasp*. for now im' going to trust the connection is stable and not lying etc (AFTER ALL, VISUALIZING THE RESULTS IS VERIFICATION ENOUGH!!!)
    if(!m_NameOfNeighborToCopycatActionsTimeline_OrEmptyStringIfNone.isEmpty() && action->ProfileName == m_NameOfNeighborToCopycatActionsTimeline_OrEmptyStringIfNone)
    {
        //NOPE: copycat mode handled differently, serializing and cryptographically verifying would be redundant, since we are a peer of the dht already
        //^i was going to move this above the serialize/cryptoNeighborReplicate calls above, and return before getting to them. it is in our role as a peer of said dht that we received the copycat timeline stream to begin with, so  we still need to fulfill our role as dht peer in addition to doing copycat stuff
        //TO DOnereq: every node i received from copycat target needs to be sent to recordMyAction, BUT (this is what i meant above) recordMyAction is only to get it to show up in the our timeline list widget -- we want to use hacks/etc to disable both the serializing and replicating to neighbors that usually comes with calling recordMyAction
        emit timelineNodeReceivedFromCopycatTarget(action); //synthesize event actual in ui
        emit timelineNodeAdded(action); //synthesize event timeline entry in ui. well that was easy lol, but TODOreq: i still need to put copycat mode into a profile-less watcher maybe. at the very least, when copycatting a and 'using' profile b, we sure as shit don't want any of the synthesized events to be recorded as b's actual actions (especially since those will then be persisted and replicated more redundantly than we desire (with the sender being a replica of himself, doh!)). i think most relevantly i want b's profile creation not to be sent. hell he shouldn't even have a timeline or profile folder or any of that shit (I'm just not sure that refactor is such a good idea)
        //return;
    }
}
void Osios::cyryptoNeighborReplicationVerificationStep2OfX_WeReceiver_handleResponseCryptoGraphicHashReceivedFromNeighbor(OsiosDhtPeer *osiosDhtPeer, QByteArray cryptoGraphicHashOfTimelineNodePreviouslySent)
{
    CryptographicHashAndTheListofDhtPeersThatHaveVerifiedItSoFar::const_iterator it = m_RecentlyGeneratedTimelineNodesAwaitingCryptographicVerificationFromMoreNeighbors_AndTheNeighborsWhoHaveVerifiedThisHashAlready.find(cryptoGraphicHashOfTimelineNodePreviouslySent);
    if(it == m_RecentlyGeneratedTimelineNodesAwaitingCryptographicVerificationFromMoreNeighbors_AndTheNeighborsWhoHaveVerifiedThisHashAlready.end())
    {
        //TODOreq: if we have N replicas but only care about X of them (with X being <= N of course), maybe we just received some we already consider "verified" and that's why it's not in this list. For now, error notification to be on the safe side
        emit notificationAvailable(tr("A neighbor sent us a cryptographic hash that we weren't expecting"), OsiosNotificationLevels::ErrorNotificationLevel); //TODOmb: ping the offending neighbor and ask what's up, start inquiries and other bullshit that increases the complexity of a dht a trillion fold
        return;
    }

    //TODOoptional, check that the dht peer didn't already send us that hash, and then error report it (I am going to ensure that they aren't accounted for twice, but might not have the error reporting)
    QSet<OsiosDhtPeer*> *dhtPeersThatAlreadySentUsThatHash = it.value();

    if(dhtPeersThatAlreadySentUsThatHash->contains(osiosDhtPeer))
    {
        emit notificationAvailable(tr("A neighbor sent us the same cryptographic hash twice"), OsiosNotificationLevels::WarningNotificationLevel);
        return;
    }

    //add peer to list of neighbors that have verified this hash
    dhtPeersThatAlreadySentUsThatHash->insert(osiosDhtPeer);


    cryptoNeighborReplicationVerificationStep2ofX_WeSenderOfTImelineOriginallyAndNowReceiverOfHashVerification_removeThisHashFromAwaitingVerificationListAfterCheckingIfEnoughNeighborsHaveCryptographicallyVerifiedBecauseWeJustAddedAPeerToThatList_AndStopTheTimeoutForThatPieceIfRemoved(cryptoGraphicHashOfTimelineNodePreviouslySent, dhtPeersThatAlreadySentUsThatHash);
    //TODOreq: now we check it against what we calculated for it earlier and (had:report errors) if the hash isn't found in our list of hashes watching for (meaning a mismatch, in which case we do nothing and the 5 second timeout will flash red etc). we should remove a hash from being watched once all (or N) of the neighbors respond with a valid cryptographic hash
}
void Osios::cyryptoNeighborReplicationVerificationStep0ofX_WeSender_propagateActionToNeighbors(TimelineNode action)
{
    //TODOreq: as an implicit sub-action of this step, we need to first record the cryptgraphic hash for later comparison/verification. However, as an optimization it's better to only ever convert action to a byte array once. I'm just going to err on the side of understandability over optimization at this point (because...)
    QByteArray actionCryptographicHash = calculateCrytographicHashOfTimelineNode(action);
    m_RecentlyGeneratedTimelineNodesAwaitingCryptographicVerificationFromMoreNeighbors_AndTheNeighborsWhoHaveVerifiedThisHashAlready.insert(actionCryptographicHash, new QSet<OsiosDhtPeer*>() /*delete the QSet in our destructor or whenever appropriate*/); //small inefficiency beats small refactor anyday. human time > *. shit how could anything be greater than everything, else be greater than itself (impossibru)
    m_RecentlyGeneratedTimelineNodesAndTheirTimeoutTimestamps.insert(actionCryptographicHash, QDateTime::currentMSecsSinceEpoch() + OSIOS_TIMELINE_NODE_CRYPTOGRAPHIC_HASH_NEIGHBOR_VERIFICATION_TIMEOUT_MILLISECONDS); //epoch-based qint64 instead of qdatetime because datetimes can "jump in time" (leap years, etc)

    //TODOreq: might be a good optimization to put everything network on a network thread, and maybe even all disk access on a disk thread. for now KISS everything goes on main thread. I am pretty sure Qt's APIs are async for me already anyways... so unless I above it, lockup is unlikely. HOWEVER this is still a good idea in general just because it will scale better in the future when maybe timeline nodes are much larger in size. It also isn't that difficult (would have taken about as much time as this comment)

    m_OsiosDht->sendNewTimelineNodeToAllDhtPeersWithHealthyConnectionForFirstStepOfCryptographicVerification(action);
}
void Osios::cyryptoNeighborReplicationVerificationStep1bOfX_WeReceiver_hashNeighborsActionAndRespondWithHash(OsiosDhtPeer *osiosDhtPeer, TimelineNode action)
{
    QByteArray sha1SumOfTimelineNode = calculateCrytographicHashOfTimelineNode(action);
    //now send it back to the neighbor for verification
    osiosDhtPeer->respondWithCryptographicHashComputedFromReceivedTimelineNode(sha1SumOfTimelineNode);
}
void Osios::cryptoNeighborReplicationVerificationStep2ofX_WeSenderOfTImelineOriginallyAndNowReceiverOfHashVerification_removeThisHashFromAwaitingVerificationListAfterCheckingIfEnoughNeighborsHaveCryptographicallyVerifiedBecauseWeJustAddedAPeerToThatList_AndStopTheTimeoutForThatPieceIfRemoved(QByteArray keyToListToRemoveFrom, QSet<OsiosDhtPeer*> *listToMaybeRemove)
{
    if(listToMaybeRemove->size() >= OSIOS_NUM_NEIGHBORS_TO_WAIT_FOR_VERIFICATION_FROM_BEFORE_CONSIDERING_A_TIMELINE_NODE_VERIFIED)
    {
        //good, verified, remove from list
        m_RecentlyGeneratedTimelineNodesAwaitingCryptographicVerificationFromMoreNeighbors_AndTheNeighborsWhoHaveVerifiedThisHashAlready.remove(keyToListToRemoveFrom);
        m_RecentlyGeneratedTimelineNodesAndTheirTimeoutTimestamps.remove(keyToListToRemoveFrom);
        //emit notificationAvailable();
        qDebug() << "Timeline Node Hash verified by" << OSIOS_NUM_NEIGHBORS_TO_WAIT_FOR_VERIFICATION_FROM_BEFORE_CONSIDERING_A_TIMELINE_NODE_VERIFIED << "DHT neighbors:" << keyToListToRemoveFrom.toHex(); //TODOoptional: associate and print human readable timeline node 'action'
    }
}
void Osios::serializeNeighborActionLocally(OsiosDhtPeer *osiosDhtPeer, TimelineNode action)
{
    //TODOreq: simple to do once I decide WHICH file[name] to write to, based on some unique id (what if two users on network choose same profile name? ideally we could handle that)
}
QByteArray Osios::calculateCrytographicHashOfTimelineNode(TimelineNode action, QCryptographicHash::Algorithm algorithm)
{
    QByteArray timelineNodeRawByteArray = action->toByteArray(WHETHER_OR_NOT_TO_HAVE_PROFILE_NAME_IN_TIMELINE_NODE_WHEN_COMPUTING_CRYPTOGRAPHIC_HASH);
    return QCryptographicHash::hash(timelineNodeRawByteArray, algorithm);
}
QString Osios::usage()
{
    //TODOreq: modal dialog? might as well just ask them for the port at that point (but... asking for bootstrap nodes is more work guh)
    QString ret(tr("You either didn't pass enough arguments to the app, or you passed invalid arguments.\n\nUsage: Osios localServerPort [--listen-port N --add-bootstrap-node host:port]\n\nIf you don't specify a --listen-port, one is chosen for you at random.\nYou can supply --add-bootstrap-node multiple times"));
    return ret;
}
void Osios::usageAndQuit()
{
    emit notificationAvailable(usage(), OsiosNotificationLevels::FatalNotificationLevel);
    emit quitRequested();
}
void Osios::initializeAndStart(IOsiosDhtBootstrapClient *osiosDhtBootstrapClient)
{
    m_OsiosDhtBootstrapClient = osiosDhtBootstrapClient;

    int profileSpecificationArgIndex = m_Arguments.indexOf("--profile");
    if(profileSpecificationArgIndex != -1)
    {
        if((profileSpecificationArgIndex+1) >= m_Arguments.size())
        {
            usageAndQuit();
            return;
        }
        m_ProfileNameUserWantsToUse_OrEmptyStringIfNoneDecidedCreatedEtcYet = m_Arguments.at(profileSpecificationArgIndex+1);
        m_Arguments.removeAt(profileSpecificationArgIndex);
        m_Arguments.removeAt(profileSpecificationArgIndex);
        if(m_Arguments.indexOf("--profile") != -1)
        {
            usageAndQuit();
            return;
        }
    }

    QScopedPointer<QSettings> settings(OsiosSettings::newSettings());
    if(m_ProfileNameUserWantsToUse_OrEmptyStringIfNoneDecidedCreatedEtcYet.isEmpty())
    {
        m_ProfileNameUserWantsToUse_OrEmptyStringIfNoneDecidedCreatedEtcYet = settings->value(LAST_USED_PROFILE_SETTINGS_KEY).toString(); //try reading last used profile, but it may also be empty/missing so that would leave the string the same as it was anyways
    }

    bool autoLoginLastUsedProfileOnBootstrap = settings->value(AUTO_LOGIN_LAST_USED_PROFILE_ON_BOOTSTRAP_SETTINGS_KEY).toBool();

    if(m_OsiosDhtBootstrapClient->hasBootstrapSplashScreen())
    {
        IOsiosDhtBootstrapSplashScreen *bootstrapSplashScreen = m_OsiosDhtBootstrapClient->createAndPresentBootstrapSplashScreen(m_ProfileNameUserWantsToUse_OrEmptyStringIfNoneDecidedCreatedEtcYet, autoLoginLastUsedProfileOnBootstrap);
        //bootstrapSplashScreen->presentSelf();
        connect(this, SIGNAL(bootstrapped()), bootstrapSplashScreen->asQObject(), SLOT(handleBootstrapped()));
    }
    connect(this, SIGNAL(bootstrapped()), this, SLOT(showProfileCreatorOrManagerOrSkipAndDisplayMainMenuIfRelevant()));

#if 0 //ip/port enough...
    QByteArray myDhtId = settings->value(DHT_ID_SETTINGS_KEY).toByteArray();
    if(myDhtId.isEmpty())
    {
        //first launch, generate dht id
        QByteArray randomInput = QDateTime::currentMSecsSinceEpoch();
        //randomInput.append(QString::number(static_cast<quintptr>(this)).toUtf8());
        myDhtId = QCryptographicHash::hash(randomInput, QCryptographicHash::Sha1);
        settings->setValue(DHT_ID_SETTINGS_KEY, myDhtId);
    }
#endif

    //parse local port from args
    quint16 localServerPort_OrZeroToChooseRandomPort = 0;
    if(!parseOptionalLocalServerPortFromArgsDidntReturnParseError(&localServerPort_OrZeroToChooseRandomPort))
    {
        return;
    }

    //parse bootstrap node ip/ports from args
    ListOfDhtPeerAddressesAndPorts bootstrapAddressesAndPorts; //TODOreq: store some (ALL LEARNED?) addresses/ips in settings or something
    if(!parseOptionalBootstrapAddressesAndPortsFromArgsDidntReturnParseError(&bootstrapAddressesAndPorts))
    {
        return;
    }

    //start bootstrapping
    connect(m_OsiosDht, SIGNAL(dhtStateChanged(OsiosDhtStates::OsiosDhtStatesEnum)), this, SLOT(handleDhtStateChanged(OsiosDhtStates::OsiosDhtStatesEnum)));
    QMetaObject::invokeMethod(m_OsiosDht, "bootstrap", Qt::QueuedConnection /*so we can connect first*/, Q_ARG(ListOfDhtPeerAddressesAndPorts, bootstrapAddressesAndPorts), Q_ARG(quint16, localServerPort_OrZeroToChooseRandomPort));
}
void Osios::recordMyAction(TimelineNode action)
{
    action->ProfileName = m_ProfileName; //TODOmb: set earlier? TODOoptimization(tl;dr:COW): don't store it in memory action, but hackily stream it over the network still. actually proper use of COW mitigates that problem anyways, nvm

    m_TimelineNodes.append(action); //takes ownership, deletes in this' destructor
    cyryptoNeighborReplicationVerificationStep0ofX_WeSender_propagateActionToNeighbors(action); //replicate it to network neighbors for cryptographic verification
    serializeMyTimelineAdditionsLocally(); //writes to file but does not flush. i could make this method the target of diskFlushTimer if i wanted, since the action being in m_TimelineNodes means it's recorded. I don't think it matters whether I buffer it or Qt does, since both are userland.
    emit timelineNodeAdded(action);
}
void Osios::showProfileCreatorOrManagerOrSkipAndDisplayMainMenuIfRelevant()
{
    disconnect(this, SIGNAL(bootstrapped()), this, SLOT(showProfileCreatorOrManagerOrSkipAndDisplayMainMenuIfRelevant())); //we only want to do this slot once, the very first time we've bootstrapped, not every time we re-bootstrap etc

    //we are bootstrapped
    //some cases when skipping and going to main menu directly are: --profile specified, lastUsedProfile is set and auto-login set and was not unchecked in the dht bootstrap splash

    if(checkCopycatShouldBeEnabledEnableItAsWell())
        return; //nothing left for us to do

    bool profileManagerRequested = false;
    int profileManagerArgIndex = m_Arguments.indexOf("--profile-manager");
    if(profileManagerArgIndex != -1)
    {
        profileManagerRequested = true;
        m_Arguments.removeAt(profileManagerArgIndex);
        if(m_Arguments.indexOf("--profile-manager") != -1)
        {
            usageAndQuit();
            return;
        }
    }

    if(profileManagerRequested)
    {
        if(m_OsiosDhtBootstrapClient->hasProfileManagerUi())
        {
            if(!m_OsiosDhtBootstrapClient->showProfileManagerAndReturnWhetherItWasAcceptedOrNotWhichTellsUsIfAProfileWasSelectedOrNot(&m_ProfileNameUserWantsToUse_OrEmptyStringIfNoneDecidedCreatedEtcYet))
            {
                //cancel pressed
                emit quitRequested();
                return;
            }
        }
        else
        {
            qFatal("Back-end requested profile manager, but the front-end did not provide one");
            return;
        }
    }
    else if(m_ProfileNameUserWantsToUse_OrEmptyStringIfNoneDecidedCreatedEtcYet.isEmpty())
    {
        if(m_OsiosDhtBootstrapClient->hasCreateProfileUi())
        {
            if(!m_OsiosDhtBootstrapClient->showCreateProfileAndReturnWhetherOrNotTheDialogWasAccepted(&m_ProfileNameUserWantsToUse_OrEmptyStringIfNoneDecidedCreatedEtcYet))
            {
                emit quitRequested(); //TODOoptional: modal dialog infinite loop asking them if they're sure they want to quit or if they want to go to the create profile screen again
                return;
            }
        }
        else
        {
            qFatal("Back-end requested create profile wizard, but the front-end did not provide one");
            return;
        }
    }

    //if we get here, m_ProfileNameToUse_OrEmptyStringIfNoneDecidedCreatedEtcYet is set and valid
    QScopedPointer<QSettings> settings(OsiosSettings::newSettings());
    settings->setValue(LAST_USED_PROFILE_SETTINGS_KEY, m_ProfileNameUserWantsToUse_OrEmptyStringIfNoneDecidedCreatedEtcYet);
    //m_Osios = new Osios(m_ProfileNameToUse_OrEmptyStringIfNoneDecidedCreatedEtcYet, localServerPort_OrZeroToChooseRandomPort, bootstrapAddressesAndPorts);
    profileAgnosticUiSetup(); //we're in normal mode (not copycat mode)

#if 0 //ignore
    if(!m_Arguments.isEmpty())
    {
        //unused arg. TODOoptional: quit? ignore?
        usageAndQuit();
        return;
    }
#endif

    beginUsingProfileNameInOsios(m_ProfileNameUserWantsToUse_OrEmptyStringIfNoneDecidedCreatedEtcYet);
}
