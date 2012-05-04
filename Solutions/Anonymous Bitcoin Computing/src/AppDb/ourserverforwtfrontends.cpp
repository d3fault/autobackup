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

    //somewhere in here we're going to need to build an AppDbToWtFrontEndMessage... and i'm thinking the 'theMessage' part of it can be determined automatically based on what the request's 'theMessage' is. we can have a static function in the protocol called like 'responseForRequest(request)'... all enum based... so we don't even need to fill one out. but there are still definitely going to be cases where we need to assign variables to the AppDbToWtFrontEndMessage..... can that be done in AppLogicRequest static method? (or i suppose AppLogicRequestResponse is probably a better place... since we're now in the response phase) that static method in AppLogicRequestResponse could be the one to dig into response->parentRequest()->m_WtFrontEndToAppDbMessage->m_ExtraString0 and "KNOW" (based on the fact that it's currently in a switch for 'theMessage' (response, as noted in this very comment) that m_ExtraString0 is the username (FOR EXAMPLE)

    //something like m_TcpSocket->sendResponseTo(response->parentRequest()->getRequestorId());

    //AppLogicRequest::returnAnAppLogicRequest(response->parentRequest());
    AppLogicRequest::returnAnInheritedAppLogicRequest(response->parentRequest());
}
void OurServerForWtFrontEnds::handleClientConnectedAndEncrypted(QSslSocket *client)
{
    connect(client, SIGNAL(readyRead()), this, SLOT(handleWtFrontEndSentUsData()));
    //TODOreq: re: list of usernames with bank account - this class should have it's own copy of the list... so we don't have to bug AppLogic (though then again, it's not like Wt Front Ends connect a whole lot...). but anyways, when a createBankAccount app logic method succeeds, it emits to us that fact, as well as the new username. we then broadcast it to ever wt front-end. this is SEPARATE from the AppLogicRequestResponse specifically to the wt front-end/user that we made a new bank account for. createBankAccount in app logic should have 2 emits on success. HOWEVER, for the wt front-end that the user is currently on, it would be possible to use the broadcast notification as the response. this might overly complicate things (but then again, receiving notification that a bank account has been created... TWICE... might complicate things too (TODOreq: make sure that the one that is propagated to the user does not conflict/make-the-same-'alreadyExists?'-sanity-checks as when the code receives the broadcast notify))
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
            //AppLogicRequest *request = AppLogicRequest::giveMeAnAppLogicRequest(SslTcpServer::getClientUniqueId(secureSocket)); //gets a new/recycled AppLogicRequest and sets it's clientId so we know who to respond to later on...


            //in order to know what kind of INHERITED AppLogicRequest (for example: CreateBankAccountAppLogicRequest : AppLogicRequest .... with virtual methods called to do the work)... we must first read the message from the network and determine 'theMessage'.

            //so maybe (with recycling included):
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

            emit requestFromWtFrontEnd(newMessage);
        }
    }
}
