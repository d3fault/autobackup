#include "qthttpserver.h"

#include <QtNetwork/QTcpSocket>

#include "qthttpclient.h"

#define QtHttpServer_ACCEPT_CONNECTIONS_FROM_LOCALHOST_ONLY 0 //simple prototype compile time switch, because a tor hidden service shouldn't be https -- but it also shouldn't accept outside connections

//KISS one url http server, to test that it works, and that I can parse url params
QtHttpServer::QtHttpServer(QObject *parent)
    : QTcpServer(parent)
{
    connect(this, SIGNAL(newConnection()), this, SLOT(handleNewConnection()));
}
bool QtHttpServer::startQtHttpServer(quint16 port)
{
    return listen(QHostAddress::Any, port);
}
void QtHttpServer::handleNewConnection()
{
    QTcpSocket *newConnection = nextPendingConnection();
#ifdef QtHttpServer_ACCEPT_CONNECTIONS_FROM_LOCALHOST_ONLY
    const QHostAddress &peerAddress = newConnection->peerAddress();
    emit e(peerAddress.toString());
    if(peerAddress != QHostAddress::LocalHost && peerAddress != QHostAddress::LocalHostIPv6)
    {
        emit e("incoming connection NOT from localhost. rejecting");
        connect(newConnection, SIGNAL(disconnected()), newConnection, SLOT(deleteLater()));
        newConnection->close();
        return;
    }
    emit e("incoming connectin from localhost. ACCEPTING");
#endif
    new QtHttpClient(newConnection);
}
