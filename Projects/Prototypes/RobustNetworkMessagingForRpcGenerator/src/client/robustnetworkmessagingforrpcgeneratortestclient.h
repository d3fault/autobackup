#ifndef ROBUSTNETWORKMESSAGINGFORRPCGENERATORTESTCLIENT_H
#define ROBUSTNETWORKMESSAGINGFORRPCGENERATORTESTCLIENT_H

#include "idebuggablestartablestoppablebackend.h"
#include <QThread>

#include "testclient.h"

class RobustNetworkMessagingForRpcGeneratorTestClient : public IDebuggableStartableStoppableBackend
{
    Q_OBJECT
private:
    TestClient *m_Client;

    QThread *m_Thread4Client;
public slots:
    void init();
    void start();
    void stop();

    void sendMessageToPeer();
};

#endif // ROBUSTNETWORKMESSAGINGFORRPCGENERATORTESTCLIENT_H
