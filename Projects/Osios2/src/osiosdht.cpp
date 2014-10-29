#include "osiosdht.h"

#include <QtNetwork/QTcpServer> //aww yee we are both client and server, <3 p2p
#include <QtNetwork/QTcpSocket>
#include <QDateTime>
#include <QTimer>
#include <QMapIterator>

#include "osios.h"
#include "osiosdhtpeer.h"
#include "osioscommon.h"

#define OSIOS_DHT_MIN_TIME_TO_WAIT_BEFORE_RETRYING_CONNECTION_MS (100)
#define OSIOS_DHT_MAX_AMOUNT_OF_TIME_TO_WAIT_BEFORE_CHECKING_LIST_FOR_PEERS_TO_RETRY_CONNECTING_TO_MS (60*1000)

//for now the other 2 IPs must be hardcoded, there will be no IP address sharing (or sharding ;-P) yet just to KISS
//TODOoptional: some multicast broadcast thing would make a decent bootstrapping strategy, especially since this app is designed specifically for LAN
OsiosDht::OsiosDht(Osios *osios, QObject *parent)
    : QObject(parent)
    , m_Osios(osios)
    , m_DhtState(OsiosDhtStates::InitialIdleNoConnectionsState)
    , m_RetryConnectionAndSayHelloToWithExponentialBackoffTimer(new QTimer(this))
{
    connect(this, SIGNAL(notificationAvailable(QString,OsiosNotificationLevels::OsiosNotificationLevelsEnum)), m_Osios, SIGNAL(notificationAvailable(QString,OsiosNotificationLevels::OsiosNotificationLevelsEnum)));

    connect(m_RetryConnectionAndSayHelloToWithExponentialBackoffTimer, SIGNAL(timeout()), this, SLOT(handleRetryConnectionAndSayHelloToWithExponentialBackoffTimerTimedOut()));
    m_RetryConnectionAndSayHelloToWithExponentialBackoffTimer->start(420); //arbitrary starting point, we'll calculate to a tad after the next expiring one after each checking of the list (with an upper limit for how often to check said list to see if enough time has elapsed for an exponential backoffs is 60 seconds)
}
void OsiosDht::sendNewTimelineNodeToAllDhtPeersWithHealthyConnectionForFirstStepOfCryptographicVerification(TimelineNode action)
{
    //fuck the dht's state, we'll send to whoever is connected!
    Q_FOREACH(OsiosDhtPeer *currentPeer, m_DhtPeersWithHealthyConnection)
    {
        currentPeer->sendNewTimelineNodeForFirstStepOfCryptographicVerification(action);
#if 0 //comment valid
        if(!currentPeer->sendNewTimelineNodeForFirstStepOfCryptographicVerification(action))
        {
            //TODOreq: error notification. also use an extremely simple timeout function (for the timeline node being added) that also triggers error notification
        }
#endif
    }
}
OsiosDhtStates::OsiosDhtStatesEnum OsiosDht::dhtState() const
{
    return m_DhtState;
}
void OsiosDht::setDhtState(const OsiosDhtStates::OsiosDhtStatesEnum &dhtState)
{
    if(dhtState != m_DhtState)
    {
        m_DhtState = dhtState;
        emit dhtStateChanged(dhtState);
    }
}
quint16 OsiosDht::generateRandomPort()
{
    return static_cast<quint16>((qrand() % static_cast<int>(65536-1024))+1024);
}
void OsiosDht::startLocalNetworkServer(quint16 localServerPort)
{
    m_LocalNetworkServer = new QTcpServer(this);
    bool portSuppliedByUser = (localServerPort > 0);
    if(!portSuppliedByUser)
    {
        qsrand(QDateTime::currentMSecsSinceEpoch());
        localServerPort = generateRandomPort();
    }
    quint8 portListenAttempts = 0;
    quint8 maxPortListenAttempts = 255;
    while(!m_LocalNetworkServer->listen(QHostAddress::Any, localServerPort))
    {
        if(portSuppliedByUser)
        {
            emit notificationAvailable(tr("DHT failed to listen on port ") + QString::number(localServerPort), OsiosNotificationLevels::FatalNotificationLevel);
            return;
        }
        if(++portListenAttempts == maxPortListenAttempts)
        {
            emit notificationAvailable(tr("DHT failed to find a free port after trying to listen on 256 random different ports. Try supplying one with the --listen-port arg"), OsiosNotificationLevels::FatalNotificationLevel);
            return;
        }
        localServerPort = generateRandomPort();
    }

    emit notificationAvailable(tr("DHT now listening on port ") + QString::number(localServerPort));
    //TODOreq: would be nice to also show external ip (although getting it with zero connections is errm wat), will settle for internal ip for now
    connect(m_LocalNetworkServer, SIGNAL(newConnection()), this, SLOT(handleLocalNetworkServerNewIncomingConnection()));
}
void OsiosDht::attemptToEstablishNetworkClientConnectionTo(DhtPeerAddressAndPort dhtPeerAddressAndPort)
{
    QTcpSocket *outgoingConnectionAttempt = new QTcpSocket(this);
    outgoingConnectionAttempt->connectToHost(dhtPeerAddressAndPort.first, dhtPeerAddressAndPort.second);
    OsiosDhtPeer *dhtPeerWrapper = new OsiosDhtPeer(outgoingConnectionAttempt, outgoingConnectionAttempt->peerName(), outgoingConnectionAttempt); //TODOoptional: maybe put in m_PendingConnections list that is periodically purged. for now fuck it KISS
    connect(dhtPeerWrapper, SIGNAL(osiosDhtPeerConnected(OsiosDhtPeer*)), this, SLOT(handleOsiosDhtPeerConnected(OsiosDhtPeer*)));
}
void OsiosDht::maybeChangeStateBecauseThereWasNewConnection()
{
    if(m_DhtState == OsiosDhtStates::BootstrappedState)
        return; //optimization, since we may have bootstrapped years ago! new connection when already bootstrapped, dgaf. in the equivalent maybeChangeStateBecauseThereDisconnection, I need to make sure we still have enough peers and might change to FellBelowMinBootstrapNodesState

    if(m_DhtState == OsiosDhtStates::BootstrappingForFirstTimeState || m_DhtState == OsiosDhtStates::FellBelowMinBootstrapNodesState)
    {
        if(m_DhtPeersWithHealthyConnection.size() >= OSIOS_DHT_MIN_PEERS_CONNECTED_FOR_BOOTSTRAP_TO_BE_CONSIDERED_COMPLETE)
        {
            setDhtState(OsiosDhtStates::BootstrappedState);
            return;
        }
    }
}
void OsiosDht::maybeChangeStateBecauseThereWasADisconnection()
{
    //TODOreq: call this from places. mb connect it (indirectly) to signals connected to QTcpSocket::disconnected and possibly even QTcpSocket::error. maybe error triggers disconnected? or maybe it triggers hello. there is no such thing as disconnected, since we retry with exponential backoff. disconnect with no attempt to reconnect is maybe possible, but that doesn't mean other nodes in the dht won't still say hello to your IP as part of their exponential backoff procedure.

    if(m_DhtState == OsiosDhtStates::BootstrappingForFirstTimeState || m_DhtState == OsiosDhtStates::CleanlyDisconnectingOutgoingNotAcceptingnewIncomingConnectionsState)
        return;

    if(m_DhtPeersWithHealthyConnection.size() < OSIOS_DHT_MIN_PEERS_CONNECTED_FOR_BOOTSTRAP_TO_BE_CONSIDERED_COMPLETE)
    {
        setDhtState(OsiosDhtStates::FellBelowMinBootstrapNodesState);
    }
}
void OsiosDht::bootstrap(ListOfDhtPeerAddressesAndPorts bootstrapHostAddresses, quint16 localServerPort_OrZeroToChooseRandomPort)
{
    //a) start local tcp server for (incoming)
    startLocalNetworkServer(localServerPort_OrZeroToChooseRandomPort);

    //b) try to connect to each of the bootstrap host addresses (outgoing)
    Q_FOREACH(DhtPeerAddressAndPort currentBootstrapHostAddress, bootstrapHostAddresses)
    {
        //attemptToEstablishNetworkClientConnectionTo(currentBootstrapHostAddress);
        m_PotentialOrOldDhtPeersToConnectAndSayHelloToRetryingWithExponentialBackoff.insert(0, qMakePair(1, currentBootstrapHostAddress.first.toString() + ":" + QString::number(currentBootstrapHostAddress.second)));
    }
    handleRetryConnectionAndSayHelloToWithExponentialBackoffTimerTimedOut(); //get things going, don't wait for first timeout

    setDhtState(OsiosDhtStates::BootstrappingForFirstTimeState);
}
void OsiosDht::handleRetryConnectionAndSayHelloToWithExponentialBackoffTimerTimedOut()
{
    if(m_PotentialOrOldDhtPeersToConnectAndSayHelloToRetryingWithExponentialBackoff.isEmpty())
        return;

    //we should use a map with the timeout as the key so it is sorted async for us. as soon as we reach a key that does not need a retry for exponential backoff, we stop iterating the list and then restart our timer for that amount. iterating the entire list could be[come] expensive
    QMapIterator<OSIOS_DHT_MAP_TYPE_OF_PEERS_TO_TRY_CONNECTING_TO_RETRYING_WITH_EXPONENTIAL_BACKOFF> mapIterator(m_PotentialOrOldDhtPeersToConnectAndSayHelloToRetryingWithExponentialBackoff);
    qint64 durationUntilFirstNextConnectionAttempt_OrNegativeOneIfWeRetriedAllInTheList = -1;
    while(mapIterator.hasNext())
    {
        //keep iterating/trying-connection until we encounter our first peer in the map that isn't ready for connection attempt, then set the timer to just a tad after when he wants to connect

        mapIterator.next();
        if(mapIterator.key() <= QDateTime::currentMSecsSinceEpoch())
        {
            attemptToEstablishNetworkClientConnectionTo(mapIterator.value().second);
            continue;
        }
        durationUntilFirstNextConnectionAttempt_OrNegativeOneIfWeRetriedAllInTheList =
                qMin(
                      (mapIterator.key() - QDateTime::currentMSecsSinceEpoch())
                    , static_cast<qint64>(OSIOS_DHT_MAX_AMOUNT_OF_TIME_TO_WAIT_BEFORE_CHECKING_LIST_FOR_PEERS_TO_RETRY_CONNECTING_TO_MS)
                    );
        break;
    }
    if(durationUntilFirstNextConnectionAttempt_OrNegativeOneIfWeRetriedAllInTheList > -1)
    {
        m_RetryConnectionAndSayHelloToWithExponentialBackoffTimer->start(durationUntilFirstNextConnectionAttempt_OrNegativeOneIfWeRetriedAllInTheList); //restart timer to be more accurate. the timer itself follows exponential backoff as well :-D (the lowest interval possible)
    }
}
void OsiosDht::handleLocalNetworkServerNewIncomingConnection()
{
    QTcpSocket *dhtPeerSocket = m_LocalNetworkServer->nextPendingConnection();
    OsiosDhtPeer *dhtPeerWrapper = new OsiosDhtPeer(dhtPeerSocket, dhtPeerSocket->peerName(), dhtPeerSocket);
    connect(dhtPeerWrapper, SIGNAL(osiosDhtPeerConnected(OsiosDhtPeer*)), this, SLOT(handleOsiosDhtPeerConnected(OsiosDhtPeer*))); //the connected signal has already been emitted. the only reason i am connecting it is so that this incoming tcp socket is in an IDENTICAL state as the outgoing tcp sockets. This will matter when re-connect strategies are coded/designed
    connect(dhtPeerWrapper, SIGNAL(connectionNoLongerConsideredGood(OsiosDhtPeer*)), this, SLOT(handleConnectionNoLongerConsideredGood(OsiosDhtPeer*)));
    handleOsiosDhtPeerConnected(dhtPeerWrapper); //like I said, the connected signal was already emitted (we are handling it right this very moment), so we call it manually just this once
}
void OsiosDht::handleOsiosDhtPeerConnected(OsiosDhtPeer *newDhtPeer)
{
    m_DhtPeersWithHealthyConnection.append(newDhtPeer);
    //connect(newDhtPeer, SIGNAL(timelineNodeReceivedFromPeer(OsiosDhtPeer*,TimelineNode)), this, SIGNAL(timelineNodeReceivedFromPeer(OsiosDhtPeer*,TimelineNode))); //TO DOneoptimization(saves me keystrokes more than anything): connect osios and the peer directly
    //connect(newDhtPeer, SIGNAL())
    connect(newDhtPeer, SIGNAL(timelineNodeReceivedFromPeer(OsiosDhtPeer*,TimelineNode)), m_Osios, SLOT(cyryptoNeighborReplicationVerificationStep1aOfX_WeReceiver_storeAndHashNeighborsActionAndRespondWithHash(OsiosDhtPeer*,TimelineNode)));
    connect(newDhtPeer, SIGNAL(responseCryptoGraphicHashReceivedFromNeighbor(OsiosDhtPeer*,QByteArray)), m_Osios, SLOT(cyryptoNeighborReplicationVerificationStep2OfX_WeReceiver_handleResponseCryptoGraphicHashReceivedFromNeighbor(OsiosDhtPeer*,QByteArray)));
    maybeChangeStateBecauseThereWasNewConnection();
}
void OsiosDht::handleConnectionNoLongerConsideredGood(OsiosDhtPeer *oldDhtPeer)
{
    m_DhtPeersWithHealthyConnection.removeOne(oldDhtPeer);
    m_PotentialOrOldDhtPeersToConnectAndSayHelloToRetryingWithExponentialBackoff.insert(QDateTime::currentMSecsSinceEpoch() + OSIOS_DHT_MIN_TIME_TO_WAIT_BEFORE_RETRYING_CONNECTION_MS, qMakePair(1, oldDhtPeer->peerConnectionInfo())); //TODOreq: impl exponential backoff
    //TODOmb: disconnect signals to/from the peer that might be emitted before deleteLayer is processed? is that even how it works?
    maybeChangeStateBecauseThereWasADisconnection(); //connected/disconnected is measured by the dht solely by m_DhtPeersWithHealthyConnection. bootstrapped or not is determiend by any timeline nodes not cryptographically verifying within 5 seconds
}
