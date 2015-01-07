#include "torhiddenservicehttplocalhostonlyserver.h"

#include <QTcpSocket>

#include "gettodaysadslotserverclientconnection.h"

//this is only for the API. the "web" hidden service uses the same "http server" as wt (so... i shouldn't redirect to https TODOreq fml?)
TorHiddenServiceHttpLocalhostOnlyServer::TorHiddenServiceHttpLocalhostOnlyServer(QObject *parent)
    : QTcpServer(parent)
{ }
void TorHiddenServiceHttpLocalhostOnlyServer::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *clientSocket = new QTcpSocket(this);
    if(clientSocket->setSocketDescriptor(socketDescriptor))
    {
        const QHostAddress &peerAddress = clientSocket->peerAddress();
        if(peerAddress != QHostAddress::LocalHost && peerAddress != QHostAddress::LocalHostIPv6)
        {
            connect(clientSocket, SIGNAL(disconnected()), clientSocket, SLOT(deleteLater()));
            clientSocket->close();
            return;
        }
        new GetTodaysAdSlotServerClientConnection(clientSocket);
    }
    else
    {
        delete clientSocket;
    }
}
