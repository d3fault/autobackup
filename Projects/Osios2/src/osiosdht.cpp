#include "osiosdht.h"

#include <QtNetwork/QTcpServer> //aww yee we are both client and server, <3 p2p
#include <QtNetwork/QTcpSocket>
#include <QDateTime>
#include <QTimer>
#include <QMapIterator>
#include <QMutableMapIterator>

#include "osios.h"
#include "osiosdhtpeer.h"
#include "osioscommon.h"
#include "osiossettings.h"

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
    //we don't rely on dht state, we'll send to whoever is connected! dht state is DERMINED by the success of my sends!
    Q_FOREACH(OsiosDhtPeer *currentPeer, m_DhtPeersWithHealthyConnection)
    {
        currentPeer->sendNewTimelineNodeForFirstStepOfCryptographicVerification(action); //TODOreq: i am getting occassional segfaults here, i think i am deleting an object/socket without removing from m_DhtPeersWithHealthyConnection maybe? MIGHT be related to other errors i'm encountering though
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
//spread your legs
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
//pop a boner
void OsiosDht::attemptToEstablishOutgoingNetworkClientConnectionTo(DhtPeerAddressAndPort dhtPeerAddressAndPort)
{
    qDebug() << "Attempting new outgoing connection to:" << QString(dhtPeerAddressAndPort.first.toString() + ":" + QString::number(dhtPeerAddressAndPort.second));
    QTcpSocket *outgoingConnectionAttempt = new QTcpSocket(this);
    OsiosDhtPeer *dhtPeerWrapper = new OsiosDhtPeer(outgoingConnectionAttempt, dhtPeerAddressAndPort, outgoingConnectionAttempt); //TODOoptional: maybe put in m_PendingConnections list that is periodically purged. for now fuck it KISS
    connect(dhtPeerWrapper, SIGNAL(osiosDhtPeerConnected(OsiosDhtPeer*)), this, SLOT(handleOsiosDhtPeerConnected(OsiosDhtPeer*)));
    //old, befor realizing i shouldn't put it in the retry-with-exponential-backoff list until after it fails to connect within a certain amount of time, and determining that is the job of abstract socket! connect(dhtPeerWrapper, SIGNAL(osiosDhtPeerConnected(OsiosDhtPeer*)), this, SLOT(removeDhtPeerFrom either exponential backoff retry list, or pending connection list. is there a difference? my minimum exponential timeout time should be greater than or equal to my 'pending connection list' timeout time, so why bother with two lists in the first place? i think we should call close() on the first connection, then re-call connectoToHost on the same abstract socket. at the very least it will save us memory rather than creating a new tcp socket every 420 ms. ok i need to differentiate between tcp socket connect attemps timing out vs exponential backoff. exponential backoff being how long we wait AFTER it times out. so that means we don't have the tcp socket around any longer when it comes time to retry (this is reflected in my code too)))); ---- AM I RESPONSIBLE FOR TIMING OUT TCP SOCKET, OR WILL EITHER ERROR OR CONNECTED ALWAYS BE EMITTED WITHIN SOME PREDEFINED (hopefully customizable) TIME? keep in mind that i'm already timing out the timeline nodes individually, so what I'm going might be reinventing the wheel, i'im unsure atm. no nvm connections list is different from timeline nodes list. I say fuck exponential backoff for now, 420ms is fine so long as i don't double connect-attempt [at any interval]
    outgoingConnectionAttempt->connectToHost(dhtPeerAddressAndPort.first, dhtPeerAddressAndPort.second);
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
void OsiosDht::maybeChangeStateBecauseThereWasAConnectionThatStoppedBeingConsideredHealthy()
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
#if OSIOS_DHT_MIN_PEERS_CONNECTED_FOR_BOOTSTRAP_TO_BE_CONSIDERED_COMPLETE < 1 //TODOoptional: eh this hack works in that it gets us to the main window, but no timeline actions are shown in the timeline widget (neither new nor deserialized). this is low priority to fix since it's not the purpose of the app
    //compile time hack to get dht to bootstrap (and therefore, make the app usable) with 0 neighbors. useful for testing. in that case we serialize a local timeline.bin and that's it
    setDhtState(OsiosDhtStates::BootstrappedState);
    return;
#endif

    setDhtState(OsiosDhtStates::BootstrappingForFirstTimeState);

    //a) start local tcp server for (incoming)
    startLocalNetworkServer(localServerPort_OrZeroToChooseRandomPort);

    //b) try to connect to each of the bootstrap host addresses (outgoing)
    Q_FOREACH(DhtPeerAddressAndPort currentBootstrapHostAddress, bootstrapHostAddresses)
    {
        attemptToEstablishOutgoingNetworkClientConnectionTo(currentBootstrapHostAddress); //TO DOnereq: since both incoming and outgoing errors are handled in the same slot, OH NVM MY RETRY LIST IS ALREADY "socket-independent", and trying to outgoing connect to them is something we want to do, so...
        //we want to connect right away, that list is for 'connect later' m_PotentialOrOldDhtPeersToConnectAndSayHelloToRetryingWithExponentialBackoff.insert(0, qMakePair(1, currentBootstrapHostAddress));
    }
    //old goes with the call to m_PotentialOrOldDhtPeersToConnectAndSayHelloToRetryingWithExponentialBackoff.insert that is commented out above: handleRetryConnectionAndSayHelloToWithExponentialBackoffTimerTimedOut(); //get things going, don't wait for first timeout
}
void OsiosDht::handleRetryConnectionAndSayHelloToWithExponentialBackoffTimerTimedOut()
{
    if(m_PotentialOrOldDhtPeersToConnectAndSayHelloToRetryingWithExponentialBackoff.isEmpty()) //TODOoptional: start/stop the timer on list insert/remove instead. TODOreq: at the very least we don't want our retry timeout to be stuck at some extremely low interval (i think i plan to use a minimum so...)
        return;

    //we use a map with the timeout as the key so it is sorted async for us when the disconnected/error/whatever signals are emitted. as soon as we reach a key that does not need a retry for exponential backoff, we stop iterating the list and then restart our timer for that amount+a-tad. iterating the entire list could be[come] expensive
    QMutableMapIterator<OSIOS_DHT_MAP_TYPE_OF_PEERS_TO_TRY_CONNECTING_TO_RETRYING_WITH_EXPONENTIAL_BACKOFF> mapIterator(m_PotentialOrOldDhtPeersToConnectAndSayHelloToRetryingWithExponentialBackoff);
    qint64 durationUntilFirstNextConnectionAttempt_OrNegativeOneIfWeRetriedAllInTheList = -1;
    while(mapIterator.hasNext())
    {
        //keep iterating/trying-connection until we encounter our first peer in the map that isn't ready for connection attempt, then set the timer to just a tad after when he wants to connect

        mapIterator.next();
        if(mapIterator.key() <= QDateTime::currentMSecsSinceEpoch())
        {
            attemptToEstablishOutgoingNetworkClientConnectionTo(mapIterator.value().second);
            mapIterator.remove(); //we'll put it back in if/when the connect attempt fails

            //TODOreq: in order to get exponential backoff to work, I need to KEEP A RECORD OF THEM (them being the just-removed above) and their current exponent. Qt's timeout will be constant (30s? arbitrary who cares), mine will have exponential backoff. 420ms is good enough for now/testing

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
        m_RetryConnectionAndSayHelloToWithExponentialBackoffTimer->start(durationUntilFirstNextConnectionAttempt_OrNegativeOneIfWeRetriedAllInTheList+OSIOS_DHT_TIME_TO_OVERSHOOT_A_RETRYWITHEXPONENTIALBACKOFF_TIMEOUT_WHEN_WE_WANT_TO_HIT_ON_NEXT_TIMEOUT_MS); //restart timer to be more accurate. the timer itself follows exponential backoff as well :-D (the lowest interval possible (TODOreq: the interval of course changes again when new peer info is inserted into the map))
    }
}
void OsiosDht::handleLocalNetworkServerNewIncomingConnection()
{
    QTcpSocket *dhtPeerSocket = m_LocalNetworkServer->nextPendingConnection();
    OsiosDhtPeer *dhtPeerWrapper = new OsiosDhtPeer(dhtPeerSocket, qMakePair(dhtPeerSocket->peerAddress(), dhtPeerSocket->peerPort()) /* TODOreq: returning empty string for host and -1 for port. This just means I won't be able to try an outgoing attempt to connect to them [with exponential backoff] should they ever dc */, dhtPeerSocket);
    qDebug() << "New incoming connection (Hello handshake NOT yet complete) from:" << QString(dhtPeerWrapper->peerConnectionInfo().first.toString() + ":" + QString::number(dhtPeerWrapper->peerConnectionInfo().second));
    connect(dhtPeerWrapper, SIGNAL(osiosDhtPeerConnected(OsiosDhtPeer*)), this, SLOT(handleOsiosDhtPeerConnected(OsiosDhtPeer*))); //the connected signal has already been emitted. the only reason i am connecting it is so that this incoming tcp socket is in an IDENTICAL state as the outgoing tcp sockets. This will matter when re-connect strategies are coded/designed
    connect(dhtPeerWrapper, SIGNAL(connectionNoLongerConsideredGood(OsiosDhtPeer*)), this, SLOT(handleConnectionNoLongerConsideredGood(OsiosDhtPeer*)));
    handleOsiosDhtPeerConnected(dhtPeerWrapper); //like I said, the connected signal was already emitted (we are handling it right this very moment), so we call it manually just this once
}
void OsiosDht::handleOsiosDhtPeerConnected(OsiosDhtPeer *newDhtPeer)
{
    qDebug() << "New healthy connection (Hello handshake COMPLETE) to/from:" << QString(newDhtPeer->peerConnectionInfo().first.toString() + ":" + QString::number(newDhtPeer->peerConnectionInfo().second));
    if(m_DhtState != OsiosDhtStates::BootstrappedState)
    {
        //TODOoptional: could put this if block in it's own dedicated slot connected to signal 'osiosDhtPeerConnected' that is disconnected on/at bootstrap. small optimization methinks, also arguably cleaner
        emit notificationAvailable("New DHT peer connected: " + newDhtPeer->peerConnectionInfo().first.toString() /*host*/ + ":" + QString::number(newDhtPeer->peerConnectionInfo().second /*port*/));
    }

    m_DhtPeersWithHealthyConnection.append(newDhtPeer);

    connect(newDhtPeer, SIGNAL(timelineNodeReceivedFromPeer(OsiosDhtPeer*,TimelineNode)), m_Osios, SLOT(cyryptoNeighborReplicationVerificationStep1aOfX_WeReceiver_storeAndHashNeighborsActionAndRespondWithHash(OsiosDhtPeer*,TimelineNode)));
    connect(newDhtPeer, SIGNAL(responseCryptoGraphicHashReceivedFromNeighbor(OsiosDhtPeer*,QByteArray)), m_Osios, SLOT(cyryptoNeighborReplicationVerificationStep2OfX_WeReceiver_handleResponseCryptoGraphicHashReceivedFromNeighbor(OsiosDhtPeer*,QByteArray)));
    maybeChangeStateBecauseThereWasNewConnection();
}
void OsiosDht::handleConnectionNoLongerConsideredGood(OsiosDhtPeer *oldDhtPeer)
{
    m_DhtPeersWithHealthyConnection.removeOne(oldDhtPeer);
    m_PotentialOrOldDhtPeersToConnectAndSayHelloToRetryingWithExponentialBackoff.insert(QDateTime::currentMSecsSinceEpoch() + OSIOS_DHT_MIN_TIME_TO_WAIT_BEFORE_RETRYING_CONNECTION_MS, qMakePair(1, oldDhtPeer->peerConnectionInfo())); //TODOreq: impl exponential backoff
    //TODOmb: disconnect signals to/from the peer that might be emitted before deleteLayer is processed? is that even how it works?
    maybeChangeStateBecauseThereWasAConnectionThatStoppedBeingConsideredHealthy(); //connected/disconnected is measured by the dht solely by m_DhtPeersWithHealthyConnection. bootstrapped or not is determiend by any timeline nodes not cryptographically verifying within 5 seconds
}
