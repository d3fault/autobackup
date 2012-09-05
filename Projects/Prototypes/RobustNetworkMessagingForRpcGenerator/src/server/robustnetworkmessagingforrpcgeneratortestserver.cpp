#include "robustnetworkmessagingforrpcgeneratortestserver.h"

void RobustNetworkMessagingForRpcGeneratorTestServer::init()
{
    m_Server = new TestServer();

    m_Thread4Server = new QThread();

    m_Server->moveToThread(m_Thread4Server);

    m_Thread4Server->start();

    connect(m_Server, SIGNAL(initialized()), this, SIGNAL(initialized()));

    connect(m_Server, SIGNAL(started()), this, SIGNAL(started()));

    connect(m_Server, SIGNAL(stopped()), this, SIGNAL(stopped()));

    connect(m_Server, SIGNAL(d(QString)), this, SIGNAL(d(QString)));

    QMetaObject::invokeMethod(m_Server, "init", Qt::QueuedConnection);
}
void RobustNetworkMessagingForRpcGeneratorTestServer::start()
{
    QMetaObject::invokeMethod(m_Server, "start", Qt::QueuedConnection);
}
void RobustNetworkMessagingForRpcGeneratorTestServer::stop()
{
    QMetaObject::invokeMethod(m_Server, "stop", Qt::QueuedConnection);
}
void RobustNetworkMessagingForRpcGeneratorTestServer::sendMessageToPeer()
{
    QMetaObject::invokeMethod(m_Server, "sendMessageToPeer", Qt::QueuedConnection);
}
