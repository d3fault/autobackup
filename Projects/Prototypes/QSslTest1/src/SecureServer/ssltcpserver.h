#ifndef SSLTCPSERVER_H
#define SSLTCPSERVER_H

#include <QtNetwork/QTcpServer>
#include <QtNetwork/QSslSocket>
#include <QQueue>

class SslTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit SslTcpServer(QObject *parent = 0);
    virtual QTcpSocket *nextPendingConnection();
private:
    QQueue<QSslSocket*> m_PendingConnections;
protected:
    virtual void incomingConnection(int handle);
signals:
    void newEncryptedConnection(QSslSocket *secureSocket);
    void d(const QString &);
private slots:
    //void handleEncrypted();
};

#endif // SSLTCPSERVER_H
