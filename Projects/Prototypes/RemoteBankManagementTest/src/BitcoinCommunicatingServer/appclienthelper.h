#ifndef APPCLIENTHELPER_H
#define APPCLIENTHELPER_H

#include <QObject>
#include <QHash>

#include "ssltcpserver.h"
#include "../sharedProtocol/sharedprotocol.h"

class AppClientHelper : public QObject
{
    Q_OBJECT
public:
    explicit AppClientHelper(QObject *parent = 0);
private:
    SslTcpServer *m_SslTcpServer;

    QHash<QString, QSslSocket*> m_SocketsByAppId;
    QSslSocket *getSocketByAppId(const QString &appId);
    bool isConnected(QSslSocket* socketToCheck);
signals:
    void d(const QString &);
    void addUserRequested(const QString &appId, const QString &userName);
    void addFundsKeyRequested(const QString &appId, const QString &userName);
public slots:
    void startListening();
    void handleUserAdded(const QString &appId, const QString &userName);
    void handleAddFundsKeyGenerated(const QString &appId, const QString &userName, const QString &newKey);
    void handlePendingAmountReceived(QString appId, QString userName, QString key, double pendingAmount);
    void handleConfirmedAmountReceived(QString appId, QString userName, QString key, double confirmedAmount);
private slots:
    void handleConnectedButNotEncryptedYet();
    void handleConnectedAndEncrypted();
    void handleReadyRead();
    void handleSslErrors(QList<QSslError> sslErrors);
    void handleSocketError(QAbstractSocket::SocketError);
};

#endif // APPCLIENTHELPER_H
