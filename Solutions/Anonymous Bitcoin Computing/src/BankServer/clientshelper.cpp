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
            ClientToBankServerMessage message;
            stream >> message;
            if(message.shit())
            {
                switch(message.action)
                {
                case createBankAccountRequest:
                    emit bankAccountCreationRequested(message.Username);
                    break;
                case balanceTransferRequest:
                    emit balanceTransferRequested(message.Username, message.Amount);
                    break;
                default:
                    emit error(); //todo
                    break;

                    //the point is, clienthelper and serverhelper (which lives in appdb land) encapsulate the appdb <--> bank server communication. clientshelper exposes an interface to bank server. a series of signals(above) and slots (to be created... examples: bankAccountCreated() and balanceTransferred())

                    //in the appdb land, serverhelper only knows about the appdb <--> bank server protocol and the interface it exposes (also via signals and slots). it DOES NOT and SHOULD NOT know about the AppDb itself. the signals/slots it exposes are pretty much analogous to the signals and slots we are defining right here for the bank server to use. a SLOT on the serverhelper called createBankAccount eventually comes SIGNAL bankAccountCreationRequested() right here in this clientshelper file, just a few lines up. protobuf probably is helpful here in avoiding the double api declaration. but oh well it's not a huge price to pay. the penalty really is that i have to keep the >> and << operator overloads in sync. that's really about it. protobuf does it for me, big whoop
                }
            }

            /*WtFrontEndToAppDbMessage *newMessage = WtFrontEndToAppDbMessage::giveMeAWtFrontEndToAppDbMessage(SslTcpServer::getClientUniqueId(secureSocket));
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
            */
        }
    }
}
