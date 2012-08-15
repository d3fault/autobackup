#ifndef TESTCLIENT_H
#define TESTCLIENT_H

#include "idebuggablestartablestoppablebackend.h"

#include "ssltcpclient.h"

class TestClient : public IDebuggableStartableStoppableBackend
{
    Q_OBJECT
private:
    SslTcpClient *m_Client;
public slots:
    void init();
    void start();
    void stop();
private slots:
    void handleConnectedAndEncrypted(QSslSocket *socketToServer);
};

#endif // TESTCLIENT_H
