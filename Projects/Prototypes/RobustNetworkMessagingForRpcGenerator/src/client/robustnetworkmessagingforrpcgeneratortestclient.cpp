#include "robustnetworkmessagingforrpcgeneratortestclient.h"

void RobustNetworkMessagingForRpcGeneratorTestClient::init()
{
    m_Client = new TestClient();

    m_Thread4Client = new QThread();

    m_Client->moveToThread(m_Thread4Client);

    m_Thread4Client->start();

    connect(m_Client, SIGNAL(initialized()), this, SIGNAL(initialized()));

    connect(m_Client, SIGNAL(started()), this, SIGNAL(started()));

    connect(m_Client, SIGNAL(stopped()), this, SIGNAL(stopped()));

    connect(m_Client, SIGNAL(d(QString)), this, SIGNAL(d(QString)));

    QMetaObject::invokeMethod(m_Client, "init", Qt::QueuedConnection);
}
void RobustNetworkMessagingForRpcGeneratorTestClient::start()
{
    QMetaObject::invokeMethod(m_Client, "start", Qt::QueuedConnection);
}
void RobustNetworkMessagingForRpcGeneratorTestClient::stop()
{
    QMetaObject::invokeMethod(m_Client, "stop", Qt::QueuedConnection);
}
void RobustNetworkMessagingForRpcGeneratorTestClient::sendMessageToPeer()
{
    QMetaObject::invokeMethod(m_Client, "sendMessageToPeer", Qt::QueuedConnection);
}
void RobustNetworkMessagingForRpcGeneratorTestClient::readSecondMessage()
{
    QMetaObject::invokeMethod(m_Client, "readSecondMessage", Qt::QueuedConnection);
}
