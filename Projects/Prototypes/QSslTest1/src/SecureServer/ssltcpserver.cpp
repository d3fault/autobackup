#include "ssltcpserver.h"

SslTcpServer::SslTcpServer(QObject *parent) :
    QTcpServer(parent)
{
}
void SslTcpServer::incomingConnection(int handle)
{
    QSslSocket *secureSocket = new QSslSocket();
    if(secureSocket->setSocketDescriptor(handle))
    {
        connect(secureSocket, SIGNAL(encrypted()), this, SLOT(handleEncrypted()));
        connect(secureSocket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(handleSslErrors(QList<QSslError>)));
        secureSocket->startServerEncryption();
    }
    else
    {
        delete secureSocket;
    }
}
void SslTcpServer::handleEncrypted()
{
    //hopefully this will work...
    QSslSocket *secureSocket = qobject_cast<QSslSocket*>(sender());
    emit newEncryptedConnection(secureSocket);
}
void SslTcpServer::handleSslErrors(QList<QSslError> sslErrors)
{
    int numErrors = sslErrors.count();
    for(int i = 0; i < numErrors; ++i)
    {
        emit d("ssl error #" + QString::number(i) + " - " + sslErrors.at(i).errorString());
    }
}
