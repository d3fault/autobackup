#include "gettodaysadslotserver.h"

#include <QtNetwork/QTcpServer>

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
{
    qRegisterMetaType<std::string>("std::string");
}
//our fo constructor, to be called after we've been movedToThread'd
bool GetTodaysAdSlotServer::initializeAndStart(quint16 port)
{
    m_Server = new QTcpServer(this);
    connect(m_Server, SIGNAL(newConnection()), this, SLOT(handleNewConnection()));
    return m_Server->listen(QHostAddress::Any, port);
}
void GetTodaysAdSlotServer::stopAndDestroy()
{
    m_Server->close();
    delete m_Server; //this implicitly deletes all GetTodaysAdSlotServerClientConnections. we know none of them have backend requests pending because beginStoppingCouchbase was called before we get here
}
void GetTodaysAdSlotServer::handleNewConnection()
{
    new GetTodaysAdSlotServerClientConnection(m_Server->nextPendingConnection(), this);
}
