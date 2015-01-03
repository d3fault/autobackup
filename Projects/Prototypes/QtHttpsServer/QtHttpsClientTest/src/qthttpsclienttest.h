#ifndef QTHTTPSCLIENTTEST_H
#define QTHTTPSCLIENTTEST_H

#include <QObject>
#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QSslError>

class QtHttpsClientTest : public QObject
{
    Q_OBJECT
public:
    explicit QtHttpsClientTest(QObject *parent = 0);
private:
    void quitPlx();
private slots:
    void handleError(QAbstractSocket::SocketError socketError);
    void handleSslErrors(QList<QSslError> sslErrors);
    void handlePeerVerifyError(QSslError peerVerifyError);
};

#endif // QTHTTPSCLIENTTEST_H
