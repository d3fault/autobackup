#ifndef TESTCLIENT_H
#define TESTCLIENT_H

#include <QObject>
#include <QDataStream>
#include <QDateTime>
#include <QBuffer>

#include "ssltcpclient.h"

class TestClient : public QObject
{
    Q_OBJECT
public:
    explicit TestClient(QObject *parent = 0);
private:
    bool m_CurrentMessageTypeIsA;
    SslTcpClient *m_Client;
    QSslSocket *m_ServerSocket;
    quint32 m_MessageSendStage;
    void streamOutHelper(QIODevice *device, int stage = -1);
signals:
    void d(const QString &);
    void initialized();
    void started();
    void stopped();
public slots:
    void init();
    void start();
    void stop();

    void sendNextChunkOfActionRequestMessage();
private slots:
    void handleConnectedAndEncrypted(QSslSocket *newConnection);
    void handleServerSentUsData();
};

#endif // TESTCLIENT_H
