#ifndef ROBUSTNETWORKMESSAGINGFORRPCGENERATORTEST_H
#define ROBUSTNETWORKMESSAGINGFORRPCGENERATORTEST_H

#include "idebuggablestartablestoppablebackend.h"
#include <QThread>

#include "testserver.h"

class RobustNetworkMessagingForRpcGeneratorTestServer : public IDebuggableStartableStoppableBackend
{
    Q_OBJECT
private:
    TestServer *m_Server;

    QThread *m_Thread4Server;
public slots:
    void init();
    void start();
    void stop();

    void sendMessageToPeer();
};

#endif // ROBUSTNETWORKMESSAGINGFORRPCGENERATORTEST_H
