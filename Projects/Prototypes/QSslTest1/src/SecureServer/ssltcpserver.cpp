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
        emit d("server is initiating delayed handshake");


        //retarded debugging
        if(!secureSocket->waitForEncrypted())
        {
            emit d(secureSocket->errorString());
        }
    }
    else
    {
        emit d("failed to set socket descriptor");
        delete secureSocket;
    }
}
void SslTcpServer::handleEncrypted()
{
    emit d("the socket has emitted encrypted()");
    //hopefully this will work...
    QSslSocket *secureSocket = qobject_cast<QSslSocket*>(sender());
    emit newEncryptedConnection(secureSocket);
}
void SslTcpServer::handleSslErrors(QList<QSslError> sslErrors)
{
    int numErrors = sslErrors.count();
    emit d(QString::number(numErrors) + " ssl errors");
    for(int i = 0; i < numErrors; ++i)
    {
        emit d("ssl error #" + QString::number(i) + " - " + sslErrors.at(i).errorString());
    }
}
