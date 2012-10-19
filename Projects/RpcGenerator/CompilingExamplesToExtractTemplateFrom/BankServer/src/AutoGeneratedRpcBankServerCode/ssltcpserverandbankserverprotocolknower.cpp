#include "ssltcpserverandbankserverprotocolknower.h"

#include "../../RpcBankServerAndClientShared/rpcbankserverheader.h"

SslTcpServerAndBankServerProtocolKnower::SslTcpServerAndBankServerProtocolKnower(QObject *parent)
    : IBankServerProtocolKnower(parent)
{
    m_SslTcpServer = new SslTcpServer(this, ":/RpcBankServerCA.pem", ":/RpcBankServerClientCA.pem", ":/RpcBankServerPrivateKey.pem", ":/RpcBankServerPublicCert.pem", "fuckyou" /* TODOopt: make it so when starting the server we are prompted for this passphrase. this way it is only ever stored in memory... which is an improvement but still not perfect */);
    connect(m_SslTcpServer, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
    connect(m_SslTcpServer, SIGNAL(clientConnectedAndEncrypted(QSslSocket*)), this, SLOT(handleClientConnectedAndEncrypted(QSslSocket*)));
}
void SslTcpServerAndBankServerProtocolKnower::init()
{
    emit d("SslTcpServerAndBankServerProtocolKnower received init message");
    if(m_SslTcpServer->init())
    {
        emit d("SslTcpServer successfully initialized");
        emit initialized();
    }
}
void SslTcpServerAndBankServerProtocolKnower::start()
{
    emit d("SslTcpServerAndBankServerProtocolKnower received init message");
    if(m_SslTcpServer->start())
    {
        emit d("SslTcpServer successfully started");
        emit started();
    }
}
void SslTcpServerAndBankServerProtocolKnower::stop()
{
    emit d("SslTcpServerAndBankServerProtocolKnower received stop message");
    m_SslTcpServer->stop();
    emit stopped();
}

//TODOreq: I can move this slot's implementation, and even handleMessageReceivedFromRpcClientOverNetwork's too, to my parent interface. The only difference is that I'll be static_cast'ing to a QAbstractSocket* instead (MAYBE a QIODevice* instead, idfk. Guess I can just keep going 'up' until I can no longer compile/function :-P). The only thing we need in this Ssl specific class is to connect to our underlying SslTcpServer's "clientConnectedAndEncrypted" (etc! disconnects too eventually) signal. But the slot I connect it to (the slot right below me, the one I'm talking about) can be in my parent implementation as it contains no Ssl specific code, nor does handleMessageReceivedFromRpcClientOverNetwork. It is also more consistent with the naming scheme of my parent interface, "IBankServerProtocolKnower". Right now Ssl knows the protocol rofl. It shouldn't. ALSO, if I do what I am describing in this comment, then this entire Ssl class becomes re-usable amongst multiple Rpc Services in the same binary or app or whatever idk what I'm trying to say exactly. There will be a "sharedAmongstRpcServices" folder in my AutoGeneratedRpcGeneratedCode folder (which is currently named AutoGeneratedRpcBankServerCode. I need to change that and then put the "Bank Server" code in a sub-folder of AutoGenerated[...]). Until I get the code working, I'm just going to leave it here because IF IT AIN'T BROKE DON'T FIX IT / KISS / in-the-middle-of-way-too-many-changes-right-now. So I guess I think I need to re-design this Ssl class in order for that to be effective.. how do I implement from multiple interfaces (rpc services)? Exactly, you can't. You need to re-think your design and then make this Ssl class operate on some IProtocolKnower interface or something, idfk. I _DONT_ need to do that re-design in order to move these slots to the parent... but I _DO_ need to do that re-design in order to let multiple Rpc Services use this exact same Ssl class.
void SslTcpServerAndBankServerProtocolKnower::handleClientConnectedAndEncrypted(QSslSocket *newClient)
{
    emit d("SslTcpServerAndBankServerProtocolKnower client connected and encrypted");
    connect(newClient, SIGNAL(readyRead()), this, SLOT(handleMessageReceivedFromRpcClientOverNetwork()));
}
void SslTcpServerAndBankServerProtocolKnower::handleMessageReceivedFromRpcClientOverNetwork()
{
    //Action Requests
    if(QSslSocket *secureSocket = static_cast<QSslSocket*>(sender()))
    {
        QDataStream stream(secureSocket);
        while(!stream.atEnd())
        {
            bool readHeaderDuringSomePreviousSlotInvocation = false;
            IRecycleableAndStreamable *messageOnlyIfWeHaveAlreadyReadItsHeaderAndNeedMoreDataForTheMessageItself__OrElseZero = m_HashOfMessagesBySocketAwaitingMoreData.value(secureSocket, 0);
            if(!messageOnlyIfWeHaveAlreadyReadItsHeaderAndNeedMoreDataForTheMessageItself__OrElseZero)
            {
                //We have not read the header yet. This means we didn't have enough data last time to read the header, or that we finished a message last time and are now starting a new one. TODOreq: remove the message from the hash after completing reading a message from the network. This is already done but need to verify it / stare at it (my introduction of a helper method for reading the message / only switch'ing once might change that)

                RpcBankServerMessageHeader header;
                if(secureSocket->bytesAvailable() >= sizeof(RpcBankServerMessageHeader))
                {
                    stream >> header;

                    //TODOreq: verify the header (magic) and simultaneously figure out which Rpc Service to use? omg this drastically changes my design rofl. The header processing needs to be in some shared interface class amongst all rpc services (NOT in the 'protocol knower'). Read the TODOreq above "handleClientConnectedAndEncrypted" for elaboration. I was not aware of what I just discovered while writing this TODOreq while writing that TODOreq

                    //we now know enough to allocate our message of a specific type. After we do that, we also have to copy the local/stack header onto the message's header pointer on the heap

                    switch(header.MessageType)
                    {
                    case RpcBankServerMessageHeader::CreateBankAccountMessageType:
                        messageOnlyIfWeHaveAlreadyReadItsHeaderAndNeedMoreDataForTheMessageItself__OrElseZero = m_CreateBankAccountMessageDispenser->getNewOrRecycled();
                    break;
                    case RpcBankServerMessageHeader::GetAddFundsKeyMessageType:
                        messageOnlyIfWeHaveAlreadyReadItsHeaderAndNeedMoreDataForTheMessageItself__OrElseZero = m_GetAddFundsKeyMessageDispenser->getNewOrRecycled();
                    break;
                    case RpcBankServerMessageHeader::InvalidMessageType:
                    default:
                    {
                        //TODOreq: handle this better, because we'll segfault if we get to copyLocalHeaderToMessageHeader. I'm not sure how whitelist programming factors in here. It should never happen... but we need to handle the case where it does. Maybe just eat the message? But how do we know that MessageSize is reliable? I guess that's what my Magic is for? Now I gotta figure out what to do if the magic fails :-/. I guess just be in an infinite "read-header" loop until magic matches? But then it makes me think magic should be the first POD and should be read uniquely independent of the header. Because what if somehow we just 'shifted' our header/messages on accident a bit (rogue bytes idfk). Basically a read-magic loop is a better loop because it will ensure alignment. The read-header loop means we might read magic into some other member of the header (id or type or whatever) and then consume it / skip it on accident. The magic could even be half in the Id and half in the Type or something, lmfao. Actually yea I do like that design... keep reading until we read in the correct magic. I mean yea also gotta check that there's enough bytes available to read in magic... but that's implied. Perhaps we need to separate Magic with RpcServiceId... but that's not a biggy. I am unsure. I envision a switch(magic) { case RpcServices::BankServer: } etc with the 'default' of that switch being the code path for my read-magic loop?
                        emit d("error: got invalid message type from client");
                        return;
                    }
                    break;
                    }

                    copyLocalHeaderToMessageHeader(header, messageOnlyIfWeHaveAlreadyReadItsHeaderAndNeedMoreDataForTheMessageItself__OrElseZero);
                }
                else
                {
                    break; //return would work too. we just want to get out of the while(!atEnd()) cycle, because it might still be returning true... and we've determined that whatever IS left... isn't enough. We need to wait for more
                }
            }
            else
            {
                //We have already read the header during some previous invocation of this slot. just need to make note of that so we don't leak memory later
                readHeaderDuringSomePreviousSlotInvocation = true;
            }

            //If we get here, we have read the header. Whether it was read in some previous invocation or right now is irrelevant, but stored in readHeaderDuringSomePreviousSlotInvocation for later usage. Note, the TODOoptimization below might change the code paths/logic, and subsequently this statement, a bit

            //TODOoptimization: so I don't have to switch the message type TWICE, I can do a "tryReadingMessageIfEnoughData" private method... and then have two places in this slot where it's called. Once after reading the header NOW (we've already done the switch!), and once for when we read the header on a previous slot invocation. We will have duplicate code (two switch statements), but I should code it so that only one of them is executed (depending on whether we have enough. One problem with this is that my 'generic' tryReadingMessageIfEnoughData has to call processCreateBankAccountMessage (or similar), so how the fuck is it going to do that and work with every message type? If I pass in an enum, I have to switch again. Rofl. I guess _MAYBE_ I could make it a pure virtual on the message itself? So it process itself? Dunno how much that fucks up / changes my design so might not be worth it at all. The pure virtual can't switch() either (which only applies if the pure virtual is in my parent interface.. not the message obv. I don't think moving it to the message will even work, but what the fuck do i know).

            quint16 messageSize = messageOnlyIfWeHaveAlreadyReadItsHeaderAndNeedMoreDataForTheMessageItself__OrElseZero->Header.MessageSize; //this optimization only saves me from a single extra deref (and however much looking up a member costs) in the next two if statements. premature optimize? prolly lolly
            if(secureSocket->bytesAvailable() >= messageSize)
            {
                if(messageSize > 0) //TODOreq: i think I should take this out because a successful createBankAccount message will have no MessageData. There is simply nothing to return (FALSE, 'Success' (and optional ErrorCode) is in the MessageData). TODOreq: don't stream the empty QByteArray onto the network when MessageSize is zero (because QByteArray has a header for the size of it's own <--- TODOreq: hmm maybe we can just peek() at that instead of storing the MessageSize?????? I mean, it's there anyways!!!). And of course, don't try to read it in (won't be there) if the MessageSize is zero ---------- ACTUALLY THAT IS WRONG, I think. Success is always streamed, and it's _INSIDE_ the MessageData. So perhaps this MessageSize > 0 _IS_ what I want?
                {
                    //this would be where I'd call that hypothetical tryReadingMessageIfEnoughData? or actually maybe before both those if's? idfk. You know come to think of it having the TWO WHOLE LINES that I use for streaming/processing duplicated in the header switch... isn't that fucking big of a deal at all. Oh and also the check to see if the sizes are good enough. Oh wait now I'm back to saying I should put it in a method :-P. facepalm.jpg

                    switch(messageOnlyIfWeHaveAlreadyReadItsHeaderAndNeedMoreDataForTheMessageItself__OrElseZero->Header.MessageType)
                    {
                    case RpcBankServerMessageHeader::CreateBankAccountMessageType:
                        {
                            //Hmmm, I think I should store the IRecycleableStreamableMessage inside the hash as the value instead of the Header. If we get a header, we will always allocate the message. So in effect it's the same. And the message has a header with it..

                            stream >> *messageOnlyIfWeHaveAlreadyReadItsHeaderAndNeedMoreDataForTheMessageItself__OrElseZero;
                            processCreateBankAccountMessage(messageOnlyIfWeHaveAlreadyReadItsHeaderAndNeedMoreDataForTheMessageItself__OrElseZero, SslTcpServer::getClientUniqueId(secureSocket)); //TODOreq: fix/change/whatever UniqueClientId shit so we can move this slot to our parent interface somewhere
                        }
                    break;
                    case RpcBankServerMessageHeader::GetAddFundsKeyMessageType:
                        {
                            stream >> *messageOnlyIfWeHaveAlreadyReadItsHeaderAndNeedMoreDataForTheMessageItself__OrElseZero;
                            processGetAddFundsKeyMessage(messageOnlyIfWeHaveAlreadyReadItsHeaderAndNeedMoreDataForTheMessageItself__OrElseZero, SslTcpServer::getClientUniqueId(secureSocket));
                        }
                    break;
                    case RpcBankServerMessageHeader::InvalidMessageType:
                    default:
                        //TODOreq: we definitely should never get here, because we'd get to the 'default' in the ABOVE header reading 'switch' for doing the getNewOrRecycled() shit. All the more reason to want to only have one switch point
                        emit d("error: got invalid message type from client");
                        return;
                    break;
                    }

                    //Now that we've read the full message, remove it from the hash so that our code knows that we're looking for a header again
                    m_HashOfMessagesBySocketAwaitingMoreData.remove(secureSocket);
                }
                else //TODOreq: make sure that broadcasts without any parameter types (haven't thought of any, but it definitely sounds like a use case that could occur are handled correctly. those are the only ones that should get to else statement. actions without parameters will still have a size > 0 because Success and ErrorCode are stored in the message. keeping this statement empty might be ok.. but then again maybe we shouldn't even perform the check > 0 to begin with? maybe we should just be ok with it and continue as usual? that's what my old code does. i have it #ifdef'd out..
                {
                    //TODOreq: after resolving the above TODOreqs, handle the [error] case where we receive empty MessageData. Another scenario I can envision where there's an empty MessageData is a broadcast without parameters. Oh fuck I already wrote about that just above, but then forgot about it in the two days I haven't been coding :-P

                    //============CASES WHERE MESSAGE_DATA IS ZERO=========
                    /*
                      Broadcasts without a parameter. I can't think of any off the top of my head, but there are most likely use cases. It's analogous to signals in Qt without parameters... which happens quite frequently

                      FALSE: Actions without parameters at a glance sounds like it, but the 'Success' (actions-only) is stored _INSIDE_ the MessageData
                    */
                }
            }
            else //We do not have enough bytesAvailable() to satisfy the header, so we have to remember the header for next read :-P
            {
                if(!readHeaderDuringSomePreviousSlotInvocation)
                {
                    //TODOreq: we might have already stored the header and pulled it back out??? We don't want to append ANOTHER (especially don't want to allocate another: memory leak). The double insert into the hash isn't that big of a concern, since it will just replace the other identical one. But the memory leak is crucial. I have this code in place (readHeaderDuringSomePreviousSlotInvocation) but still want to stare at it + verify it some more
                    //RpcBankServerMessageHeader *storedHeader = new RpcBankServerMessageHeader(); //TODOreq: make sure this gets deleted where appropriate
                    //copyLocalHeaderToMessageHeader(header, storedHeader);
                    m_HashOfMessagesBySocketAwaitingMoreData.insert(secureSocket, messageOnlyIfWeHaveAlreadyReadItsHeaderAndNeedMoreDataForTheMessageItself__OrElseZero);
                }
            }
        } // while(!stream.atEnd())
    } // if(QSslSocket *secureSocket = static_cast<QSslSocket*>(sender()))

#if 0

            //old below

            RpcBankServerMessageHeader header; //stack alloc because there is no dptr. might change this if i want header to hold client response data etc

            //TODOreq: we can't use a member bool 'headerReceived' because this class is shared amongst all the sockets/peers, and each one has a different state of where it is in reading. so I might need a hash just for that fml... or I could change that static_cast above to be my implemented TcpSslClient and add a bool member (wait no i can't it ISN't one!!! we never instantiate that on the server side)
            //TODOreq: we also can't use a member 'currentHeader' for the very same reason as above
            //^^^^^^^^^I'm thinking a solution (to both i guess) would be a hash where the socket is the key. if we don't have the header yet, it is not in the hash (slight optimization to make searching faster). we return a default value of 0 when it isn't there. actually fuck the 'headerReceived' one has no need for a value in that case... but the 'currentHeader' does have a value for the hash: the header itself. of course i could just combine the two and just use a hash. I should only add it when we don't have enough bytesAvailable() to satisfy the header. if we do, there is no reason to remember it. So it can/should be called something like QHash<QSocket*,RpcBankServerMessageHeader*> m_HashOfHeadersAwaitingMoreData; -- the header is a pointer because I've found that it just works better that way. updating in place without having to re-insert..
            //and of course, if we don't have enough data for the header itself... we just break.. and when we come back, our test looking into that hash will fail, which tells us we don't have the header :-P

            TODO LEFT OFF
#if 0 // temporary, just while i code because i intentionally broke it
            if(!got header)
            {
                if(secureSocket->bytesAvailable() >= sizeof(RpcBankServerMessageHeader))
                {
                    stream >> header;
                    got header = true, or something? read above TODOreq
                }
                else
                {
                    break; //return would work too. we just want to get out of the while(!atEnd()) cycle, because it might still be returning true... and we've determined that whatever IS left... isn't enough.
                }
            }

            //if we get here, that means we've gotten our header already
            if(secureSocket->bytesAvailable() >= header.MessageSize) //got header && enough
#endif
            {
                if(header.MessageSize > 0)
                {
                    //can probably steal this code from below :)
                }
                else //TODOreq: make sure that broadcasts without any parameter types (haven't thought of any, but it definitely sounds like a use case that could occur are handled correctly. those are the only ones that should get to else statement. actions without parameters will still have a size > 0 because Success and ErrorCode are stored in the message. keeping this statement empty might be ok.. but then again maybe we shouldn't even perform the check > 0 to begin with? maybe we should just be ok with it and continue as usual? that's what my old code does. i have it #ifdef'd out..
                {


                }
            }
#if 0
            if(header.MessageSize > 0)
            {
#endif
            switch(header.MessageType)
            {
            case RpcBankServerMessageHeader::CreateBankAccountMessageType:
                {
                    CreateBankAccountMessage *createBankAccountMessage = m_CreateBankAccountMessageDispenser->getNewOrRecycled();
                    copyLocalHeaderToMessageHeader(header, createBankAccountMessage);
                    stream >> *createBankAccountMessage;
                    processCreateBankAccountMessage(createBankAccountMessage, SslTcpServer::getClientUniqueId(secureSocket));
                }
            break;
            case RpcBankServerMessageHeader::GetAddFundsKeyMessageType:
                {
                    ServerGetAddFundsKeyMessage *getAddFundsKeyMessage = m_GetAddFundsKeyMessageDispenser->getNewOrRecycled();
                    copyLocalHeaderToMessageHeader(header, getAddFundsKeyMessage);
                    stream >> *getAddFundsKeyMessage;
                    processGetAddFundsKeyMessage(getAddFundsKeyMessage, SslTcpServer::getClientUniqueId(secureSocket));
                }
            break;
            case RpcBankServerMessageHeader::InvalidMessageType:
            default:
                emit d("error: got invalid message type from client");
            break;
            }
            //} if(header.MessageSize > 0)
        }
    }
#endif
}

//These myTransmit and myBroadcast slots can also probably go to a parent interface. I see no Ssl specific code in there...
void SslTcpServerAndBankServerProtocolKnower::myTransmit(IMessage *message, uint uniqueRpcClientId)
{
    //Action Responses + Broadcasts

    //TODOreq -- uniqueRpcClientId is 0 for broadcasts... and perhaps if the action broadcasts it's response?
    //TODOreq: this isn't directly related to this method, but i need to make note of it somewhere. only one of the broadcast receivers should write the broadcast value to the db (assuming a shared-among-broadcast-receivers couchbase cluster)... so maybe that implies i should just broadcast to one client and then let him notify his siblings? or something? idfk

    //TODOreq: handle the case where none of broadcast's servers (clients) are online, nobody to send to wtf

    //TODOreq: it is better to only have to instantiate QDataStream once [per socket methinks?], not every time I want to send something. But gah doesn't that mean that I'd have to look up the QDataStream from a hash lol just like my IRecycleableAndStreamable* storing for the header shit? Is this efficient? Again, the max size of the hash will only ever be CLIENT_COUNT... so like a few hundred or so? Allocating QDataStream over and over vs. a hash lookup. ALSO, maybe I can just use one shared QDataStream? That might have [hidden] performance implictions... but hell, it might not. QDataStream isn't even a QObject type (not sure how that factors in). What I mean to say that it's mostly just data. You'd be changing it's internal QIODevice* that it's operating on. Hell, that sounds a lot cheaper than a hash lookup TODOreq do that instead maybe after some thinking and IDEALLY testing.

    QSslSocket *socket = m_SslTcpServer->getSocketByUniqueId(uniqueRpcClientId);
    QDataStream stream(socket);
    stream << message->Header;
    if(!message->Header.Success)
        stream << message->FailedReasonEnum;
    stream << *message;
}
void SslTcpServerAndBankServerProtocolKnower::myBroadcast(IMessage *message)
{
    //TODOreq: didn't I change the design for this such that we only choose one client to send the broadcast to, and he is responsible for notifying his neighbors (after writing to the couchbase cluster, and then performing a lookup into the couchbase cluster to find which of his neighbors are interested or not (whether or not he is interested is 'slightly' irrelevant)

    //TODOreq: meh I'm not sure if i can do this anymore. not a HUGE deal if I can't, just slightly less efficient: the client that uhh... performs the "GetAddFundsKey" action should be the receiver of the "pendingBalanceDetected" and "confirmedBalanceDetected" broadcasts. I have to somehow associate the Action to the two Broadcasts.... but fuck I gots no clue how to do that atm rofl. It would be nice to somehow make it a part of the functionality of the Rpc Generator (so I can use the same 'smart-broadcast' type thing in other rpc services), but I might settle for hacking in the code on a case by case basis

    //broadcast
    QList<uint> allConnectedClients = m_SslTcpServer->getAllConnectedUniqueIds();
    QListIterator<uint> it(allConnectedClients);
    uint clientId;
    while(it.hasNext())
    {
        clientId = it.next();
        emit d(QString("broadcasting a message to clientId: ") + QString::number(clientId));
        myTransmit(message, clientId);
    }
}
