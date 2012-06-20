#include "serverandprotocolknower.h"

ServerAndProtocolKnower::ServerAndProtocolKnower(QObject *parent) :
    QObject(parent), m_SslTcpServer(0)
{
    if(!m_SslTcpServer)
    {
        new SslTcpServer(this, ":/serverCa.pem", ":/clientCa.pem", ":/serverPrivateEncryptionKey.pem", ":/serverPublicLocalCertificate.pem", "fuckyou" /* TODOopt: make it so when starting the server we are prompted for this passphrase. this way it is only ever stored in memory... which is an improvement but still not perfect */); //since this code is auto-generated, we don't really need to put all the public/private key shits in the constructor as arguments. HOWEVER, it is still very good for the re-use of SslTcpServer, so i'll just leave it. it makes no difference in the auto-generated scenario where the string replaced variables are located, is all i'm saying
        connect(m_SslTcpServer, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
        connect(m_SslTcpServer, SIGNAL(clientConnectedAndEncrypted(QSslSocket*)), this, SLOT(handleClientConnectedAndEncrypted(QSslSocket*)));
        m_SslTcpServer->initAndStartListening();
    }
    //todo: put m_Server on it's own thread (maybe not, unsure. might not matter... might be safer to keep them on the same thread. i really don't know. the fact that it's a signal sort of implies it can be read from any thread... right?), connect to it's handleClientConnectedAndEncrypted(QSslSocket *client), and then connect client's readyRead to a slot in here that deciphers the protocol + emits

}
void ServerAndProtocolKnower::handleClientConnectedAndEncrypted(QSslSocket *client)
{
    connect(client, SIGNAL(readyRead()), this, SLOT(handleMessageReceivedFromRpcClientOverNetwork()));
}
void ServerAndProtocolKnower::handleMessageReceivedFromRpcClientOverNetwork()
{
    if(QSslSocket *secureSocket = qobject_cast<QSslSocket*>(sender()))
    {
        QDataStream stream(secureSocket);
        while(!stream.atEnd())
        {
            RpcBankServerMessage newMessage;
            newMessage.isResponse = false; //TODO: set by default in constructor OR cache message getter and remove this line.
            //right in this moment i have realized that it's insanely smart to make an rpc generator, pretty much right after i get createBankAccount and it's response hypothetically automated. creating the rest of the actions will become fun/easy
            stream >> newMessage;
            if(newMessage.m_MessageType != RpcBankServerMessage::RpcBankServerMessageActionType)
            {
                emit d("somehow got the wrong message type in handleMessageReceivedFromRpcClientOverNetwork");
                //WtFrontEndToAppDbMessage::returnAWtFrontEndToAppDbMessage(newMessage);
                return;
            }
            switch(newMessage.m_MessageAction)
            {
            case RpcBankServerMessageAction::CreateBankAccountAction:
                uint clientId = SslTcpServer::getClientUniqueId(secureSocket);
                if(m_NetworkClientIdAndPendingCreateBankAccountRequestsHash.contains(clientId))
                {
                    if(m_NetworkClientIdAndPendingCreateBankAccountRequestsHash.values(clientId).contains(newMessage.m_Username))
                    {
                        //TODO: deal with already pending. i think this is my race detection code. probably just respond instantly with the error + reason. TODOopt: maybe even check the pending request to see how long it's been since it was dispatched... and if it's been too long then we say wtf and/or request another
                        return; //so we don't emit/request the message
                    }
                }
                m_NetworkClientIdAndPendingCreateBankAccountRequestsHash.insert(clientId, newMessage.m_Username);
                emit createBankAccount(clientId, newMessage.m_Username); //the clientId is used in this case as the app specific bank account id. i think i remember writing somewhere that i was going to pass in some sort of ID for the first parameter. now i'm wondering if it's better to use a 32-bit request id and just match up the request id's with the requests (and therefore responses?). the user doesn't have to do anything with clientId, and on their end it can/should be called requestId. all they do is emit it back with their own input (the request... in this case, just the username string. can/might/should(?) be an object, which could mean the request id is/could-be a part of it. actually i kind of like that idea. i also need to keep genericizing of race conditions in mind, objects are harder to compare (default to pointer comparison instead of value comparison... but this can/should be changed))
            break;
            //TODO: this is where new actions go. we might be able to abstract the pending.contains + append + emit'ing between each action... but variable parameter amounts might mess that up (or just 1 helper per action? all auto-generated anyways. idfk. do whatever. make it work first tho ;-))
            case RpcBankServerMessageAction::InvalidAction:
            default:
                //TODO: i guess respond to the client saying "wtf that message was fucked"... and i guess it can try sending another? infinite loop of errors potentially here. TODOopt: catch infinite loop (lol gl with that... you need a concept of recycling the cookie and the message/message handlers knowing it's a retry and/or how many times it has retried). basically, ideally we'd want to be lenient but after like 3-5 retries we'd not retry anymore and note of a system error to the admins. dump variables etc
                break;
            }
        }
    }
}
void ServerAndProtocolKnower::createBankAccountCompleted(const QString &username)
{
    m_NetworkClientIdAndPendingCreateBankAccountRequestsHash.re
    //m_PendingCreateBankAccountRequests.removeOne(username); //TODOreq: should this be a list/hash of requests so that i can retrieve it? or should it be just for use in detecting the race condition of one already pending.... OR BOTH????????????
    //TODO: send the success message across the network

    //TODO: re-using would mean we only have the message as an RpcBankServerMessage, not an RpcBankServerMessageAction. it shouldn't matter though....
    RpcBankServerMessageAction message;
    message.isResponse = true;
    message.m_MessageAction = RpcBankServerMessageAction::CreateBankAccountAction;
    message.m_Username = username;

    //TODO: move below into own method
    QDataStream stream(socket lol wtf i need to associate this with the request too..);
    stream << message;

    //TODOreq: request/response identification on so many levels: so the request needs to be identified by a magic cookie/identifier when it comes back over the network... it needs to be associated by value to detect for pending and to line back up with the request to rpc server impl... and it needs to use an identifier (uint cert serial number atm) to associate the request (and therefore who to send the response to) with a socket

    //^^^^
    //what network message (response) goes with which request?
    //what pending request for the associated action is the response associated with (using only the parameters?
    //what socket does the request/response that we just got back from the rpc server impl correspond to?
    //TODOreq: there is no pre-existing request or socket for broadcasts... and yet the broadcast still needs to select the correct socket.... without even knowing of their existence
}
