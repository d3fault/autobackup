#ifndef SSLTCPSERVER_H
#define SSLTCPSERVER_H

#include <QtNetwork/QTcpServer>
#include <QtNetwork/QSslSocket>

class SslTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit SslTcpServer(QObject *parent = 0);
protected:
    virtual void incomingConnection(int handle);
signals:
    void newEncryptedConnection(QSslSocket *secureSocket);
    void d(const QString &);
private slots:
    void handleEncrypted();
    void handleSslErrors(QList<QSslError> sslErrors);

};

#endif // SSLTCPSERVER_H
