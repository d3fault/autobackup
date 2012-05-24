#include "ssltcpserver.h"

SslTcpServer::SslTcpServer(QObject *parent, const QString &serverCaFile, const QString &clientCaFile, const QString &serverPrivateEncryptionKeyFile, const QString &serverPublicLocalCertificateFile, const QString &serverPrivateEncryptionKeyPassPhrase) :
    QTcpServer(parent), m_ServerCaFile(serverCaFile), m_ClientCaFile(clientCaFile), m_ServerPrivateEncryptionKeyFile(serverPrivateEncryptionKeyFile), m_ServerPrivateEncryptionKeyPassPhrase(serverPrivateEncryptionKeyPassPhrase), m_ServerPublicLocalCertificateFile(serverPublicLocalCertificateFile)
{
    //we moved our constructor to init() so that our emit d()'s would work
}
void SslTcpServer::initAndStartListening()
{
    if(!QSslSocket::supportsSsl())
    {
        emit d("ssl is not supported");
        return;
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
        return;
    }
    emit d("the server certificate is not null");

    //Client CA is a list of certificates that the server uses to verify the authenticity of the clients. each client can use the same certificate, but for our purposes we're going to have each client having a unique certificate. it is how we will identify them.
    QFile clientCaFileResource(m_ClientCaFile);
    clientCaFileResource.open(QFile::ReadOnly);
    QByteArray clientCaByteArray = clientCaFileResource.readAll();
    clientCaFileResource.close();

    QSslCertificate clientCA(clientCaByteArray);
    if(clientCA.isNull())
    {
        emit d("client CA is null");
        return;
    }
    emit d("client CA is not null");

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
        return;
    }
    emit d("server private encryption key is not null");

    //Server Public Local Certificate is the public decryption key that we send to our client
    QFile serverPublicLocalCertificateFileResource(m_ServerPublicLocalCertificateFile);
    serverPublicLocalCertificateFileResource.open(QFile::ReadOnly);
    QByteArray serverPublicLocalCertificateByteArray = serverPublicLocalCertificateFileResource.readAll();
    serverPublicLocalCertificateFileResource.close();

    m_ServerPublicLocalCertificate = new QSslCertificate(serverPublicLocalCertificateByteArray);
    if(m_ServerPublicLocalCertificate->isNull() /*TODO: also isValid?*/)
    {
        emit d("server public local certificate is null");
        return;
    }
    emit d("server public local certificate is not null");

    connect(this, SIGNAL(newConnection()), this, SLOT(handleNewConnectionNotYetEncrypted()));

    this->listen(QHostAddress::LocalHost, 6969);
    emit d("now listening on port 6969");
}
void SslTcpServer::incomingConnection(int handle)
{
    QSslSocket *secureSocket = new QSslSocket(this);
    if(secureSocket->setSocketDescriptor(handle))
    {
        secureSocket->setCaCertificates(m_AllMyCertificateAuthorities);

        secureSocket->setPrivateKey(*m_ServerPrivateEncryptionKey);

        secureSocket->setLocalCertificate(*m_ServerPublicLocalCertificate);

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
        //connect(secureSocket, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));

        //should i emit clientConnectedAndEncrypted(secureSocket), allowing my parent class to process the messages using it's app-specific prototol? i like the idea of using a list of connected certificate serial numbers so as to detect a duplicate connection... in the case our [to be written] 'alive?' code fails to detect a connection drop. there is no reason that multiple clients can't authenticate using the same client public certificate (and it still passes our clientCA vefification test)..... UNLESS we keep track of the currently connected serial numbers (corresponding to the certificates). this comment is mainly just me thinking out loud. i should keep the serial number private, and emit the socket pointer so i can listen to it's readyRead. but i don't like the idea of passing around the socket pointer within the application logic (which is then passed back to use for telling us who to reply to). a serial number as a uint is better served at that, and it also means we can continue operation when a connection drops and reconnects (ie, the socket pointer changes). still, need to connect to that readyRead()!!!!!!
        //fml idk wtf to do. i'm overthinking this.
        //both a 32-bit pointer to a socket and a uint have the same overhead as far as passing them around... with 64-bit platforms doubling for the former (at least i think)
        //seeing as i don't want to double the overhead for 64-bit (because i very well may use 64-bit servers)... and i don't want the business logic to have to contain a QSslSocket pointer.... i should use the serial number in a uint as my identifier. badabing badabam all i gotta do is TTHHHIIIIINNNNNKKKKKKK

        //basically what's fucking with my head right here is that i'm using the serialNumber as an ID for two separate purposes:
        //A) to detect duplicate connections (in the case we don't detect a connection drop)
        //B) to identify which client to send an AppLogicRequest[Response] back to

        //i could use an IP address for both it really doesn't matter...

        //just don't forget that they are TWO SEPARATE REASONS and you should be alright
        //but should they share the same QHash<uint,QSslSocket*> ........ ?????????...... that's what's really getting me. and is QSslSocket* the right value type for that hash? where will i keep track of the last 'i-AM-alive' received? could just be an object that has that QDateTime + the QSslSocket... ya dig?
        //i am also writing/designing around code that i am not even implementing yet (connection management)... so meh.

        //for now, just code for B.

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
