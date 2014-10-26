#include "osiosdht.h"

#include <QtNetwork/QTcpServer> //aww yee we are both client and server, <3 p2p
#include <QtNetwork/QTcpSocket>

#include "osios.h"
#include "osiosdhtpeer.h"
#include "osioscommon.h"

//for now the other 2 IPs must be hardcoded, there will be no IP address sharing (or sharding ;-P) yet just to KISS
//TODOoptional: some multicast broadcast thing would make a decent bootstrapping strategy, especially since this app is designed specifically for LAN
OsiosDht::OsiosDht(Osios *osios, QObject *parent)
    : QObject(parent)
    , m_Osios(osios)
    , m_DhtState(OsiosDhtStates::InitialIdleNoConnectionsState)
{ }
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
void OsiosDht::startLocalNetworkServer(quint16 localServerPort)
{
    m_LocalNetworkServer = new QTcpServer(this);
    if(!m_LocalNetworkServer->listen(QHostAddress::Any, localServerPort))
    {
        //TODOreq: notification error
        return;
    }

    //TODOreq: notification status "now listening on port X" (would be nice to also show external ip (although getting it with zero connections is errm wat), will settle for internal ip for now)
    connect(m_LocalNetworkServer, SIGNAL(newConnection()), this, SLOT(handleLocalNetworkServerNewIncomingConnection()));
}
void OsiosDht::setDhtState(const OsiosDhtStates::OsiosDhtStatesEnum &dhtState)
{
    if(dhtState != m_DhtState)
    {
        m_DhtState = dhtState;
        emit dhtStateChanged(dhtState);
    }
}
void OsiosDht::attemptToEstablishNetworkClientConnectionTo(DhtPeerAddressAndPort dhtPeerAddressAndPort)
{
    QTcpSocket *outgoingConnectionAttempt = new QTcpSocket(this);
    OsiosDhtPeer *dhtPeerWrapper = new OsiosDhtPeer(outgoingConnectionAttempt, outgoingConnectionAttempt); //TODOoptional: maybe put in m_PendingConnections list that is periodically purged. for now fuck it KISS
    connect(dhtPeerWrapper, SIGNAL(osiosDhtPeerConnected(OsiosDhtPeer*)), this, SLOT(handleOsiosDhtPeerConnected(OsiosDhtPeer*)));
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
void OsiosDht::bootstrap(ListOfDhtPeerAddressesAndPorts bootstrapHostAddresses, quint16 localServerPort)
{
    //a) start local tcp server for (incoming)
    startLocalNetworkServer(localServerPort);

    //b) try to connect to each of the bootstrap host addresses (outgoing)
    Q_FOREACH(DhtPeerAddressAndPort currentBootstrapHostAddress, bootstrapHostAddresses)
    {
        attemptToEstablishNetworkClientConnectionTo(currentBootstrapHostAddress);
    }

    setDhtState(OsiosDhtStates::BootstrappingForFirstTimeState);
}
void OsiosDht::handleLocalNetworkServerNewIncomingConnection()
{
    QTcpSocket *dhtPeerSocket = m_LocalNetworkServer->nextPendingConnection();
    OsiosDhtPeer *dhtPeerWrapper = new OsiosDhtPeer(dhtPeerSocket, dhtPeerSocket);
    connect(dhtPeerWrapper, SIGNAL(osiosDhtPeerConnected(OsiosDhtPeer*)), this, SLOT(handleOsiosDhtPeerConnected(OsiosDhtPeer*))); //the connected signal has already been emitted. the only reason i am connecting it is so that this incoming tcp socket is in an IDENTICAL state as the outgoing tcp sockets. This will matter when re-connect strategies are coded/designed
    handleOsiosDhtPeerConnected(dhtPeerWrapper); //like I said, the signal was already emitted (we are handling it right this very moment), so we call it manually just this once
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
