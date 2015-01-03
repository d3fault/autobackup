#include "qthttpsclienttest.h"

#include <QtNetwork/QSslSocket>
#include <QDebug>
#include <QCoreApplication>
#include <QTimer>

QtHttpsClientTest::QtHttpsClientTest(QObject *parent)
    : QObject(parent)
{
    QSslSocket *client = new QSslSocket(this);
    connect(client, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleError(QAbstractSocket::SocketError)));
    connect(client, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(handleSslErrors(QList<QSslError>)));
    connect(client, SIGNAL(peerVerifyError(QSslError)), this, SLOT(handlePeerVerifyError(QSslError)));
    client->connectToHostEncrypted("127.0.0.1", 9999, "anonymousbitcoincomputing.com");
    if(!client->waitForEncrypted(5000)) //fuck proper event driven~
    {
        qDebug() << "encrypted failed" << endl;
        quitPlx();
        return;
    }
    qDebug() << "encrypted!!!";
    client->write("GET /echothis HTTP/1.0\r\n");
    if(!client->waitForBytesWritten(5000))
    {
        qDebug() << "failed to bytes written";
        quitPlx();
        return;
    }
    qDebug() << "bytes written!!!";
    if(!client->waitForReadyRead(5000))
    {
        qDebug() << "failed to ready read";
        quitPlx();
        return;
    }
    qDebug() << "response: " << client->readAll();
    QTimer::singleShot(1000, qApp, SLOT(quit()));
}
void QtHttpsClientTest::quitPlx()
{
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
void QtHttpsClientTest::handleError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "socket error";
    qDebug() << socketError;
}
void QtHttpsClientTest::handleSslErrors(QList<QSslError> sslErrors)
{
    qDebug() << "ssl errors";
    qDebug() << sslErrors;
}
void QtHttpsClientTest::handlePeerVerifyError(QSslError peerVerifyError)
{
    qDebug() << "peer verify error";
    qDebug() << peerVerifyError;
}
