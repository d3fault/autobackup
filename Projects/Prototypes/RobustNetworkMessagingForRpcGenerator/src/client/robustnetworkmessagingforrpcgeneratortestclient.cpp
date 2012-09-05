#include "robustnetworkmessagingforrpcgeneratortest.h"

void RobustNetworkMessagingForRpcGeneratorTest::init()
{
    m_Server = new TestServer();
    m_Client = new TestClient();

    m_Thread4Server = new QThread();
    m_Thread4Client = new QThread();

    m_Server->moveToThread(m_Thread4Server);
    m_Client->moveToThread(m_Thread4Client);

    m_Thread4Server->start();
    m_Thread4Client->start();

    connect(m_Server, SIGNAL(initialized()), m_Client, SLOT(init()));
    connect(m_Client, SIGNAL(initialized()), this, SIGNAL(initialized()));

    connect(m_Server, SIGNAL(started()), m_Client, SLOT(start()));
    connect(m_Client, SIGNAL(started()), this, SIGNAL(started()));

    connect(m_Client, SIGNAL(stopped()), m_Server, SLOT(stop()));
    connect(m_Server, SIGNAL(stopped()), this, SIGNAL(stopped()));

    connect(m_Server, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
    connect(m_Client, SIGNAL(d(QString)), this, SIGNAL(d(QString)));

    QMetaObject::invokeMethod(m_Server, "init", Qt::QueuedConnection);
}
void RobustNetworkMessagingForRpcGeneratorTest::start()
{
    QMetaObject::invokeMethod(m_Server, "start", Qt::QueuedConnection);
}
void RobustNetworkMessagingForRpcGeneratorTest::stop()
{
    QMetaObject::invokeMethod(m_Client, "stop", Qt::QueuedConnection);
}
