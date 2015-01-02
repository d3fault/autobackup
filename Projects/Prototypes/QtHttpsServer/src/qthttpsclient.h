#ifndef QTHTTPCLIENT_H
#define QTHTTPCLIENT_H

#include <QObject>
#include <QTextStream>
#include <QtNetwork/QSslError>

class QSslSocket;

class QtHttpsClient : public QObject
{
    Q_OBJECT
public:
    explicit QtHttpsClient(QSslSocket *clientSocket, QObject *parent = 0);
private:
    //QTcpSocket *m_ClientSocket;
    QTextStream m_ClientStream;
signals:
    void e(const QString &msg);
private slots:
    void handlePeerVerifyError(QSslError sslError);
    void handleSslErrors(QList<QSslError> sslErrors);
    void handleClientSocketEncrypted();
    void handleReadyRead();
};

#endif // QTHTTPCLIENT_H
