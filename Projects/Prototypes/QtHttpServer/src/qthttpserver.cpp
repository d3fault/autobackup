#include "qthttpserver.h"

#include <QtNetwork/QTcpSocket>

#include "qthttpclient.h"

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
    new QtHttpClient(nextPendingConnection());
}
