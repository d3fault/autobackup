#include "torhiddenservicehttplocalhostonlyserver.h"

#include <QTcpSocket>

#include "gettodaysadslotserverclientconnection.h"

//this is only for the API. the "web" hidden service uses the same "http server" as wt
TorHiddenServiceHttpLocalhostOnlyServer::TorHiddenServiceHttpLocalhostOnlyServer(QObject *parent)
    : QTcpServer(parent)
{ }
void TorHiddenServiceHttpLocalhostOnlyServer::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *clientSocket = new QTcpSocket(this);
    if(clientSocket->setSocketDescriptor(socketDescriptor))
    {
        const QHostAddress &peerAddress = clientSocket->peerAddress();
        bool peerAddressIsLocalhost = ((peerAddress == QHostAddress::LocalHost) || (peerAddress == QHostAddress::LocalHostIPv6));
        if(!peerAddressIsLocalhost) //TODOoptional: !isLoopback() instead?
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
