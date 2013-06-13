#include "multiserverclientabstraction.h"

MultiServerClientAbstraction::MultiServerClientAbstraction(IProtocolKnowerFactory *protocolKnowerFactory, QObject *parent)
    : QObject(parent)
{
    AbstractServerConnection::setMultiServerClientAbstraction(this);
    AbstractServerConnection::setProtocolKnowerFactory(protocolKnowerFactory);
}
MultiServerClientAbstraction::~MultiServerClientAbstraction()
{
    //TODOreq: IDFK what, but dc/delete shit sounds about right...
}
void MultiServerClientAbstraction::setupSocketSpecificDisconnectAndErrorSignaling(QIODevice *ioDeviceToClient, AbstractServerConnection *abstractClientConnection)
{
    QSslSocket *sslSocket_ifThatType_orZero = qobject_cast<QSslSocket*>(ioDeviceToClient);

    if(sslSocket_ifThatType_orZero)
    {
        //Ssl socket
        setupSslSocketSpecificErrorConnections(sslSocket_ifThatType_orZero, abstractClientConnection);
    }
    else
    {
        //Not Ssl Socket, so now try tcp only

        QTcpSocket *tcpSocket_ifThatType_orZero = qobject_cast<QTcpSocket*>(ioDeviceToClient);

        if(tcpSocket_ifThatType_orZero)
        {
            //Tcp Socket
            setupQAbstractSocketSpecificErrorConnections(tcpSocket_ifThatType_orZero, abstractClientConnection);
        }
        else
        {
            //Not Tcp Socket. In the future we could check for UDP or something else (threads?), but for now our 3rd is going to be localsocket --- but I mean udp is also abstract so it might just get set up if cast-able to qabstract socket at same time as tcp

            QLocalSocket *localSocket_ifThatType_orZero = qobject_cast<QLocalSocket*>(ioDeviceToClient);

            if(localSocket_ifThatType_orZero)
            {
                //Local Socket
                setupQLocalSocketSpecificErrorConnections(localSocket_ifThatType_orZero, abstractClientConnection);
            }
            else
            {
                //Not Local Socket, so give error [for now]
                emit d("Couldn't determine what kind of socket type for setting up errors");
                abstractClientConnection->makeConnectionBad(); //fuck it why not
            }

        }
    }
}
void MultiServerClientAbstraction::connectionDoneHelloing(AbstractServerConnection *abstractClientConnection)
{
    //TODOreq: It makes sense that we would then notify the business or something/someone that the rpc service is now ready to be used. We need at least 1 connection for that to be true. By ready, I mean: Action Requests can now be.... requested...
    //^^^(the business is expected to ALREADY be ready for broadcasts before that notification, BUT realistically none will come until we have at least 1 connection)
    m_ListOfHelloedConnections.append(abstractClientConnection);

    if(!m_ReadyForActionRequests)
    {
        m_ReadyForActionRequests = true;
        emit readyForActionRequests();
    }
}
AbstractServerConnection *MultiServerClientAbstraction::getSuitableClientConnectionNextInRoundRobinToUseForBroadcast_OR_Zero()
{
    if(m_RoundRobinQueue.length() < 1)
    {
        //needs refill
        refillRoundRobinFromHellodConnections();
    }

    //did refill succeed? we might not have any clients so don't try if that's the case
    if(m_RoundRobinQueue.length() > 0)
    {
        return m_RoundRobinQueue.dequeue();
    }

    return 0;
}
void MultiServerClientAbstraction::reportConnectionDestroying(AbstractServerConnection *connectionBeingDestroyed)
{
    int numRemoved = m_ListOfConnectionsIgnoringHelloState.removeAll(connectionBeingDestroyed);
    if(numRemoved != 1)
    {
        //TODO errors n shit, this should never happen
    }
}
void MultiServerClientAbstraction::initializeAndStartConnections(MultiServerClientsAbstractionArgs multiServerClientAbstractionArgs)
{
    //m_MultiServerClientAbstractionArgs = multiServerClientAbstractionArgs;
    int currentProtocolConnectionCount = multiServerClientAbstractionArgs.SslTcpClientsArgs.size();
    for(int i = 0; i < currentProtocolConnectionCount; ++i)
    {
        SslTcpClient *sslTcpClient = new SslTcpClient(this);
        connect(sslTcpClient, SIGNAL(d(const QString &)), this, SIGNAL(d(const QString &)));
        connect(sslTcpClient, SIGNAL(connectedAndEncrypted(QSslSocket*)), this, SLOT(handleConnectedToSslServer(QSslSocket*)));
        sslTcpClient->initialize(multiServerClientAbstractionArgs.SslTcpClientsArgs.at(i));
        //AbstractServerConnection *abstractServerConnection = new AbstractServerConnection(sslTcpClient, this);
        //m_ListOfConnectionsIgnoringHelloState.append(abstractServerConnection);
        m_ListOfIODevicesIgnoringConnectionAndHelloState.append(sslTcpClient);
        sslTcpClient->start();
    }
    currentProtocolConnectionCount = multiServerClientAbstractionArgs.TcpClientsArgs.size();
    for(int i = 0; i < currentProtocolConnectionCount; ++i)
    {
        //TODOreq
    }
    currentProtocolConnectionCount = multiServerClientAbstractionArgs.LocalServersNames.size();
    for(int i = 0; i < currentProtocolConnectionCount; ++i)
    {
        //TODOreq
    }
}
void MultiServerClientAbstraction::start()
{
    //TODOreq
}
void MultiServerClientAbstraction::stop()
{
    //TODOreq
}
AbstractServerConnection *MultiServerClientAbstraction::handleNewConnectionToServer(QIODevice *newClient)
{
    emit d("new server connection established, starting hello phase");

    AbstractServerConnection *newClientConnection = new AbstractServerConnection(newClient, this);
    m_ListOfConnectionsIgnoringHelloState.append(newClientConnection);
    return newClientConnection;
}
void MultiServerClientAbstraction::dontBroadcastTo(AbstractServerConnection *abstractClientConnection)
{
    int numRemoved = m_ListOfHelloedConnections.removeAll(abstractClientConnection);

    if(numRemoved == 1)
    {
        if(m_RoundRobinQueue.contains(abstractClientConnection))
        {
            numRemoved = m_RoundRobinQueue.removeAll(abstractClientConnection);
            if(numRemoved != 1)
            {
                //we had more than 1 of the same in the queue, which means our round robin code is fucked... should never happen if round robin code (etc, because we build it from m_ListOfHelloedConnections) is clean/correct
            }
        }
    }
    else
    {
        //wtf should never happen for same reason above!
    }
}
void MultiServerClientAbstraction::refillRoundRobinFromHellodConnections()
{
    QList<int> indexesRemaining;

    int hellodConnectionsSize = m_ListOfHelloedConnections.length();
    for(int i = 0; i < hellodConnectionsSize; ++i)
    {
        indexesRemaining.append(i);
    }

    //TODOoptimization: don't call .length() so fucking much (code is in server also). This refill method will be called pretty often tbh
    while(indexesRemaining.length() > 0)
    {
        int randomIndexIntoIndexList = (qrand() % indexesRemaining.length());
        int indexChosenRandomlyFromIndexList = indexesRemaining.takeAt(randomIndexIntoIndexList);
        m_RoundRobinQueue.enqueue(m_ListOfHelloedConnections.at(indexChosenRandomlyFromIndexList));
    }
}
void MultiServerClientAbstraction::setupQAbstractSocketSpecificErrorConnections(QAbstractSocket *abstractSocket, AbstractServerConnection *abstractServerConnection)
{
    connect(abstractSocket, SIGNAL(error(QAbstractSocket::SocketError)), abstractServerConnection, SLOT(makeConnectionBad()));
    connect(abstractSocket, SIGNAL(disconnected()), abstractServerConnection, SLOT(makeConnectionBad()));
    connect(abstractSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), abstractServerConnection, SLOT(makeConnectionBadIfNewSocketStateSucks(QAbstractSocket::SocketState)));
}
void MultiServerClientAbstraction::setupSslSocketSpecificErrorConnections(QSslSocket *sslSocket, AbstractServerConnection *abstractServerConnection)
{
    setupQAbstractSocketSpecificErrorConnections(sslSocket, abstractServerConnection);

    connect(sslSocket, SIGNAL(sslErrors(QList<QSslError>)), abstractServerConnection, SLOT(makeConnectionBad()));
}
void MultiServerClientAbstraction::setupQLocalSocketSpecificErrorConnections(QLocalSocket *localSocket, AbstractServerConnection *abstractServerConnection)
{
    connect(localSocket, SIGNAL(error(QLocalSocket::LocalSocketError)), abstractServerConnection, SLOT(makeConnectionBad()));
    connect(localSocket, SIGNAL(disconnected()), abstractServerConnection, SLOT(makeConnectionBad()));
    connect(localSocket, SIGNAL(stateChanged(QLocalSocket::LocalSocketState)), abstractServerConnection, SLOT(makeConnectionBadIfNewQLocalSocketStateSucks(QLocalSocket::LocalSocketState)));
}
void MultiServerClientAbstraction::handleConnectedToSslServer(QSslSocket *sslSocketToServer)
{
    AbstractServerConnection *abstractClientConnectionSoICanConnectToSocketTypeSpecificDisconnectSignals = handleNewConnectionToServer(sslSocketToServer);

    setupSslSocketSpecificErrorConnections(sslSocketToServer, abstractClientConnectionSoICanConnectToSocketTypeSpecificDisconnectSignals);
}
