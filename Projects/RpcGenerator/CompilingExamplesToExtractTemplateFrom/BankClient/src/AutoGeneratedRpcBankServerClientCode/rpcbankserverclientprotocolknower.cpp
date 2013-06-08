#include "rpcbankserverclientprotocolknower.h"

IAcceptRpcBankServerClientActionDeliveries_AND_IEmitActionsAndBroadcastsBothWithMessageAsParamForSignalRelayHack *RpcBankServerClientProtocolKnower::m_SignalRelayHackEmitter = 0;

RpcBankServerClientProtocolKnower::RpcBankServerClientProtocolKnower(QObject *parent) :
    IAcceptRpcBankServerActionDeliveries(parent)
{
    m_PendingBalanceDetectedMessageDispenser = new ClientPendingBalanceDetectedMessageDispenser(this, this); //TODOreq: not 100% sure that the destination is correct here... but I also recall that it might just be ignored/irrelevant in this case so maybe it's fine being inaccurate!?
    m_ConfirmedBalanceDetectedMessageDispenser = new ClientConfirmedBalanceDetectedMessageDispenser(this, this);
}
void RpcBankServerClientProtocolKnower::messageReceived()
{
    //TODOreq: the guts of this method will be MACRO'ified. See the server's equivalent if you forget what that means

    RpcBankServerMessageHeader header;
    *m_MessageReceivedDataStream >> header;
    switch(header.GenericRpcMessageType)
    {
        case RpcBankServerMessageHeader::ActionResponseGenericServer2ClientMessageType:
        {
            switch(header.RpcServiceSpecificMessageType)
            {
                case RpcBankServerMessageHeader::CreateBankAccountRpcBankServerSpecificMessageType:
                {
                    //TODOreq:
                    //remove it from the "action requests that have been sent to server list awaiting response" list
                    //TODOreq: flip the toggle bit or something so when we re-use the message/id on another request, THIS response will get it's "lazy ack" [at basically zero cost <3]. Really we could do it at 3 places and IDK if it matters where it's done. Here, when doneWithMessageIsCalled, or at/just-before dispensing via getNewOrRecycled. All 3 are auto-generated anyways so who gives a fuck. I suppose that since the broadcasts will be using toggle bits too (since they also lazy ack ack) their dependency on it might help us select a suitable place...
                    //TODOreq: use the signal relay emitter or something somehow to notify the business of the action response. Maybe it's just a deliver() call but then I wouldn't have the message as a param. Is it also going to use the signal relay emitter hack? I forget (or perhaps haven't figured it out yet) tbh

                    //Just realized it's going to be hard to code this "handle response (match up with request)" part of the code BEFORE coding the "send request" code. Now... where did I put that shit :-P....

                    CreateBankAccountMessage *createBankAccountMessage = m_CreateBankAccountMessagesPendingResponseFromServer.take(header.MessageId);
                    *m_MessageReceivedDataStream >> *createBankAccountMessage; //The message already has the request portion (members) filled out, and now we're filling in the response portion (members) :)
                    m_SignalRelayHackEmitter->emitCreateBankAccountCompleted(createBankAccountMessage);
                }
                break;
                //Etc for each Action
                case RpcBankServerMessageHeader::InvalidRpcBankServerSpecificMessageType:
                default:
                {
                    //TODOreq: errors and shit
                }
                break;
            }
        }
        break;

        //etc TODOreq: broadcasts and shit will need to go here, but for my first test I'm focusing only on Actions. So I only need to deal with Action Responses (and then the retries after that). After I have Actions and their acks/retries all sorted out, Broadcasts should come easy as they are just a simplification of it and I can/will just copy/paste code for them

    case RpcBankServerMessageHeader::InvalidGenericServer2ClientMessageType:
    default:
        //TODOreq
        break;
    }
}
void RpcBankServerClientProtocolKnower::createBankAccountDelivery()
{
    //TODOreq: later the guts of this method will just be an inline call that works with all action requests, specifying the RpcServiceSpecificMessageType and like the action request list for pending response from server...

    //We get here when business calls ".deliver", which means that they want us to send their Action Request to the server. We have to make note of it's messageId -- and 'this' class also needs a periodic timeout (TODOoptimization: disabled when there is nothing pending response from server) that iterates over the messageIds and then like does stuff to like retry and then retry again and then ultimately commits suicide (just kidding, all it will do is "report to business" of the failure, and business decides what to do from there (or we could SHUTDOWN BY DEFAULT unless the business says not to... sounds like a better default tbh)).

    //The server has two lists. One for keeping track of messages in business, and one for keeping track of one's that have been sent back and haven't yet been ack'd. On the client we only need one list: messages sent to server awaiting response
    //Starting to think maybe Action Responses need to use relay hack emitter as well? But IDFK tbh. LoL. It makes sense that it isn't the "exact opposite of server" because broadcasts are 1-way and actions are 2-way. The reason we don't use relay emitter hack on server is _BECAUSE_ broadcasts are one way. Man I really wish I had a formal design of this beast of an app... and I especially wish that's all I needed ;-) (I know that to be true, but it is not true 'yet' because I have not [yet] made it so (I've analyze other compiling UML offerings and didn't like what I saw)).
    IActionMessage *createBankAccountMessage = static_cast<IActionMessage*>(sender());
    if(createBankAccount)
    {
        m_CreateBankAccountMessagesPendingResponseFromServer.insert(createBankAccountMessage->Header.MessageId, createBankAccountMessage);
        createBankAccountMessage->Header.GenericRpcMessageType = RpcBankServerMessageHeader::ActionRequestGenericClient2ServerMessageType;
        createBankAccountMessage->Header.RpcServiceSpecificMessageType = RpcBankServerMessageHeader::CreateBankAccountRpcBankServerSpecificMessageType;
        //TODOreq: where is MessageId set? Does the dispenser logic handle that? I am only pretty sure at this point that client will be setting it, but still dunno where lol. Here *might* be the right spot but I think the dispenser is actually... idfk
        streamToByteArrayAndTransmitToServer(createBankAccountMessage);
        //TODOreq: So now that it's streamed to the server, we need to set up a timeout to check on it once (and twice should the first one not help us out at all). We also need to receive the response and then remove/cancel the message's timeout functionality. There might even be more shit we have to account for and I should check that "giant list of enums" to make sure I'm not missing anything. OH I need to handle dupe responses for race conditions etc etc. I think I have a text document in the design folder that mentions all the various ways I can error out (but I think that giant enum also covers all/most of them already?)
    }
    else
    {
        qWarning("null message delivered"); //I can see this happening if the helper and business are on the same thread. The AutoConnection will become direct and therefore sender is invalid!
    }
}
void RpcBankServerClientProtocolKnower::getAddFundsKeyDelivery()
{
}
