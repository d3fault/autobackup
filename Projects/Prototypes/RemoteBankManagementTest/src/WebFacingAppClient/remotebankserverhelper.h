#ifndef REMOTEBANKSERVERHELPER_H
#define REMOTEBANKSERVERHELPER_H

#include <QObject>
#include <QtNetwork/QSslSocket>
#include <QFile>

#include "../sharedProtocol/sharedprotocol.h"

#define MY_CLIENT_APP_ID "lkjfosidf08043298234098234dsafljkd" //todo: a nice 32 digit hex value or something

class RemoteBankServerHelper : public QObject
{
    Q_OBJECT
public:
    explicit RemoteBankServerHelper(QObject *parent = 0);
    void addUser(QString userToAdd);
    void getNewAddFundsKey(QString user);
private:
    QSslSocket *m_SslSocket;
    bool isConnected();
    //bool isReady(); //isConnected + isAppIdOk -- BLOCKING/waiting for both

    //messages
    void sendAddUserMessage(QString userToAdd);
    //void sendAppIdMessage();
signals:
    void userAdded(QString newUser);
    void addFundsKeyReceived(QString user, QString newKey);
    void pendingPaymentReceived(QString user, QString key, double pendingAmount);
    void confirmedPaymentReceived(QString user, QString key, double confirmedAmount);
    void d(const QString &);
private slots:
    void handleConnectedAndEncrypted();
    void handleSslErrors(QList<QSslError> sslErrors);
    void handleReadyRead();
    void handleSocketError(QAbstractSocket::SocketError);
    void handleDisconnect();
};

#endif // REMOTEBANKSERVERHELPER_H
