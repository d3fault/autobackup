#include "osios.h"

#include <QSettings>
#include <QTimer>
#include <QFile>
#include <QDataStream>
#include <QDebug>

#include "osioscommon.h"
#include "timelineserializer.h"
#include "osiosdht.h"
#include "osiosdhtpeer.h"

#define OSIOS_TIMELINE_FILENAME "timeline.bin"

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
    m_LocalPersistenceDevice = new QFile(appendSlashIfNeeded(dataDir) + OSIOS_TIMELINE_FILENAME, this);
    if(!m_LocalPersistenceDevice->open(QIODevice::ReadWrite))
    {
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
        //TODOreq: error notification
    }
    //TODOreq: still need to do "sync" system call, but i don't think there's a portable way to do it :(
}
void Osios::handleDhtStateChanged(OsiosDhtStates::OsiosDhtStatesEnum newDhtState)
{
    switch(newDhtState)
    {
        case OsiosDhtStates::InitialIdleNoConnectionsState:
        {
            emit connectionColorChanged(Qt::black); //TO DOnereq(nop): disabling dht- (wait no you can't it's the whole purpose of the app). err was going to say that if the dht is disabled then black should always be shown...
            emit notificationAvailable(tr("DHT state changed to Initial Idle No Connections"));
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
        default /*wat*/:
        {
            emit connectionColorChanged(Qt::red);
            emit notificationAvailable(tr("The DHT changed states to indicate that there are NOT at least ") + QString::number(OSIOS_DHT_MIN_PEERS_CONNECTED_FOR_BOOTSTRAP_TO_BE_CONSIDERED_COMPLETE) + tr(" peers connected!"), OsiosNotificationLevels::ErrorNotificationLevel);
        }
            break;
    }
}
void Osios::cyryptoNeighborReplicationVerificationStep1aOfX_WeReceiver_storeAndHashNeighborsActionAndRespondWithHash(OsiosDhtPeer *osiosDhtPeer, TimelineNode action)
{
    //nothing too difficult here, but what 'profile' to use for the serialization is in question. is a dht and it's set of nodes associated with a profile? or should all of the incoming dhts be global to every local profile?
    //what i mean by "what profile" is basically I need a dht node id, and ip address isn't good enough. it needs to reflect their profile name perhaps. another way to solve this is to use public/private keys and to use the public keys as identifiers (this has other gains as well ofc, but I'm simply looking for simplest)

    replicateNeighborActionLocally(osiosDhtPeer, action); //TODOreq: use the same period flushing code as our own serializeMyTimelineAdditionsLocally (this is why a io thread is nice (however i'm not as thread safe as i could be dommit))

    cyryptoNeighborReplicationVerificationStep1bOfX_WeReceiver_hashNeighborsActionAndRespondWithHash(osiosDhtPeer, action);
}
void Osios::cyryptoNeighborReplicationVerificationStep2OfX_WeReceiver_handleResponseCryptoGraphicHashReceivedFromNeighbor(OsiosDhtPeer *osiosDhtPeer, QByteArray cryptoGraphicHashOfTimelineNodePreviouslySent)
{
    //TODOreq: now we check it against what we calculated for it earlier and (had:report errors) if the hash isn't found in our list of hashes watching for (meaning a mismatch, in which case we do nothing and the 5 second timeout will flash red etc). we should remove a hash from being watched once all (or N) of the neighbors respond with a valid cryptographic hash
}
void Osios::cyryptoNeighborReplicationVerificationStep0ofX_WeSender_propagateActionToNeighbors(TimelineNode action)
{
    //TODOreq: might be a good optimization to put everything network on a network thread, and maybe even all disk access on a disk thread. for now KISS everything goes on main thread. I am pretty sure Qt's APIs are async for me already anyways... so unless I above it, lockup is unlikely. HOWEVER this is still a good idea in general just because it will scale better in the future when maybe timeline nodes are much larger in size. It also isn't that difficult (would have taken about as much time as this comment)

    m_OsiosDht->sendNewTimelineNodeToAllDhtPeersWithHealthyConnectionForFirstStepOfCryptographicVerification(action);
}
void Osios::cyryptoNeighborReplicationVerificationStep1bOfX_WeReceiver_hashNeighborsActionAndRespondWithHash(OsiosDhtPeer *osiosDhtPeer, TimelineNode action)
{
    QByteArray sha1SumOfTimelineNode = calculateCrytographicHashOfTimelineNode(action);
    //now send it back to the neighbor for verification
    osiosDhtPeer->respondWithCryptographicHashComputedFromReceivedTimelineNode(sha1SumOfTimelineNode);
}
void Osios::replicateNeighborActionLocally(OsiosDhtPeer *osiosDhtPeer, TimelineNode action)
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
    serializeMyTimelineAdditionsLocally(); //writes to file but does not flush
}
