#ifndef SECURECLIENT_H
#define SECURECLIENT_H

#include <QObject>
#include <QtNetwork/QSslSocket>
#include <QtNetwork/QHostAddress>
#include <QFile>

#include "../shared/protocol.h"

class SecureClient : public QObject
{
    Q_OBJECT
public:
    explicit SecureClient(QObject *parent = 0);
private:
    QSslSocket *m_SslSocket;

    void sendMessageToServer(ClientToServerMessage::TheMessage theMessage);
signals:
    void d(const QString &);
public slots:
    void connectToSecureServer();
private slots:
    void handleConnectedNotYetEncrypted();
    void handleConnectedAndEncrypted();
    void handleSslErrors(QList<QSslError> sslErrors);
    void handleDisconnect();
    void handleReadyRead();
};

#endif // SECURECLIENT_H
