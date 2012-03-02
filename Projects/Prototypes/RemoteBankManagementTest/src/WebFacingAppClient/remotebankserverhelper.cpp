#include "remotebankserverhelper.h"

RemoteBankServerHelper::RemoteBankServerHelper(QObject *parent) :
    QObject(parent)
{
    //todo: move this shit out of the constructor. our emit d()'s our not being received because we can't connect to the signal until after we construct the object. we're still getting the "connected and encrypted" signal though (read: it's working) so whatever for now...

    m_SslSocket = new QSslSocket(this);
    //connect(m_SslSocket, SIGNAL(connected()), this, SLOT(handleConnectedNotYetEncrypted())); we dgaf
    connect(m_SslSocket, SIGNAL(encrypted()), this, SLOT(handleConnectedAndEncrypted()));
    connect(m_SslSocket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(handleSslErrors(QList<QSslError>)));
    connect(m_SslSocket, SIGNAL(disconnected()), this, SLOT(handleDisconnect()));
    connect(m_SslSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleSocketError(QAbstractSocket::SocketError)));

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

    //client CA
    QFile clientCaFileResource(":/clientCA.pem");
    clientCaFileResource.open(QFile::ReadOnly);
    QByteArray clientCaByteArray = clientCaFileResource.readAll();
    clientCaFileResource.close();

    QSslCertificate clientCA(clientCaByteArray);
    if(clientCA.isNull())
    {
        emit d("client CA is null");
        return;
    }
    emit d("client ca is not null");


    QList<QSslCertificate> allCertificateAuthorities;
    allCertificateAuthorities.append(serverCA);
    allCertificateAuthorities.append(clientCA);

    m_SslSocket->setCaCertificates(allCertificateAuthorities);

    QByteArray passPhrase("fuckyou");
    m_SslSocket->setPrivateKey(":/clientPrivateKey.pem", QSsl::Rsa, QSsl::Pem, passPhrase);
    m_SslSocket->setLocalCertificate(":/clientPublicCert.pem");

    m_SslSocket->connectToHostEncrypted("localhost", 6969);
    emit d("attempting to connect at port 6969");

    /*if(!m_SslSocket->waitForEncrypted())
    {
        emit d(m_SslSocket->errorString());
    }
    emit d("got past wait");*/
}
void RemoteBankServerHelper::addUser(QString userToAdd)
{
    sendAddUserMessage(userToAdd);
}
bool RemoteBankServerHelper::isConnected()
{
    bool weAreConnected = false;
    if(!m_SslSocket->isEncrypted())
    {
        if(m_SslSocket->waitForEncrypted(10000))
        {
            weAreConnected = true;
        }
    }
    else
    {
        weAreConnected = true;
    }
    if(!weAreConnected)
    {
        emit d("failed to connect in/at/before/during isConnected()");
    }
    return weAreConnected;
}
void RemoteBankServerHelper::sendAddUserMessage(QString userToAdd)
{
    //protocol struct + sned via datastream/ssl socket
    if(isConnected())
    {
        ClientToServerMessage addUserMessage(MY_CLIENT_APP_ID, ClientToServerMessage::AddUserWithThisName, userToAdd);
        QDataStream stream(m_SslSocket);
        stream << addUserMessage;
    }
    else
    {
        emit d("unable to send add user message -- isConnected() returned false");
        //todo: for the server (clienthelper), it's a big deal if we're not connected when trying to send a message.. but for the client (this), we can simply show the user an error 'service temporarily unavailable' and stop
    }
}
void RemoteBankServerHelper::handleConnectedAndEncrypted()
{
    emit d("connected and encrypted");
    connect(m_SslSocket, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));
    //sendAppIdMessage();
}
/*
void RemoteBankServerHelper::sendAppIdMessage()
{
    //this is my 'hello' of sorts. i send my application id to the server, which the server uses to identify us and our remote bank account (users have accounts that belong to the app blanket bank account). i have to receive an app_id_ok message before i can start doing addUser, etc. TODO: fix isConnected() to make sure we have app_id_ok received. making it wait() for app_id_ok might be a bitch...
    //ALTERNATIVELY, i can send the app_id with each and every message... wastes a bit of bandwidth, however.
    //going with sending appId with every message. fuck bandwidth. makes management on the server side easier/more sturdy (maybe not) too. idk how multiple clients/1-server functions [yet]... it might even be necessary... it might just be wasteful (my guess).

    //TODO: test multiple client connections (multiple app_id's) to the server

    if(isConnected())
    {
        ClientToServerMessage appIdMessage;
        appIdMessage.m_TheMessage = ClientToServerMessage::HeresMyAppId;
        appIdMessage.m_ExtraString = MY_CLIENT_APP_ID;
        QDataStream stream(m_SslSocket);
        stream << appIdMessage;
    }
}
*/
void RemoteBankServerHelper::handleSslErrors(QList<QSslError> sslErrors)
{
    int numErrors = sslErrors.count();
    emit d(QString::number(numErrors) + " ssl errors");
    for(int i = 0; i < numErrors; ++i)
    {
        emit d("ssl error #" + QString::number(i) + " - " + sslErrors.at(i).errorString());
    }
}
void RemoteBankServerHelper::handleReadyRead()
{
    emit d("received data from server");
    QDataStream stream(m_SslSocket);
    while(!stream.atEnd())
    {
        ServerToClientMessage message;
        stream >> message;
        if(message.m_MessageType != Message::ServerToClientMessageType)
        {
            emit d("somehow got a message that wasn't a server2client message");
            return;
        }
        if(message.m_AppId != MY_CLIENT_APP_ID)
        {
            emit d("server gave us a message that doesn't match our app id");
            //todo: notify the server of this error. log it so we can fix the bug. i doubt it'll ever happen though... but you never know
            return;
        }
        switch(message.m_TheMessage)
        {
        case ServerToClientMessage::InvalidAppId:
            //todo: this would be the case where the app_id we sent is not registered on the server. but it IS equal to the one we sent, otherwise we wouldn't get to this switch statement
            emit d("invalid app id -- check server");
            break;
        case ServerToClientMessage::UserAdded:
            //todo: get the bank id, emit so parent can save it + update gui
            emit userAdded(message.m_ExtraString /*username*/);
            break;
        default:
            emit d("received invalid server2client message");
            return;
        }
    }
}
void RemoteBankServerHelper::handleSocketError(QAbstractSocket::SocketError socketError)
{
    emit d("abstract socket error #" + QString::number(socketError));
}
