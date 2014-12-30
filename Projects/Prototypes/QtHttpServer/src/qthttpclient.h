#ifndef QTHTTPCLIENT_H
#define QTHTTPCLIENT_H

#include <QObject>
#include <QTextStream>

class QTcpSocket;

class QtHttpClient : public QObject
{
    Q_OBJECT
public:
    explicit QtHttpClient(QTcpSocket *clientSocket, QObject *parent = 0);
private:
    //QTcpSocket *m_ClientSocket;
    QTextStream m_ClientStream;
private slots:
    void handleReadyRead();
};

#endif // QTHTTPCLIENT_H
