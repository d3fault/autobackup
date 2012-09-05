#ifndef SSLTCPSERVER_H
#define SSLTCPSERVER_H

#include <QtNetwork/QTcpServer>
#include <QtNetwork/QSslSocket>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>
#include <QQueue>
#include <QHash>
#include <QList>
#include <QFile>

/*
To Generate All Server CA Stuff:

use /etc/pki/tls/misc/CA script to generate the CA, the cert/private-key, and then sign the cert with the CA.

I personally recommend doing this in a VM because the CA script integrates too tightly with your system. I set a checkpoint and roll back each time after manually copying the files out

./CA -newca
./CA -newreq
./CA -sign

then put the CA on both the server/client -- it's like their authentication list
and the cert/private-key on the server only

If you want 2-way authentication (stronger), give the client it's own CA. the process is [additionally] reversed and both sides then have 2 CA certs. their own and their peer's
*/

class SslTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit SslTcpServer(QObject *parent, const QString &serverCaFile, const QString &clientCaFile, const QString &serverPrivateEncryptionKeyFile, const QString &serverPublicLocalCertificateFile, const QString &serverPrivateEncryptionKeyPassPhrase);
    virtual QTcpSocket *nextPendingConnection();
    static bool isSslConnectionGood(QSslSocket *socketToCheck);
    static uint getClientUniqueId(QSslSocket *client);
    QSslSocket *getSocketByUniqueId(uint uniqueId);
    QList<uint> getAllConnectedUniqueIds();
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
