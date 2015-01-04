#include "gettodaysadslotserver.h"

#include <QtNetwork/QTcpServer>

#include "gettodaysadslothttpsserver.h"
#include "gettodaysadslotserverclientconnection.h"

//TODOreq: https (use same cert/key passed to wt via args)
//single threaded (or two threads if you count the db thread), yea, but ASYNC woooot
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
    , m_StdErr(stderr)
{
    qRegisterMetaType<std::string>("std::string");
}
//our fo constructor, to be called after we've been movedToThread'd
bool GetTodaysAdSlotServer::initializeAndStart(quint16 port, const QString &sslCertFilePath, const QString &sslPrivkeyFilePath)
{
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
    return m_Server->listen(QHostAddress::Any, port);
}
void GetTodaysAdSlotServer::stopAndDestroy()
{
    if(m_Server)
    {
        m_Server->close();
        delete m_Server; //this implicitly deletes all GetTodaysAdSlotServerClientConnections. we know none of them have backend requests pending because beginStoppingCouchbase was called before we get here
    }
}
void GetTodaysAdSlotServer::handleE(const QString &msg)
{
    //thread unssafe access to cerr, but I only emit errors during init anyways TODOreq
    m_StdErr << msg << endl;
}
#if 0
void GetTodaysAdSlotServer::handleNewConnection()
{
    new GetTodaysAdSlotServerClientConnection(m_Server->nextPendingConnection());
}
#endif
