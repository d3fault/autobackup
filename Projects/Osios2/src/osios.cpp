#include "osios.h"

#include <QSettings>
#include <QTimer>
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <QHash>
#include <QHashIterator>
#include <QDateTime>

#include "osioscommon.h"
#include "timelineserializer.h"
#include "osiosdht.h"
#include "osiosdhtpeer.h"
#include "osiossettings.h"

#define OSIOS_TIMELINE_FILENAME "timeline.bin"

#define OSIOS_TIMELINE_NODE_CRYPTOGRAPHIC_HASH_NEIGHBOR_VERIFICATION_TIMEOUT_MILLISECONDS 5000

//TODOoptional: COW on the timeline nodes (the seraialized bytes, that is) could be achieved with a timeline node called "import state from doc x, mutate with empty character" (or perhaps these two are separated), effectively incrementing the reference count (although delete is not in plans anyways)
//TODOoptional: in addition to (or perhaps integrated INTO) the red/green "bar of color", i could create reproduceable patterns of color using the latest timeline node hash, so that i can look for that pattern on each of my monitors/nodes and see that they are all 3 synchronized. it's mildly cryptographic verification performed by a human (since doing so in an extreme manner would require you to sit all day every day typing in things on a calculator)
Osios::Osios(const QString &profileName, quint16 localServerPort_OrZeroToChooseRandomPort, ListOfDhtPeerAddressesAndPorts bootstrapAddressesAndPorts, QObject *parent)
    : QObject(parent)
    , m_ProfileName(profileName)
    , m_LastSerializedTimelineIndex(-1)
    , m_OsiosDht(new OsiosDht(this))
{
    QSettings settings;
    settings.beginGroup(PROFILES_GOUP_SETTINGS_KEY);
    settings.beginGroup(profileName);
    QString dataDir = settings.value(OSIOS_DATA_DIR_SETTINGS_KEY).toString(); //TODOoptional: sanitize empty/invalid filename etc. also the value needs to be set during profile creation (and the dir/filename needs to be queried)
    settings.endGroup();
    settings.endGroup();
    QString fileName = appendSlashIfNeeded(dataDir) + OSIOS_TIMELINE_FILENAME;
    qDebug() << "file path of timeline for current profile: " << fileName;
    m_LocalPersistenceDevice = new QFile(fileName, this);
    if(!m_LocalPersistenceDevice->open(QIODevice::ReadWrite))
    {
        qDebug() << "failed to open local persistence device" << fileName;
        //constructor, guh: emit notificationAvailable(Fatal);
        //TODOoptional: better handling
        return;
    }
    m_LocalPersistenceStream.setDevice(m_LocalPersistenceDevice);

    qRegisterMetaType<TimelineNode>("TimelineNode");

    //read in all previously serialized entries, which also puts the write cursor at the end for appending
    readInAllPreviouslySerializedEntries();

    int timelineSize = m_TimelineNodes.size();
    qDebug() << "Read in " << QString::number(timelineSize) << " previously serialized entries in the timeline";
    if(timelineSize == 0)
    {
        //no timeline nodes, so write zero size placeholder into file, which is rewound to and updated in our destructor
        m_LocalPersistenceStream << timelineSize;
    }

    QTimer *diskFlushTimer = new QTimer(this);
    connect(diskFlushTimer, SIGNAL(timeout()), this, SLOT(flushDiskBuffer()));
    diskFlushTimer->start(2*60*1000); //2 mins

    QTimer *checkRecentlyGeneratedTimelineNodesAwaitingCryptographicVerificationFromMoreNeighborsForTimedOutTimelineNodesTimer = new QTimer(this);
    connect(checkRecentlyGeneratedTimelineNodesAwaitingCryptographicVerificationFromMoreNeighborsForTimedOutTimelineNodesTimer, SIGNAL(timeout()), this, SLOT(checkRecentlyGeneratedTimelineNodesAwaitingCryptographicVerificationFromMoreNeighborsForTimedOutTimelineNodes()));
    checkRecentlyGeneratedTimelineNodesAwaitingCryptographicVerificationFromMoreNeighborsForTimedOutTimelineNodesTimer->start(1500 /*OSIOS_TIMELINE_NODE_CRYPTOGRAPHIC_HASH_NEIGHBOR_VERIFICATION_TIMEOUT_MILLISECONDS <-- nope. check every 1.5 seconds, if any are older than 5 seconds. i could give each byte array it's own timer, but nahhh */);

    connect(m_OsiosDht, SIGNAL(dhtStateChanged(OsiosDhtStates::OsiosDhtStatesEnum)), this, SLOT(handleDhtStateChanged(OsiosDhtStates::OsiosDhtStatesEnum)));

    //old, we now pass Osios into the dht so that it can connect us DIRECTLY to the dht peers signals (this commented out code is signal daisy chaning: peer -> dht -> osios
    //connect(m_OsiosDht, SIGNAL(timelineNodeReceivedFromPeer(OsiosDhtPeer*,TimelineNode)), this, SLOT(cyryptoNeighborReplicationVerificationStep1aOfX_WeReceiver_storeAndHashNeighborsActionAndRespondWithHash(OsiosDhtPeer*,TimelineNode)));
    //connect(m_OsiosDht,)

    QMetaObject::invokeMethod(m_OsiosDht, "bootstrap", Qt::QueuedConnection /*so we can connect first*/, Q_ARG(ListOfDhtPeerAddressesAndPorts, bootstrapAddressesAndPorts), Q_ARG(quint16, localServerPort_OrZeroToChooseRandomPort));
}
Osios::~Osios()
{
    //update the [probably, but not necessarily, changed] serialized number of timeline nodes at the beginning of the file (better, use a mutation append-only strategy)
    m_LocalPersistenceDevice->seek(0);
    m_LocalPersistenceStream << m_TimelineNodes.size();
    m_LocalPersistenceDevice->flush();
    qDeleteAll(m_TimelineNodes);
}
QList<TimelineNode> Osios::timelineNodes() const
{
    return m_TimelineNodes;
}
void Osios::readInAllPreviouslySerializedEntries()
{
    while(!m_LocalPersistenceStream.atEnd())
    {
        int numTimelineNodes;
        //if(m_LocalPersistenceStream.device()->bytesAvailable() < sizeof(numTimelineNodes)) -- not network so yea
        //    return;
        m_LocalPersistenceStream >> numTimelineNodes;
        int currentIndex = m_TimelineNodes.size()-1;
        for(int i = 0; i < numTimelineNodes; ++i)
        {
            ITimelineNode *serializedTimelineNode = deserializeNextTimelineNode();
            //m_LocalPersistenceStream >> serializedTimelineNode;
            m_TimelineNodes.append(serializedTimelineNode);
            ++currentIndex;
            m_LastSerializedTimelineIndex = currentIndex;
        }
    }
}
ITimelineNode *Osios::deserializeNextTimelineNode()
{
    //we have to return a pointer because we have to instantiate the derived type
    return TimelineSerializer::peekInstantiateAndDeserializeNextTimelineNodeFromIoDevice(m_LocalPersistenceDevice);
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
    m_LocalPersistenceStream << *action;
}
void Osios::flushDiskBuffer()
{
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
        if(recentlyGeneratedTimelineNodesAndTheirTimeoutTimestampsIterator.value() >= QDateTime::currentMSecsSinceEpoch())
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
    QByteArray timelineNodeRawByteArray = action->toByteArray();
    return QCryptographicHash::hash(timelineNodeRawByteArray, algorithm);
}
void Osios::recordMyAction(TimelineNode action)
{
    m_TimelineNodes.append(action); //takes ownership, deletes in this' destructor
    cyryptoNeighborReplicationVerificationStep0ofX_WeSender_propagateActionToNeighbors(action); //replicate it to network neighbors for cryptographic verification
    serializeMyTimelineAdditionsLocally(); //writes to file but does not flush. i could make this method the target of diskFlushTimer if i wanted, since the action being in m_TimelineNodes means it's recorded. I don't think it matters whether I buffer it or Qt does, since both are userland.
    emit timelineNodeAdded(action);
}
