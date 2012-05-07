#include "clientshelper.h"

ClientsHelper::ClientsHelper(QObject *parent) :
    QObject(parent), m_SslTcpServer(0)
{
}
void ClientsHelper::init()
{
    if(!m_SslTcpServer)
    {
        new SslTcpServer(this, ":/serverCa.pem", ":/clientCa.pem", ":/serverPrivateEncryptionKey.pem", ":/serverPublicLocalCertificate.pem", "fuckyou" /* TODOopt: make it so when starting the server we are prompted for this passphrase. this way it is only ever stored in memory... which is an improvement but still not perfect */);
        connect(m_SslTcpServer, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
        connect(m_SslTcpServer, SIGNAL(clientConnectedAndEncrypted(QSslSocket*)), this, SLOT(handleClientConnectedAndEncrypted(QSslSocket*)));
        m_SslTcpServer->initAndStartListening();
    }
}
void ClientsHelper::handleClientConnectedAndEncrypted(QSslSocket *client)
{
    connect(client, SIGNAL(readyRead()), this, SLOT(handleClientSentUsData()));
}
void ClientsHelper::handleClientSentUsData()
{
    emit d("received data from client");
    if(QSslSocket *secureSocket = qobject_cast<QSslSocket*>(sender()))
    {
        QDataStream stream(secureSocket);
        while(!stream.atEnd())
        {
            WtFrontEndToAppDbMessage *newMessage = WtFrontEndToAppDbMessage::giveMeAWtFrontEndToAppDbMessage(SslTcpServer::getClientUniqueId(secureSocket));
            stream >> *newMessage;
            if(newMessage->m_WtFrontEndAndAppDbMessageType != WtFrontEndAndAppDbMessage::WtFrontEndToAppDbMessageType)
            {
                emit d("somehow got the wrong message type in handleWtFrontEndSentUsData");
                WtFrontEndToAppDbMessage::returnAWtFrontEndToAppDbMessage(newMessage);
                return;
            }
            //then, now that the message has been read off the network, we can get the appropriate type of AppLogicRequest (the inherited type... even though we refer to it by pointer here... AS WELL AS set the newMessage to be a child of it... so when we recycle the AppLogicRequest* (a hash or something, with 'theMessage' as the key, and a QList<AppLogicRequest*> as the value), we also recycle the newMessage too
            AppLogicRequest *inheritedAppLogicRequest = AppLogicRequest::giveMeAnInheritedAppLogicRequestBasedOnTheMessage(newMessage);

            emit requestFSromWtFrontEnd(newMessage);
        }
    }
}
