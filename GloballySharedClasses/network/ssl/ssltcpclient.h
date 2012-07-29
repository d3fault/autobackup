#ifndef SSLTCPCLIENT_H
#define SSLTCPCLIENT_H

#include <QtNetwork/QSslSocket>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>
#include <QList>
#include <QFile>

class SslTcpClient : public QSslSocket
{
    Q_OBJECT
public:
    explicit SslTcpClient(QObject *parent, const QString &clientCaFile, const QString &serverCaFile, const QString &clientPrivateEncryptionKeyFile, const QString &clientPublicLocalCertificateFile, const QString &clientPrivateEncryptionKeyPassPhrase);
    bool init();
    bool start();
    void stop();
private:
    QList<QSslCertificate> m_AllMyCertificateAuthorities;
    QSslKey *m_ClientPrivateEncryptionKey;
    QSslCertificate *m_ClientPublicLocalCertificate;

    //simply passing from constructor to init
    QString m_ClientCaFile;
    QString m_ServerCaFile;
    QString m_ClientPrivateEncryptionKeyFile;
    QString m_ClientPrivateEncryptionKeyPassPhrase; //TODOreq: if i ever implement the whole 'enter passphrase via ssh on app startup' shit, i need to make sure i do overwrite the passphrase in memory after i use it. i also need to take into account that QString will detach on write... and the passphrase will still be in memory lol
    QString m_ClientPublicLocalCertificateFile;
signals:
    void d(const QString &);
    void connectedAndEncrypted(QSslSocket *socket);
private slots:
    void handleEncrypted();
    void handleSslErrors(QList<QSslError> sslErrors);
    void handleSocketError(QAbstractSocket::SocketError abstractSocketError);
};

#endif // SSLTCPCLIENT_H
