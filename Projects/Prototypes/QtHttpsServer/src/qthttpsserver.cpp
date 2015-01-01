#include "qthttpsserver.h"

#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QSslSocket>
#include <QFile>
#include <QtNetwork/QSslKey>
#include <QtNetwork/QSslCertificate>

#include "qthttpsclient.h"

//KISS one url http server, to test that it works, and that I can parse url params
QtHttpsServer::QtHttpsServer(QObject *parent)
    : QTcpServer(parent)
    , m_ServerPrivateEncryptionKey(0)
    , m_ServerPublicLocalCertificate(0)
{
    //connect(this, SIGNAL(newConnection()), this, SLOT(handleNewConnection()));

    QFile serverPrivateEncryptionKeyFileResource("/run/shm/server.key");
    if(!serverPrivateEncryptionKeyFileResource.open(QFile::ReadOnly))
    {
        emit e("server.key not found");
        return;
    }
    QByteArray serverPrivateEncryptionKeyByteArray = serverPrivateEncryptionKeyFileResource.readAll();

    m_ServerPrivateEncryptionKey = new QSslKey(serverPrivateEncryptionKeyByteArray, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey);

    if(m_ServerPrivateEncryptionKey->isNull())
    {
        emit e("server private encryption key is null");
        return;
    }

    //Server Public Local Certificate is the public decryption key that we send to our client
    QFile serverPublicLocalCertificateFileResource("/run/shm/server.pem");
    if(!serverPublicLocalCertificateFileResource.open(QFile::ReadOnly))
    {
        emit e("server.pem not found");
        return;
    }
    QByteArray serverPublicLocalCertificateByteArray = serverPublicLocalCertificateFileResource.readAll();

    m_ServerPublicLocalCertificate = new QSslCertificate(serverPublicLocalCertificateByteArray);
    if(m_ServerPublicLocalCertificate->isNull())
    {
        emit e("server public local certificate is null");
        return;
    }
    if(m_ServerPublicLocalCertificate->isBlacklisted())
    {
        emit e("server public local certificate is blacklisted");
        return;
    }
}
bool QtHttpsServer::startQtHttpServer(quint16 port)
{
    return listen(QHostAddress::Any, port);
}
QtHttpsServer::~QtHttpsServer()
{
    if(m_ServerPrivateEncryptionKey)
    {
        delete m_ServerPrivateEncryptionKey;
    }
    if(m_ServerPublicLocalCertificate)
    {
        delete m_ServerPublicLocalCertificate;
    }
}
void QtHttpsServer::incomingConnection(qintptr socketDescriptor)
{
    QSslSocket *clientSocket = new QSslSocket(this);
    if(clientSocket->setSocketDescriptor(socketDescriptor))
    {
        clientSocket->setPrivateKey(*m_ServerPrivateEncryptionKey);
        clientSocket->setLocalCertificate(*m_ServerPublicLocalCertificate);
        new QtHttpsClient(clientSocket, clientSocket);
        clientSocket->startServerEncryption();
    }
    else
    {
        delete clientSocket;
    }
}
void QtHttpsServer::handleNewConnection()
{
    //QTcpSocket *newConnection = nextPendingConnection();
    //new QtHttpsClient(newConnection, newConnection);
}
