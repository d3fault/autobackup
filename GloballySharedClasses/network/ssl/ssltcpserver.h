#ifndef SSLTCPSERVER_H
#define SSLTCPSERVER_H

#include <QtNetwork/QTcpServer>
#include <QtNetwork/QSslSocket>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>
#include <QQueue>
#include <QHash>
#include <QFile>

class SslTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit SslTcpServer(QObject *parent, const QString &serverCaFile, const QString &clientCaFile, const QString &serverPrivateEncryptionKeyFile, const QString &serverPublicLocalCertificateFile, const QString &serverPrivateEncryptionKeyPassPhrase);
    virtual QTcpSocket *nextPendingConnection();
    static uint getClientUniqueId(QSslSocket *client);
    QSslSocket *getSocketByUniqueId(uint uniqueId);
    //void initAndStartListening();
    bool init();
    bool start();
    void stop();
private:
    QQueue<QSslSocket*> m_PendingConnections;
    QHash<uint,QSslSocket*> m_EncryptedSocketsBySerialNumber;
    QList<QSslCertificate> m_AllMyCertificateAuthorities;
    QSslKey *m_ServerPrivateEncryptionKey;
    QSslCertificate *m_ServerPublicLocalCertificate;

    //simply passing from constructor to init
    QString m_ServerCaFile;
    QString m_ClientCaFile;
    QString m_ServerPrivateEncryptionKeyFile;
    QString m_ServerPrivateEncryptionKeyPassPhrase;
    QString m_ServerPublicLocalCertificateFile;
protected:
    void incomingConnection(int handle);
signals:
    void d(const QString &);
    void clientConnectedAndEncrypted(QSslSocket *client);
private slots:
    void handleNewConnectionNotYetEncrypted();
    void handleConnectedAndEncrypted();
    void handleSslErrors(QList<QSslError> sslErrors);
    void handleSocketError(QAbstractSocket::SocketError abstractSocketError);
};

#endif // SSLTCPSERVER_H
