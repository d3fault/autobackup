#ifndef SSLTCPCLIENT_H
#define SSLTCPCLIENT_H

#include <QtNetwork/QSslSocket>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>
#include <QHostAddress>
#include <QList>
#include <QFile>

/*
To Generate All Client CA Stuff (not needed if you only want regular one-way HTTPS-like encryption):

use /etc/pki/tls/misc/CA script to generate the CA, the cert/private-key, and then sign the cert with the CA.

I personally recommend doing this in a VM because the CA script integrates too tightly with your system. I set a checkpoint and roll back each time after manually copying the files out

./CA -newca
./CA -newreq
./CA -sign

then put the CA on both the server/client -- it's like their authentication list
and the cert/private-key on the client only
*/

struct SslTcpClientArgs
{
    QHostAddress HostAddress;
    quint16 Port;
    QSslConfiguration SslConfiguration;
    QString ClientCaFilename;
    //bool UseClientCA2WaySecurity;
    QString ServerCaFilename;
    QString ClientPrivateEncryptionKeyFilename;
    QString ClientPublicLocalCertificateFilename;
    QString ClientPrivateEncryptionKeyPassPhrase;
};
//TODOreq: need to be able to work when there isn't 2-way authentication. right now we'd probably get "null" debug messages and prematurely return instead of handling the "empty string filenames for the ca [which? i forget lol]" when not doing 2 way...
class SslTcpClient : public QSslSocket
{
    Q_OBJECT
public:
    explicit SslTcpClient(QObject *parent = 0); /*, const QString &clientCaFile, const QString &serverCaFile, const QString &clientPrivateEncryptionKeyFile, const QString &clientPublicLocalCertificateFile, const QString &clientPrivateEncryptionKeyPassPhrase);*/
    ~SslTcpClient();
    //bool isSslConnectionGood();
    void initialize(SslTcpClientArgs sslTcpClientArgs);
    void start(); //TODOreq: start only returns true if we successfully initiate the connection... but my code might be interpretting it as "the socket is connected, encrypted, and ready to be written to". i actually think QAbstractSocket is ok with this (though what happens to the writes if the connection drops due to SSL errors, for example?)
    void stop();
private:
    SslTcpClientArgs m_SslTcpClientArgs;
    QList<QSslCertificate> m_AllMyCertificateAuthorities;
    QSslKey *m_ClientPrivateEncryptionKey;
    QSslCertificate *m_ClientPublicLocalCertificate;

    //simply passing from constructor to init
    //QString m_ClientCaFile;
    //QString m_ServerCaFile;
    //QString m_ClientPrivateEncryptionKeyFile;
    //QString m_ClientPrivateEncryptionKeyPassPhrase; //TODOreq: if i ever implement the whole 'enter passphrase via ssh on app startup' shit, i need to make sure i do overwrite the passphrase in memory after i use it. i also need to take into account that QString will detach on write... and the passphrase will still be in memory lol
    //QString m_ClientPublicLocalCertificateFile;
signals:
    void d(const QString &);
    void connectedAndEncrypted(QSslSocket *socket);
private slots:
    void handleEncrypted();
    void handleSslErrors(QList<QSslError> sslErrors);
    void handleSocketError(QAbstractSocket::SocketError abstractSocketError);
};

#endif // SSLTCPCLIENT_H
