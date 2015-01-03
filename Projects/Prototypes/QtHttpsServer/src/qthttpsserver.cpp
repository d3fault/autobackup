#include "qthttpsserver.h"

#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QSslSocket>
#include <QFile>
#include <QtNetwork/QSslKey>
#include <QtNetwork/QSslCertificate>

#include "qthttpsclient.h"

//KISS one url https server, to test that it works from the browser
QtHttpsServer::QtHttpsServer(QObject *parent)
    : QTcpServer(parent)
    , m_ServerPrivateEncryptionKey(0)
    , m_ServerPublicLocalCertificate(0)
{
    //connect(this, SIGNAL(newConnection()), this, SLOT(handleNewConnection()));
}
bool QtHttpsServer::startQtHttpServer(quint16 port)
{
#if 0
    QFile serverCaFileResource("/run/shm/demoCA/cacert.pem");
    if(!serverCaFileResource.open(QFile::ReadOnly))
    {
        emit e("CA could not be opened");
        return false;
    }
    QByteArray serverCaByteArray = serverCaFileResource.readAll();
    QSslCertificate ca(serverCaByteArray);
    if(ca.isNull())
    {
        emit e("CA is null");
        return false;
    }
    if(ca.isBlacklisted())
    {
        emit e("CA is blacklisted");
        return false;
    }
    m_CAs.append(ca);
#endif

    QFile serverPrivateEncryptionKeyFileResource("/run/shm/server.key");
    if(!serverPrivateEncryptionKeyFileResource.open(QFile::ReadOnly))
    {
        emit e("server.key could not be opened");
        return false;
    }
    QByteArray serverPrivateEncryptionKeyByteArray = serverPrivateEncryptionKeyFileResource.readAll();

    QByteArray privateKeyPassphraseBA("fuckyou");
    m_ServerPrivateEncryptionKey = new QSslKey(serverPrivateEncryptionKeyByteArray, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey, privateKeyPassphraseBA);

    if(m_ServerPrivateEncryptionKey->isNull())
    {
        emit e("server private encryption key is null");
        return false;
    }

    //Server Public Local Certificate is the public decryption key that we send to our client
    QFile serverPublicLocalCertificateFileResource("/run/shm/server.pem");
    if(!serverPublicLocalCertificateFileResource.open(QFile::ReadOnly))
    {
        emit e("server.pem could not be opened");
        return false;
    }
    QByteArray serverPublicLocalCertificateByteArray = serverPublicLocalCertificateFileResource.readAll();

    m_ServerPublicLocalCertificate = new QSslCertificate(serverPublicLocalCertificateByteArray);
    if(m_ServerPublicLocalCertificate->isNull())
    {
        emit e("server public local certificate is null");
        return false;
    }
    if(m_ServerPublicLocalCertificate->isBlacklisted())
    {
        emit e("server public local certificate is blacklisted");
        return false;
    }

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
        //clientSocket->setCaCertificates(m_CAs);
        clientSocket->setPrivateKey(*m_ServerPrivateEncryptionKey);
        clientSocket->setLocalCertificate(*m_ServerPublicLocalCertificate);
        QtHttpsClient *httpsClient = new QtHttpsClient(clientSocket);
        connect(httpsClient, SIGNAL(e(QString)), this, SIGNAL(e(QString)));
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
