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
        QFile serverCaFileResource(":/serverCA.pem");
        serverCaFileResource.open(QFile::ReadOnly);
        QByteArray serverCaByteArray = serverCaFileResource.readAll();
        serverCaFileResource.close();

        QSslCertificate serverCA(serverCaByteArray);
        if(serverCA.isNull())
        {
            emit d("the server CA is null");
            return;
        }
        emit d("the server certificate is not null");

        QFile clientCaFileResource(":/clientCA.pem");
        clientCaFileResource.open(QFile::ReadOnly);
        QByteArray clientCaByteArray = clientCaFileResource.readAll();
        clientCaFileResource.close();

        QSslCertificate clientCA(clientCaByteArray);
        if(clientCA.isNull())
        {
            emit d("client CA is null");
        }
        emit d("client ca is not null");

        QList<QSslCertificate> allCertificateAuthorities;
        allCertificateAuthorities.append(serverCA);
        allCertificateAuthorities.append(clientCA);

        secureSocket->setCaCertificates(allCertificateAuthorities);

        QByteArray passPhrase("fuckyou");

        secureSocket->setPrivateKey(":/serverprivatekey.pem", QSsl::Rsa, QSsl::Pem, passPhrase);
        secureSocket->setLocalCertificate(":/servercert.pem");

        m_PendingConnections.enqueue(secureSocket);
        secureSocket->startServerEncryption();
        emit d("server is initiating delayed handshake");

        /*
        if(!secureSocket->waitForEncrypted())
        {
            emit d(secureSocket->errorString());
        }
        emit d("got past wait");*/
    }
    else
    {
        emit d("failed to set socket descriptor");
        delete secureSocket;
    }
}
QTcpSocket * SslTcpServer::nextPendingConnection()
{
    if(m_PendingConnections.isEmpty())
    {
        return 0;
    }
    return m_PendingConnections.dequeue();
}
