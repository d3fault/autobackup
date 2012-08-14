#ifndef TESTSERVER_H
#define TESTSERVER_H

#include <QObject>
#include <QDataStream>

#include "ssltcpserver.h"
#include "simpleassprotocol.h"

class TestServer : public QObject
{
    Q_OBJECT
public:
    explicit TestServer(QObject *parent = 0);
private:
    SslTcpServer *m_Server;
    QSslSocket *m_ClientSocket;

    bool m_WaitingForHeader;
    MyMessageHeader *m_CurrentHeader;
signals:
    void d(const QString &);
    void initialized();
    void started();
    void stopped();
public slots:
    void init();
    void start();
    void stop();

    void sendNextChunkOfBroadcastMessage();
private slots:
    void handleClientConnectedAndEncrypted(QSslSocket *clientSocket);
    void handleClientSentUsData();
};

#endif // TESTSERVER_H
