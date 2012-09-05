#ifndef TESTSERVER_H
#define TESTSERVER_H

#include "idebuggablestartablestoppablebackend.h"

#include "ssltcpserver.h"

class TestServer : public IDebuggableStartableStoppableBackend
{
    Q_OBJECT
private:
    SslTcpServer *m_Server;
public slots:
    void init();
    void start();
    void stop();
private slots:
    void handleClientConnectedAndEncrypted(QSslSocket *newClientSocket);
};

#endif // TESTSERVER_H
