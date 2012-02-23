#include "ssltcpserver.h"

SslTcpServer::SslTcpServer(QObject *parent) :
    QTcpServer(parent)
{
}
void SslTcpServer::incomingConnection(int handle)
{
    QSslSocket *secureSocket = new QSslSocket(this);
    if(secureSocket->setSocketDescriptor(handle))
    {
        //connect(secureSocket, SIGNAL(encrypted()), this, SLOT(handleEncrypted()));
        //connect(secureSocket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(handleSslErrors(QList<QSslError>)));

        //TODO: setPrivateKey and setLocalCertificate
        /*
          openssl genrsa -out privkey.pem 2048
          openssl req -new -x509 -key privkey.pem -out cacert.pem -days 1095
        */
        secureSocket->setPrivateKey(":/serverprivatekey.pem");
        secureSocket->setLocalCertificate(":/servercert.pem");
        //secureSocket->setCaCertificates(); <-- i think i need to do this one the client for the ca cert that generated my server's local cert or something? barely understand what i'm doing

        m_PendingConnections.enqueue(secureSocket);
        secureSocket->startServerEncryption();
        emit d("server is initiating delayed handshake");


        //retarded debugging
        /*if(!secureSocket->waitForEncrypted())
        {
            emit d(secureSocket->errorString());
        }*/
    }
    else
    {
        emit d("failed to set socket descriptor");
        delete secureSocket;
    }
}
/*void SslTcpServer::handleEncrypted()
{
    emit d("the socket has emitted encrypted()");
    //hopefully this will work...
    QSslSocket *secureSocket = qobject_cast<QSslSocket*>(sender());
    emit newEncryptedConnection(secureSocket);
}*/
QTcpSocket * SslTcpServer::nextPendingConnection()
{
    if(m_PendingConnections.isEmpty())
    {
        return 0;
    }
    return m_PendingConnections.dequeue();
}
