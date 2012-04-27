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
        connect(m_SslTcpServer, SIGNAL(clientConnectedAndEncrypted(QSslSocket*)), this, SLOT(handleClientConnectedAndEncrypted(QSslSocket*)));
        m_SslTcpServer->initAndStartListening();
    }
}
void OurServerForWtFrontEnds::handleResponseFromAppLogic(AppLogicRequestResponse *response)
{
    //todo: blah blah blah
    //then, after we're done with the response... after we've sent the request over the network (blah blah blah), return the response to be recycled

    //something like m_TcpSocket->sendResponseTo(response->parentRequest()->getRequestorId());

    AppLogicRequest::returnAnAppLogicRequest(response->parentRequest());
}
void OurServerForWtFrontEnds::handleClientConnectedAndEncrypted(QSslSocket *client)
{
    connect(client, SIGNAL(readyRead()), this, SLOT(handleWtFrontEndSentUsData()));
    //TODOreq: send them the list of usernames that have a bank account already created.... and TODOreq further, make sure that when a new incoming duplicate connection (didn't detect dropped)... we DON'T send them the list of users again? man this is getting stupid confusing.
    //a) they disconnect and reconnect, we send them the list of usernames again
    //b) the connection drops and then picks back up (???? same QSslSocket* ????), we do not send them the list of usernames again
    //c) the connection drops and then they re-establish a new one (new QSslSocket*, but same certificate serial number) [before we notice the other one is dropped... otherwise it would appear to us as (a)]

    //^^^^^this is really ssltcpserver code, but i'm just venting it with relation to the 'send list of usernames on first connection' as it is a use case and makes it easier to comprehend

    //ok now that i've written the //something like m_TcpSocket->sendResponseTo(response->parentRequest()->getRequestorId()); in handleResponseFromAppLogic... i see that THIS CLASS doesn't need to give a fuck about a list of currently connected QSslSockets. woot.
}
void OurServerForWtFrontEnds::handleWtFrontEndSentUsData()
{
    emit d("received data from wt front-end");
    if(QSslSocket *secureSocket = qobject_cast<QSslSocket*>(sender()))
    {
        QDataStream stream(secureSocket);
        while(!stream.atEnd())
        {
            AppLogicRequest *request = AppLogicRequest::giveMeAnAppLogicRequest(SslTcpServer::getClientUniqueId(secureSocket)); //gets a new/recycled AppLogicRequest and sets it's clientId so we know who to respond to later on...
            stream >> *(request->m_WtFrontEndToAppDbMessage); //i hope dereferencing this and re-using it is okay!!!! lol 'hope'. TODOreq: write small prototype to make sure this shit works
            if(request->m_WtFrontEndToAppDbMessage->m_WtFrontEndAndAppDbMessageType != WtFrontEndAndAppDbMessage::WtFrontEndToAppDbMessageType)
            {
                emit d("somehow got the wrong message type in handleWtFrontEndSentUsData");
                AppLogicRequest::returnAnAppLogicRequest(request); //no point in leaving it on the heap!
                return;
            }
            emit requestFromWtFrontEnd(request);
        }
    }
}
