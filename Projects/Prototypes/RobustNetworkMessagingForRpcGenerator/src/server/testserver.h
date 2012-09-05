#ifndef TESTSERVER_H
#define TESTSERVER_H

#include <QDataStream>
#include <QDateTime>

#include "idebuggablestartablestoppablebackend.h"

#include "ssltcpserver.h"

class TestServer : public IDebuggableStartableStoppableBackend
{
    Q_OBJECT
private:
    SslTcpServer *m_Server;

    QDataStream *m_ClientDataStream;
    QSslSocket *m_Client;
    quint32 m_DebugMessageNum;
    bool m_ShitIsFucked;
public slots:
    void init();
    void start();
    void stop();

    void sendMessageToPeer();
private slots:
    void handleClientConnectedAndEncrypted(QSslSocket *newClientSocket);
    void handleDataReceivedFromClient();
};

#endif // TESTSERVER_H
