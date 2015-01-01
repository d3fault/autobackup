#ifndef QTHTTPSERVER_H
#define QTHTTPSERVER_H

#include <QtNetwork/QTcpServer>

class QSslKey;
class QSslCertificate;

class QtHttpsServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit QtHttpsServer(QObject *parent = 0);
    bool startQtHttpServer(quint16 port);
    ~QtHttpsServer();
private:
    QSslKey *m_ServerPrivateEncryptionKey;
    QSslCertificate *m_ServerPublicLocalCertificate;
protected:
    void incomingConnection(qintptr socketDescriptor);
signals:
    void e(const QString &msg);
    void quitRequested();
private slots:
    void handleNewConnection();
};

#endif // QTHTTPSERVER_H
