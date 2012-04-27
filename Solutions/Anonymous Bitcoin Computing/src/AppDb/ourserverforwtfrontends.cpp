#include "ourserverforwtfrontends.h"

OurServerForWtFrontEnds::OurServerForWtFrontEnds(QObject *parent) :
    QObject(parent), m_SslTcpServer(0)
{
}
void OurServerForWtFrontEnds::startListeningForWtFrontEnds()
{
    //TODO: start the SSL server (contained. as in, it's logic should not mix with this class's logic........ which considering i have to implement QSslSocket anyways... shouldn't be TOO hard. but idk if i should have yet another class encapsulating even that). if i were to have it contained, the contained layer would have to know about my protocol. i could maybe send a generic protocol to it in it's constructor.. but that doesn't sound worth the effort [atm!]. this way the container would just have a signal like "protocolMessageDetected(GenericProtocolMessage *message)"... and i'd [again] have to figure out what kind it is using a switch() based on the same shit i used as input for the constructor... AND it would also have 2 slots... sendMessageToSpecificClient and broadcastMessage......... OR SOMETHING. idfk and don't care to figure it out right now. i just know that my handleNewIncomingConnectionNotYetEncrypted() shit could probably be encapsulated/re-used

    if(!m_SslTcpServer)
    {
        m_SslTcpServer = new SslTcpServer(this, ":/serverCa.pem", ":/clientCa.pem", ":/serverPrivateEncryptionKey.pem", ":/serverPublicLocalCertificate.pem", "fuckyou" /* TODOopt: make it so when starting the server we are prompted for this passphrase. this way it is only ever stored in memory... which is an improvement but still not perfect */);
        connect(m_SslTcpServer, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
        connect(m_SslTcpServer, SIGNAL(clientConnectedAndEncrypted(uint)), this, SLOT(handleClientConnectedAndEncrypted(uint)));
        m_SslTcpServer->initAndStartListening();
    }
}
void OurServerForWtFrontEnds::handleResponseFromAppLogic(AppLogicRequestResponse *response)
{
}
void OurServerForWtFrontEnds::handleClientConnectedAndEncrypted(uint clientId, QSslSocket *client)
{
    //clientId is how we'll refer to it from now on... and we only need client right here to efficiently connect to it's readyRead signal
    //TODO: should i store the clientId here too? or in the socket code? idfk.
    connect(client, SIGNAL(readyRead()), this, SLOT(handleWtFrontEndSentUsData()));
    //TODOreq: send them the list of usernames that have a bank account already created
}
void OurServerForWtFrontEnds::handleWtFrontEndSentUsData()
{
    emit d("received data from wt front-end");
    if(QSslSocket *secureSocket = qobject_cast<QSslSocket*>(sender()))
    {
        QDataStream stream(secureSocket);
        while(!stream.atEnd())
        {
            WtFrontEndToAppDbMessage message;
            stream >> message;
            if(message.m_WtFrontEndAndAppDbMessageType != WtFrontEndAndAppDbMessage::WtFrontEndToAppDbMessageType)
            {
                emit d("somehow got the wrong message type");
                return;
            }
                emit requestFromWtFrontEnd(AppLogicRequest::fromWtFrontEndToAppDbMessage(message));
        }
    }
}
