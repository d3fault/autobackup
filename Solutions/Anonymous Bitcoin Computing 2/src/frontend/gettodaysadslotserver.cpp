#include "gettodaysadslotserver.h"

#include <QtNetwork/QTcpServer>

#include "gettodaysadslothttpsserver.h"
#include "torhiddenservicehttplocalhostonlyserver.h"
#include "gettodaysadslotserverclientconnection.h"

//single threaded (or two threads if you count the db thread), yea, but ASYNC woooot
//Better named: "ABC API Server" -- even though for now yea it is just 'current ad' ('today' implies they chose 24 hours)
void GetTodaysAdSlotServer::setBackendGetQueue(boost::lockfree::queue<GetCouchbaseDocumentByKeyRequest *> *backendQueue)
{
    GetTodaysAdSlotServerClientConnection::setBackendGetQueue(backendQueue);
}
void GetTodaysAdSlotServer::setBackendStoreQueue(boost::lockfree::queue<StoreCouchbaseDocumentByKeyRequest *> *backendQueue)
{
    GetTodaysAdSlotServerClientConnection::setBackendStoreQueue(backendQueue);
}
void GetTodaysAdSlotServer::setBackendGetQueueEvent(struct event *backendQueueEvent)
{
    GetTodaysAdSlotServerClientConnection::setBackendGetQueueEvent(backendQueueEvent);
}
void GetTodaysAdSlotServer::setBackendStoreQueueEvent(struct event *backendQueueEvent)
{
    GetTodaysAdSlotServerClientConnection::setBackendStoreQueueEvent(backendQueueEvent);
}
GetTodaysAdSlotServer::GetTodaysAdSlotServer(QObject *parent)
    : QObject(parent)
    , m_Server(0)
    , m_TorHiddenServiceHttpLocalhostOnlyServer(0)
    , m_StdErr(stderr)
{
    qRegisterMetaType<std::string>("std::string");
}
//our fo constructor, to be called after we've been movedToThread'd
bool GetTodaysAdSlotServer::initializeAndStart(quint16 port, const QString &sslCertFilePath, const QString &sslPrivkeyFilePath, quint16 optionalTorHiddenServiceHttpServerPort_OrZeroIfNotToStartIt)
{
    //TODOoptional: would be more properer to have these two servers emit a "clientConnected(QTcpSocket*)", listen to it in this class, and then we ourselves instantiate the GetTodaysAdSlotServerClientConnection object (the servers shouldn't know about it) -- just eh proper design n shit

    //m_Server = new QTcpServer(this);
    GetTodaysAdSlotHttpsServer *httpsServer = new GetTodaysAdSlotHttpsServer(this);
    connect(httpsServer, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    if(!httpsServer->initialize(sslCertFilePath, sslPrivkeyFilePath))
    {
        delete httpsServer;
        return false;
    }
    m_Server = httpsServer;
    //HTTP: connect(m_Server, SIGNAL(newConnection()), this, SLOT(handleNewConnection()));
    bool ret = m_Server->listen(QHostAddress::Any, port);
    if(!ret)
    {
        handleE(m_Server->errorString());
        return ret;
    }
    if(optionalTorHiddenServiceHttpServerPort_OrZeroIfNotToStartIt != 0)
    {
        m_TorHiddenServiceHttpLocalhostOnlyServer = new TorHiddenServiceHttpLocalhostOnlyServer(this);
        ret = m_TorHiddenServiceHttpLocalhostOnlyServer->listen(QHostAddress::Any, optionalTorHiddenServiceHttpServerPort_OrZeroIfNotToStartIt);
    }
    return ret;
}
void GetTodaysAdSlotServer::stopAndDestroy()
{
    if(m_Server)
    {
        m_Server->close();
        delete m_Server; //this implicitly deletes all GetTodaysAdSlotServerClientConnections. we know none of them have backend requests pending because beginStoppingCouchbase was called before we get here
        m_Server = 0;
    }
    if(m_TorHiddenServiceHttpLocalhostOnlyServer)
    {
        m_TorHiddenServiceHttpLocalhostOnlyServer->close();
        delete m_TorHiddenServiceHttpLocalhostOnlyServer;
        m_TorHiddenServiceHttpLocalhostOnlyServer = 0;
    }
}
void GetTodaysAdSlotServer::handleE(const QString &msg)
{
    //thread unsafe access to cerr, but I only emit errors during init anyways TODOreq
    m_StdErr << msg << endl;
}
#if 0
void GetTodaysAdSlotServer::handleNewConnection()
{
    new GetTodaysAdSlotServerClientConnection(m_Server->nextPendingConnection());
}
#endif
