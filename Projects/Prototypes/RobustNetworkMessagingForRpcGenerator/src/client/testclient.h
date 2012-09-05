#ifndef TESTCLIENT_H
#define TESTCLIENT_H

#include <QDataStream>
#include <QDateTime>

#include "idebuggablestartablestoppablebackend.h"

#include "ssltcpclient.h"

class TestClient : public IDebuggableStartableStoppableBackend
{
    Q_OBJECT
private:
    QDataStream *m_ServerStream;
    SslTcpClient *m_Client;
    quint32 m_DebugMessageNum;
public slots:
    void init();
    void start();
    void stop();

    void sendMessageToPeer();
private slots:
    void handleConnectedAndEncrypted(QSslSocket *socketToServer);
    void handleServerSentUsData();
};

#endif // TESTCLIENT_H
