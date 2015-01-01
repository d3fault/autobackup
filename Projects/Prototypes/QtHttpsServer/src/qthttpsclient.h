#ifndef QTHTTPCLIENT_H
#define QTHTTPCLIENT_H

#include <QObject>
#include <QTextStream>

class QTcpSocket;

class QtHttpsClient : public QObject
{
    Q_OBJECT
public:
    explicit QtHttpsClient(QTcpSocket *clientSocket, QObject *parent = 0);
private:
    //QTcpSocket *m_ClientSocket;
    QTextStream m_ClientStream;
private slots:
    void handleClientSocketEncrypted();
    void handleReadyRead();
};

#endif // QTHTTPCLIENT_H
