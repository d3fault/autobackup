#include "ssltcpserver.h"

SslTcpServer::SslTcpServer(QObject *parent, const QString &serverCaFile, const QString &clientCaFile, const QString &serverPrivateEncryptionKeyFile, const QString &serverPublicLocalCertificateFile, const QString &serverPrivateEncryptionKeyPassPhrase) :
    QTcpServer(parent), m_ServerCaFile(serverCaFile), m_ClientCaFile(clientCaFile), m_ServerPrivateEncryptionKeyFile(serverPrivateEncryptionKeyFile), m_ServerPrivateEncryptionKeyPassPhrase(serverPrivateEncryptionKeyPassPhrase), m_ServerPublicLocalCertificateFile(serverPublicLocalCertificateFile)
{ }
bool SslTcpServer::init()
{
    if(!QSslSocket::supportsSsl())
    {
        emit d("ssl is not supported");
        return false;
    }

    //Server CA is what the client's use to verify the authenticity of the server. It is a "List", but in our case only contains our server's Local Certificate. It has to be 'securely' copied to the client connection. both sides have it set. On this side, we only use it when loading our public local certificate to see if it's on the list (and verify against it).
    QFile serverCaFileResource(m_ServerCaFile);
    serverCaFileResource.open(QFile::ReadOnly);
    QByteArray serverCaByteArray = serverCaFileResource.readAll();
    serverCaFileResource.close();

    QSslCertificate serverCA(serverCaByteArray);
    if(serverCA.isNull())
    {
        emit d("the server CA is null");
        return false;
    }
    emit d("the server CA is not null");
    if(!serverCA.isValid())
    {
        emit d("the server CA is not valid");
        return false;
    }
    emit d("the server CA is valid");

    //Client CA is a list of certificates that the server uses to verify the authenticity of the clients. each client can use the same certificate, but for our purposes we're going to have each client having a unique certificate. it is how we will identify them.
    QFile clientCaFileResource(m_ClientCaFile);
    clientCaFileResource.open(QFile::ReadOnly);
    QByteArray clientCaByteArray = clientCaFileResource.readAll();
    clientCaFileResource.close();

    QSslCertificate clientCA(clientCaByteArray);
    if(clientCA.isNull())
    {
        emit d("client CA is null");
        return false;
    }
    emit d("client CA is not null");
    if(!clientCA.isValid())
    {
        emit d("client CA is not valid");
        return false;
    }
    emit d("client CA is valid");

    m_AllMyCertificateAuthorities.append(serverCA);
    m_AllMyCertificateAuthorities.append(clientCA);


    //Server Private Encryption Key is the private portion of our Local Certificate
    QByteArray serverPrivateEncryptionKeyPassPhraseByteArray(m_ServerPrivateEncryptionKeyPassPhrase.toUtf8()); //TODO: not sure if .toUtf8() is right... even though it does in fact return a QByteArray. idfk what kind it expects.. shit doesn't say

    QFile serverPrivateEncryptionKeyFileResource(m_ServerPrivateEncryptionKeyFile);
    serverPrivateEncryptionKeyFileResource.open(QFile::ReadOnly);
    QByteArray serverPrivateEncryptionKeyByteArray = serverPrivateEncryptionKeyFileResource.readAll();
    serverCaFileResource.close();

    m_ServerPrivateEncryptionKey = new QSslKey(serverPrivateEncryptionKeyByteArray, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey, serverPrivateEncryptionKeyPassPhraseByteArray);

    if(m_ServerPrivateEncryptionKey->isNull())
    {
        emit d("server private encryption key is null");
        return false;
    }
    emit d("server private encryption key is not null");

    //Server Public Local Certificate is the public decryption key that we send to our client
    QFile serverPublicLocalCertificateFileResource(m_ServerPublicLocalCertificateFile);
    serverPublicLocalCertificateFileResource.open(QFile::ReadOnly);
    QByteArray serverPublicLocalCertificateByteArray = serverPublicLocalCertificateFileResource.readAll();
    serverPublicLocalCertificateFileResource.close();

    m_ServerPublicLocalCertificate = new QSslCertificate(serverPublicLocalCertificateByteArray);
    if(m_ServerPublicLocalCertificate->isNull())
    {
        emit d("server public local certificate is null");
        return false;
    }
    emit d("server public local certificate is not null");

    connect(this, SIGNAL(newConnection()), this, SLOT(handleNewConnectionNotYetEncrypted()));

    return true;
}
bool SslTcpServer::start()
{
    if(this->listen(QHostAddress::LocalHost, 6969))
    {
        emit d("now listening on port 6969");
        return true;
    }
    emit d("listen failed");
    return false;
}
void SslTcpServer::stop()
{
    this->close();
    emit d("stopping ssl tcp server");
}
void SslTcpServer::incomingConnection(int handle)
{
    QSslSocket *secureSocket = new QSslSocket(this);
    if(secureSocket->setSocketDescriptor(handle))
    {
        secureSocket->setCaCertificates(m_AllMyCertificateAuthorities);

        secureSocket->setPrivateKey(*m_ServerPrivateEncryptionKey);

        secureSocket->setLocalCertificate(*m_ServerPublicLocalCertificate);

        //makes us request + REQUIRE that the client cert is valid. client does this to us by default, but we don't do it to him by default. i should still explicitly set it for client
        secureSocket->setPeerVerifyMode(QSslSocket::VerifyPeer);

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
QTcpSocket *SslTcpServer::nextPendingConnection()
{
    if(m_PendingConnections.isEmpty())
    {
        return 0;
    }
    return m_PendingConnections.dequeue();
}
void SslTcpServer::handleNewConnectionNotYetEncrypted()
{
    emit d("new connection, not yet encrypted");
    QSslSocket *newConnection = qobject_cast<QSslSocket*>(this->nextPendingConnection());
    if(newConnection)
    {
        connect(newConnection, SIGNAL(encrypted()), this, SLOT(handleConnectedAndEncrypted()));
        connect(newConnection, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(handleSslErrors(QList<QSslError>)));
        connect(newConnection, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleSocketError(QAbstractSocket::SocketError)));
        emit d("got a new connection from nextPendingConnection()");
    }
    else
    {
        emit d("nextPendingConnection() returned 0");
    }
}
void SslTcpServer::handleConnectedAndEncrypted()
{
    emit d("connection ENCRYPTED");
    QSslSocket *secureSocket = qobject_cast<QSslSocket*>(sender());
    if(secureSocket)
    {
#ifdef ABC_CONNECTION_MANAGEMENT
        uint clientSerialNumber = secureSocket->peerCertificate().serialNumber().toUInt();
        if(m_EncryptedSocketsBySerialNumber.contains(clientSerialNumber))
        {
            //TODOreq: they've already connected... which i guess would mean that they lost connection and are now reconnecting... and we didn't detect it and remove them
            //TODOreq: deal with the un-flushed queue of shit we thought we sent or something or whatever idfk. don't just forget
            m_EncryptedSocketsBySerialNumber[clientSerialNumber] = secureSocket; //for now, just overwrite the old socket
        }
        else
        {
            //not in our hash yet, so add it
            m_EncryptedSocketsBySerialNumber.insert(clientSerialNumber, secureSocket);
            emit clientConnectedAndEncrypted(clientSerialNumber, secureSocket);
        }
#endif
        //this might be used later in connection management, but right now i just want getSocketByUniqueId(uint id);
        //m_SocketsByUniqueId
        //lol found out i already had one from commented out connection management attempt
        m_EncryptedSocketsBySerialNumber.insert(getClientUniqueId(secureSocket), secureSocket);
        //i wonder if it's  more efficient to just store teh socket. this way at least i ensure only 1 per cert (don't i?)

        emit clientConnectedAndEncrypted(secureSocket);
    }
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
void SslTcpServer::handleSocketError(QAbstractSocket::SocketError abstractSocketError)
{
    emit d("abstract socket error #" + QString::number(abstractSocketError));
}
uint SslTcpServer::getClientUniqueId(QSslSocket *client)
{
    return client->peerCertificate().serialNumber().toUInt(); //so i can change this for ip address or something easily if i need to
}
QSslSocket *SslTcpServer::getSocketByUniqueId(uint uniqueId)
{
    return m_EncryptedSocketsBySerialNumber.value(uniqueId);
}
QList<uint> SslTcpServer::getAllConnectedUniqueIds()
{
    return m_EncryptedSocketsBySerialNumber.keys();
}
