#ifndef ROBUSTNETWORKMESSAGINGFORRPCGENERATORTEST_H
#define ROBUSTNETWORKMESSAGINGFORRPCGENERATORTEST_H

#include "idebuggablestartablestoppablebackend.h"
#include <QThread>

#include "testserver.h"
#include "testclient.h"

class RobustNetworkMessagingForRpcGeneratorTest : public IDebuggableStartableStoppableBackend
{
    Q_OBJECT
private:
    TestServer *m_Server;
    TestClient *m_Client;

    QThread *m_Thread4Server;
    QThread *m_Thread4Client;
public slots:
    void init();
    void start();
    void stop();
};

#endif // ROBUSTNETWORKMESSAGINGFORRPCGENERATORTEST_H
