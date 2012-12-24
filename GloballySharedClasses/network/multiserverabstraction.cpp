#include "multiserverabstraction.h"

quint32 ServerHelloStatus::overflowingClientIdsUsedAsInputForMd5er = 0;

MultiServerAbstraction::MultiServerAbstraction(IMultiServerBusiness *serverBusiness, QObject *parent)
    : QObject(parent), m_SslTcpServer(0) /* etc tcp local */
{
    AbstractClientConnection::setServerBusiness(serverBusiness);
    deletePointersAndSetEachFalse();
}
MultiServerAbstraction::~MultiServerAbstraction()
{
    //TODOreq: flush? Perhaps it's implied when I do the delete? idfk
    deletePointersAndSetEachFalse();
}
void MultiServerAbstraction::sendMessage(QByteArray *message, quint32 clientId)
{
    QIODevice *clientIODevice = m_ClientsByCookie.value(clientId, 0);
    if(clientIODevice)
    {
        QDataStream networkStream(clientIODevice);
        NetworkMagic::streamOutMagic(networkStream);
        networkStream << *message;
    }
    else
    {
        //TODOreq: stale or something idfk, queue?
    }
}
void MultiServerAbstraction::initialize(MultiServerAbstractionArgs multiServerAbstractionArgs)
{
    if(multiServerAbstractionArgs.m_SslTcpEnabled)
    {
        m_BeSslServer = true;
        m_SslTcpServer = new SslTcpServer(this);
        connect(m_SslTcpServer, SIGNAL(d(const QString &), this, SIGNAL(d(const QString &))));
        connect(m_SslTcpServer, SIGNAL(clientConnectedAndEncrypted(QSslSocket*)), this, SLOT(handleNewSslClientConnected(QSslSocket*)));
        m_SslTcpServer->initialize(multiServerAbstractionArgs.m_SslTcpServerArgs);
    }
}
void MultiServerAbstraction::start()
{
    if(m_BeSslServer)
    {
        m_SslTcpServer->start();
    }
    if(m_BeTcpServer)
    {
        //TODOreq
    }
    if(m_BeLocalSocketServer)
    {
        //TODOreq
    }
}
void MultiServerAbstraction::stop()
{
    if(m_BeSslServer)
    {
        m_SslTcpServer->stop();
    }
    if(m_BeTcpServer)
    {
        //TODOreq
    }
    if(m_BeLocalSocketServer)
    {
        //TODOreq
    }
}
void MultiServerAbstraction::handleNewClientConnected(QIODevice *newClient)
{
#if 0
    m_ServerHelloStatusesByIODevice.insert(newClient, new ServerHelloStatus());
    connect(newClient, SIGNAL(readyRead()), this, SLOT(handleNewClientSentData()));
#endif

    emit d("new client connected, starting hello phase");

    AbstractClientConnection *newClientConnection = new AbstractClientConnection(newClient, this);
    m_ClientConnections.append(newClientConnection); //TODOreq: we need to figure out it's cookie or assign him one. if he gives us a previous one, 'merge' the two connections in this list. What does that involve? Flushing a queue? Doesn't flushing a queue mean finding out (from new connection) where exactly we are? There will be packets that will be IMPLICITLY ack'd at this phase, when we flush the queue from a given point. We may not have received a formal lazy ack ack for the ones that ARE NOT flushed in the queue (the recent coordination told us we didn't need them), but they are now implicitly acked. The reason for that is the disconnection we're recovering from may have lost us the ack ack... but the client might have sent it before knowing the connection sucked. No need to re-send them, just say where we're at and that's good enough. TODOreq _FUCK_ doesn't flushing a queue like that imply a sort of synchroncity? I thought this was supposed to be asynchronous. I'm so confused..... or maybe the lazy ack ack arrives with the retried message... so there is a 1:1 anyways and I don't need to flush any queues because he'll re-send his action requests (TODOreq: broadcasts still need to be sent/synchronized/flushed somehow)

    connect(newClient, SIGNAL(readyRead()), newClientConnection, SLOT(handleDataReceivedFromClient()));
    connect(newClientConnection, SIGNAL(d(QString)), this, SIGNAL(d(QString)));

/*
    m_ServerHelloStatusesByIODevice.insert(newClient, new ServerHelloStatus()); //TODOreq: delete ServerHelloStatus somewhere (dc? dupe detected? both? idfk)
    connect(newClient, SIGNAL(readyRead()), this, SLOT(handleNewClientSentData()));

    //delete m_ServerHelloStatusesByIODevice.value(newClient)->m_IODevicePeeker;
    m_ServerHelloStatusesByIODevice.value(newClient)->m_IODevicePeeker = new ByteArrayMessageSizePeekerForIODevice(newClient);
    */
}
void MultiServerAbstraction::handleNewSslClientConnected(QSslSocket *newSslClient)
{
    handleNewClientConnected(newSslClient);
}
