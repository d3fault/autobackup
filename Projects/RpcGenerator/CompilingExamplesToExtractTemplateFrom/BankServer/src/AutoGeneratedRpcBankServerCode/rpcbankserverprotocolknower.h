#ifndef RPCBANKSERVERPROTOCOLKNOWER_H
#define RPCBANKSERVERPROTOCOLKNOWER_H

#include <QHash>

#include "iprotocolknower.h"
#include "abstractclientconnection.h"

#include "../../RpcBankServerAndClientShared/rpcbankserverheader.h"

#include "iemitrpcbankserveractionrequestsignalswithmessageasparamandiacceptactiondeliveries.h"
#include "iprotocolknower.h"

#include "MessagesAndDispensers/Dispensers/Actions/servercreatebankaccountmessagedispenser.h"
#include "MessagesAndDispensers/Dispensers/Actions/servergetaddfundskeymessagedispenser.h"
//etc for each Action

//signal relay hack
#include "iacceptrpcbankserverbroadcastdeliveries_and_iemitactionsforsignalrelayhack.h"

//TODOoptimization: a shit ton of the methods in this header/class can/should go in IProtocolKnower .. but that's an optimization for later, who cares for now when there's much more important and confusing things to be done

class RpcBankServerProtocolKnower : public IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptActionDeliveries, public IProtocolKnower
{
    Q_OBJECT
public:
    static void setSignalRelayHackEmitter(IAcceptRpcBankServerBroadcastDeliveries_AND_IEmitActionsForSignalRelayHack *signalRelayHackEmitter) { m_SignalRelayHackEmitter = signalRelayHackEmitter; }
    explicit RpcBankServerProtocolKnower(QObject *parent);
    virtual void messageReceived(); //QDataStream *messageDataStream);
    virtual void queueActionResponsesHasBeenEnabledSoBuildLists();
    inline void streamToByteArrayAndTransmitToClient(IMessage *message)
    {
        //stream to byte array
        resetTransmitMessage();

        //m_AbstractClientConnection->streamDoneHelloingFromServerIntoMessageAboutToBeTransmitted(&m_TransmitMessageByteArray); //pretty sure i can now conclude that "streaming into" a message does not clear/overwrite it. it wouldn't make any sense if it did... how could i stream multiple items into a single qba if that was the case. OR PERHAPS the question was more relating to streaming out of (reading) a QDS and into a singular.... POD (QString counts). does that POD first need resetting? TODOreq (ignore first part of comment lol, it's already handled)

        streamDoneHelloingIntoMessageAboutToBeTransmitted();

        //after writing the following line, i just realize that i need to use the appropriate enums in places like... 'here it is again bitch' TODOreq... and probably lots of other places too! that "enum for-each 'find usages'" op should solve this implicitly as well...
        m_TransmitMessageDataStream << message->Header; //TODOreq: actions pretty much just send the same header back (after updating the GenericRpcType of course), but Broadcasts need to fill out the entire header manually
        m_TransmitMessageDataStream << *message;

        //transmit
        m_AbstractConnection->transmitMessage(&m_TransmitMessageByteArray);
    }
    inline void streamToByteArrayAndTransmitToClient_ButDontStreamTheMessageBecauseWeDontNeedItInThisSpecialCase(IMessage *message)
    {
        resetTransmitMessage();

        streamDoneHelloingIntoMessageAboutToBeTransmitted();

        m_TransmitMessageDataStream << message->Header;
        //coulda used bools etc to not stream this out depending on when it's needed, but it's an optimization to not have to check a bool that's rarely used for every single message. this method is used far less often than the above
        //m_TransmitMessageDataStream << *message;

        //transmit
        m_AbstractConnection->transmitMessage(&m_TransmitMessageByteArray);
    }
private:
    static IAcceptRpcBankServerBroadcastDeliveries_AND_IEmitActionsForSignalRelayHack *m_SignalRelayHackEmitter;

    //Dispensers
    ServerCreateBankAccountMessageDispenser *m_CreateBankAccountMessageDispenser;
    ServerGetAddFundsKeyMessageDispenser *m_GetAddFundsKeyMessageDispenser;
    //etc for each Action

    //Action Responses that might be re-requested in certain cases and are awaiting acknowledgement
    QHash<quint32 /*MessageId*/, IActionMessage* /*ack-awaiting-ack*/> m_CreateBankAccountMessagesAwaitingLazyResponseAck;
    QHash<quint32 /*MessageId*/, IActionMessage* /*ack-awaiting-ack*/> m_GetAddFundsKeyMessagesAwaitingLazyResponseAck;
    //etc for each Action

    //Actions that are being processed by the business
    QHash<quint32 /*MessageId*/, IActionMessage* /*pending-in-business*/> m_CreateBankAccountMessagesPendingInBusiness;
    QHash<quint32 /*MessageId*/, IActionMessage* /*pending-in-business*/> m_GetAddFundsKeyMessagesPendingInBusiness;
    //etc for each Action

    //List of Actions that were in business at the moment we enabled queue mode (which happens both when a dupe cookie is enabled (queue mode is enabled for a couple of milliseconds at most), and also when any socket error signal is received (queue mode stays enabled indefinitely until a new connection with same cookie is detected)
    QList<quint32> m_CreateBankAccountMessagesPendingInBusinessWhenEnablingQueueActionResponsesMode;
    QList<quint32> m_GetAddFundsKeyMessagesPendingInBusinessWhenEnablingQueueActionResponsesMode;
    //etc for each action

    //TODOreq (wtf is this old as balls? pretty sure it's done): shit our messages get so easily sent back to our rpc bank server clients helper (the specific connection really) that I don't know where to do pendingInBusiness.remove!!! perhaps in the deliver() logic just before the signal is emitted??? Fuck me this really screws with the design, never thought of that before now. I'll probably need a hack to accomodate it. HAHAHAHA rofl oh just realized I am the same person who receives those signals... so it should be easy once I get that coded :-P. Still worth the TODOreq because I simply didn't have that much coded in the part I'm taking the code from (ported/upgraded/optimized/redesigned)

    //TODOoptimization: figure out if, since it's inline, that they need to be pointers in order to not increase the reference count. since it's inline it might be faster (no deref'ing) to pass by value. idk how inline handles implicitly shared
    inline bool processNewRpcBankServerActionMessage_AND_RecordMessageInBusinessAndReturnTrueIfNeedEmitToBusiness(IActionMessage *actionMessage, QHash<quint32, IActionMessage*> *actionSpecificAckList, QHash<quint32, IActionMessage*> *actionSpecificPendingInBusinessList)
    {
        //We know this is a new Action Request, but we don't know whether it's messageId has been used before or not. If it has, then we do the final (lazy-as-fuck) ack for the Action Request that previously used that messageId
        quint16 messageId = actionMessage->Header.MessageId;
        IActionMessage *messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero = actionSpecificAckList->value(messageId, 0);

#if 0 //Compacting this if/else, because this method appears to always 'insert in business' and 'return true' (only as of this recent design refactor of course)
        if(messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero)
        {
            //This is a lazy ack ack for a, now successful, previous Action Request

            //We used to have toggleBit for detecting this (whether it's new or old (if old (toggleBit didn't change), it was implied that it was a retry)), but that design has changed and now this method is ONLY called on new Action Requests

            //remove from ackAck list and 'done' the old Action Request
            actionSpecificAckList->remove(messageId);
            messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero->doneWithMessage();

            //TODOmessageErrors: this is our typical non-error case, but for every time after the first time we use that MessageId

            //insert into business pending and 'return true' (indicating to invoke business) the new Action Request
            actionSpecificPendingInBusinessList->insert(messageId, actionMessage);
            return true;
        }
        else
        {
            //First time seeing this messageId? I'm tempted to write that it might be in business pending, however if that were the case then we wouldn't be in this method, we'd be in the retry1/retry2 method! Client only uses the enum for a new Action Request for the same messageId ONLY AFTER he has first received a response to the first Action Request. Yes this does mean our server is dependent on correct client code, but they are pretty intertwined to begin with so go fuck yourself

            ////TODOmessageErrors: this is our typical non-error case, but only for the first time seeing this MessageId

            //insert into business pending and 'return true' (indicating to invoke business) the new Action Request
            actionSpecificPendingInBusinessList->insert(messageId, actionMessage);
            return true;
        }
#endif
        //Here is the above ifdef'd out if/else compacted

        if(messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero)
        {
            //This is the lazy ack ack for a, now successful, previous Action Request

            //We used to have toggleBit for detecting this (whether it's new or old (if old (toggleBit didn't change), it was implied that it was a retry)), but that design has changed and now this method is ONLY called on new Action Requests

            //remove from ackAck list and 'done' the old Action Request
            actionSpecificAckList->remove(messageId);
            messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero->doneWithMessage();

            //TODOmessageErrors: this is our typical non-error case, but for every time after the first time we use that MessageId
        }
        //else
        //{
            //First time seeing this messageId? I'm tempted to write that it might be in business pending, however if that were the case then we wouldn't be in this method, we'd be in the retry1/retry2 method! Client only uses the enum for a new Action Request for the same messageId ONLY AFTER he has first received a response to the first Action Request. Yes this does mean our server is dependent on correct client code, but they are pretty intertwined to begin with so go fuck yourself. TODOreq or perhaps optional/optimization: if client does send an Action Request with the same MessageId with the "New Action Request" enum set before receiving a response to the old one, and the old one is still in business (if it was in lazy ack list then it would still cause problems, just different kinds. we don't know that they actually received it. but i mean faulty client is faulty at this point so...), this method is now not smart enough to handle that properly. It used to be because we used to always check in business pending, but now we don't and have a separate method for retries. If the client is faulty, he can make us insert two messages in business pending with the same messageId, and of course that will fuck shit up royally when we try to remove/etc (our remove code can/should verify that only one was removed, so we can at least detect this case (how can you 'recover' from a faulty client ANYWAYS???))
            ////TODOmessageErrors: this is our typical non-error case, but only for the first time seeing this MessageId
        //}

        //insert into business pending and 'return true' (indicating to invoke business) the new Action Request
        actionSpecificPendingInBusinessList->insert(messageId, actionMessage);

        return true;

        //this return true now seems worthless since it is always used. but maybe blacklist programming detecting unforseen error cases in the future will change this method to 'return false' somewhere? that and the fact that this method will be called via a macro that is swappable with other methods with matching signatures (and, the 'retry' method, for example... STILL DOES need to be able to return true/false based on whether or not business needs to be invoked)... means we need to keep the bool return variable whether we use it or not (perhaps i'm over-optimizing again with dat macro shit)

        //TODOreq: Below is old design but very important code! As of this moment forward, this method is only for Action Requests, not retries! Still, I should analyze it and it's 'req's thoroughly to make sure the same cases are accounted for AGAIN in the new design. I'm using plusses and minuses to mark each line as 'has been accomodated' (plus) vs. 'not yet' (minus)
#if 0
        //TODOreq: even though this function doesn't use my various bits (retry1, retryBecauseConnectionMerge, ahh isn't this sameshit as messageType below?) in their actual names, I do deem it usable for now. I think once I get to the client implementation portion of this it will become more evident if I actually need to change the designs a bit
        //TODOreq: i still think i need to add a messageType to differentiate between actionRequests/actionResponses/broadcasts/cleanDisconnectRequest/actionRequestRetry1(expects response). It needs to go over the network ofc. We could share the field with the hello code, but really don't have to since our protocol is dynamic as fuck :). Still it might be wise to do so?

        //our first check is for the lazy ack ack, as it is the most likely. we COULD check that it's in business still (I guess this depends on if the retry bit is set?), but since that's less likely we'll check it next

        //first just see if it's there. it will most likely be there. if it isn't, it's very likely in business pending...
        +IActionMessage *messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero = actionSpecificAckList->value(actionMessage->Header.MessageId, 0);

        +if(messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero)
        +{
            //so it's in the ack awaiting ack list, now we need to determine if we're lazy ack'ing an old message or if we want to re-send the response. this is why we lazy ack to begin with...

            //edit: for example right here the retry bit should be specified because what if the first request just didn't make it?

            +if(messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero->toggleBit() == actionMessage->toggleBit())
            +{
                //if they are the same, retryBit is implied and we just return the one that's already existing.
                //TODOreq: wtf do I do with the new one? just recycle it right here right now? I think so but am unsure...

                //it might seem inefficient that we have two copies of the message at this point... BUT WE (client) DON'T KNOW THAT THE FIRST ONE ARRIVED MOTHERFUCKER... so we did have to send the whole request again. HOWEVER, the server has determined it's worthless.
                //TODOmessageErrors: this is our response retry case

                +actionMessage->doneWithMessage(); //doesn't have response filled out anyways~ piss off

                //we re-send the old one, but still don't remove it from the ack pending ack. what if this one gets lost too? actually TODOreq: since this would be our second attempt at a response, there is definitely not going to be a third SO it might be ok to say doneWithMessage at this point??????? it would mean that the next time we see the MessageId, it will be treated as though it was the first~ <---- all of this is only true if we only retry exactly once. err, try exactly twice.

                //re-transmit
                //messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero->deliver(); //TODOreq (possibly done already): the handler of deliver() expects only to see messages coming from business. It removes them from the pending in business. It shouldn't matter that we remove our messageId/message from a hash that we aren't in, but it is a worthwhile optimization to NOT do so. It also re-appends us to our ack-awaiting-ack list... so maybe I should just call myTransmit here directly instead???? just don't have that clientId, but I think I'm going to be adding that as a member of IActionMessage anyways???? (it does not apply to broadcast I don't think? (aside from them special client-prioritized broadcasts that I'm not even sure I can use yet lmfao, which'd probably include a db lookup of some sort to figure out who that client is anyways? really no fucking clue))
                +if(!m_AbstractClientConnection->queueActionResponsesBecauseTheyMightBeReRequestedInNewConnection())
                +{
                    +streamToByteArrayAndTransmitToClient(messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero);
                +}
                //else: we got a retry from a connection that has since failed or is about to merge, so meh don't bother trying to send it back because the new connection will be the one to do that
            +}
            +else
            +{
                //they are not the same, so we know we're on the next message. We can now recycle the old one: we know we don't need it anymore. The ack has officially been ack'd :-D
                +actionSpecificAckList->remove(messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero->Header.MessageId);
                +messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero->doneWithMessage();


                //TODOreq: now that that's taken care of, we begin processing the new message

                //TODOmessageErrors: this is our typical non-error case, but for every time after the first time we use that MessageId

                //dispatchCreateBankAccountMessageToBusiness(createBankAccountMessage);
                +actionSpecificPendingInBusinessList->insert(actionMessage->Header.MessageId, actionMessage);
                +return true;
            +}
        }
        +else
        +{
            //not in ack lazy awaiting ack list, so it might be in business pending... or TODOreq: it might be our first time seeing it and in neither!!!

            //hack: the name of the pointer doesn't match our usage, but no point in allocating another one lol. is 'messageOnlyIfPendingInBusiness' from now on
            +messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero = actionSpecificPendingInBusinessList->value(actionMessage->Header.MessageId, 0);

            //if messageOnlyIfPendingInBusiness really
            +if(messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero)
            +{
                //as of right now, both of the business pending checks come here. despite coming at different times, all we have to do is tell the truth. we are still in business! I guess we don't need the 3 different requests to indicate which status they are, simply sending the same message with the toggle bit not changing three times in a row will indicate it!
                //TODOreq: it is worth noting that the first (initial), second (retry1/status1), and third (retry2,status2) messages that a client sends to the server are the exact same, yet only the second (and POSSIBLY third, though it can be an optimization to know from the second one whether or not the whole message needs to be resent) warrants a response ("still in business"). The response to the first one is the answer/response itself [assuming no errors and normal conditions]! The 3rd message is the same as the second and also warrants the same response. IT IS UP TO THE CLIENT to be able to differentiate between any responses in retryBecauseRequestNetworkTimeout, stillInBusiness[1], retryBecauseBusinessTimedOut, stillInBusiness[2]
                //TODOreq: there is an amount of custom logic that must be performed to be able to deduce on the client what messages are what. if we receive our answer/response just after we dispatch either of our retry* (also 2->3rd might be even more complicated idfk), then we need to know we're definitely going to get the same answer/response and this time with the responseBit set (i might have meant to write "responseRETRYbit" right there, idfk)

#if 0
    ********TODOreq : say "still in business"********** -- twice too
                        TO/*DO LEFT */off
                        //and i think i need a message type bit. i need it for clean disconnects, but also for an alleged "still in business" message. i think i'll end up doing a request type, requestRetry1(status1) type, requestRetry2(status2) type, requestResponse type, broadcast type, disconnectPlz type, stillInBusiness type (status1 response), stillInBusinessAgainWtfLoLTimeToCrashProllyButThatsNotUpToMe(status2 response), etc as many as needed
                    i am getting so close i can taste it -- my tongue now tastes/feels like sandpaper, but that might be because i ate 1.9 lbs of sour patch kids over the course of 3-4 days last week :-/. also drawing that "THEN/NOW" comic put into perspective just how much work is ahead of me FML
#endif
                        //messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero->setErrorCode(); or something similar, but error codes are specific to the user's protocol so DEFINITELY NOT THAT. I think I need to delete my old/outdated "Header" class but not sure how to replace it. My brain fucking hurts.

                        //TODO LEFT OFF REALLY IMPORTANT THIS IS THE LATEST (for now rofl) REQUEST PROTOCOL WHEN ALL SAID AND DONE -- this attempt was written after the helloer/qbapeeker/magic etc rewrite. I'm pretty sure all other attempts were written before those and are therefore outdated
                        //this is also the protocol not showing 'magic', which comes first of course
                        //Yet another attempt at finalizing the request protocol on the byte level:

                        //PeekedAndReadInFullAlreadyQByteArray[HelloStatus(Done normally, but we have to re-stream this over and over so that we can eventually get a "DisconnectRequested" (possibly just "Goodbye" because the disconnect REQUEST is at the generic rpc layer, not the hello(/goodbye) layer) status. Other statuses are InitialHello etc),RpcGeneratorAbstractMessageBody[quint32 RpcServiceId, quint32 RpcServiceMessageId(can-repeat-in-other-rpc-service-ids-but-never-within-one), quint32/enum whatThisMessageIs[examples: InitialRequest(fuck toggle bit i guess?),RequestRetry1Status1,RequestRetry2Status2WithMessage,RequestRetry2Status2WithoutMessage], quint32 RpcServiceIdSpecificMessageType (stored as a quint32, but utilized by inheriters to assign special enum types indicating the rpc service methods made available), /*QByteArray*/ theActualFuckingMessageElements (NOT going to be in it's own QBA, will just follow directly after the other stuff since we know the protocol)]]

                        //^^Only when the hello-level status is "Done" do we have an RpcGeneratorAbstractMessageBody following it. During the hello phase (and various stages therein) we usually just follow the status with the cookie (if we have one [at that point or otherwise]) or something

                        //RpcGeneratorAbstractMessageBody could have been another QBA, but right now it is merely "implicitly separated" (comes directly after) from the HelloStatus. This is to save us a quint32 QBAsize on each message... but I'm going to leave it in the above protocol definition to let us know that the two are LOGICALLY separated

                        //Do I really need RpcServiceId? Wouldn't it be alright to let different rpc services (even within the same process) simply use different ports in order to specify different rpc services? I don't think it matters either way. I keep flip-flopping mentally on whether such a scenario would utilize "multiple rpc servers" or "one rpc server that communicates with multiple businesses (based on RpcServiceId)". Obviously for 2 separate processes they would need their own rpc server (and therefore port), so is that the case that I should design.... to? If so, I don't need RpcServiceId!!! TODOreq
                        //^^In a way it's a matter of memory optimization vs. protocol optimization (duplicate instantions of MultiServerAbstraction that are basically the same but bound to different ports). It's probably smarter to focus on the protocol optimization because bandwidth is way more expensive than memory..... I'm just a little uncertain about the long term and chaotic implications it will have on [??????????everything-that-uses-this-shit??????????]






                    //TO DOnereq: say "still in business" motherfucker. the above protocol definition for requests might be my only real accomplishment for the day. still need to implement it and then come back here and USE it... but eh i'm staring at it extra long to make sure i didn't miss anything. fuck yea deftones white pony









                        //TODOreq (2-layer definitely btw!): Does the hello level handle the "disconnect requested received so ignoring your request", or does the rpc level handle it? If the rpc level handles it like I'm thinking, we need a custom response type TODOreq that says something like "WontRespondBecauseDisconnectRequestedUseAnotherClient" (mindfuck: does that WontRespond message get ack'd?). Still it does seem like the hello'er level would be involved. Perhaps it also has a disconnect message that is only used AFTER we are sure we are ready to perform a clean disconnect? We could be cute and call it Goodbye ;-P

                    //TODOreq: the difference between the first network timeout and the second business timeout (both detected on client via same way) needs to be greater than the average response time for that connection. We need to give ample time for our RequestRetry1Status1 to be responded to before dispatching the RequestRetry2Status2 (this does relate to whether or not RequestRetry2 will contain the full request again, but also makes common sense even without that optimization)

                //TO DONEreq (decided i need to have two status checks and the first one is a retry but the second can know whether or not to send the message again (a small optimization TODOoptimization)): decided that rpc generator will first report the status of the message, and then later ----- actually fuck it that makes unnecessary complications. I should just _ONLY_ check once: the maximum timeout allowed. But the decision remains: I will simply "report" the stuck-in-business error and let the controller-of handle it. There is no need to ask until -----
                //ACTUALLY WAIT THERE IS A REASON
                //If I _DON'T_ have a 2-phase "eh he's still in pending gimmeh more time" method, then TimeUntilResponseLostCheckingIsPerformed has to EQUAL MaximumTimeAllowedInBusiness. Is it ok to make these equal (because we'd only have one check to handle both)?
                //^^^^^It doesn't seem TOOOOO harmful, but I'd like to think that I'm giving my business an ample amount of time (some slow down is acceptable versus shutting down everything after all) while still having snappy "response lost here it is again" recovery. That tells me they shouldn't equal each other.



                //psbly old below, idfk:

                //TODOreq: still in business. should we respond with a 'give us more time plox because if this doens't work itself out, shit is FUCKED and everything should shut down'? TODOreq: so does that imply that the client sends us yet another (3rd!!!) try? And if THAT ONE doesn't work, then we're really really fucked? Or should I ignore it (store it?) and set up a timer on the business and then report fucked? I like the idea of staying in communication with the client at this point, even though we don't have much to say except "it's the business, not us". :-/. This is probably one of the hardest use cases to decide what to do... but I guess I'd say opt towards giving him more time before shutting the fuck down... or even trying another server
                //I'm going to be so ridiculously happy if I never ever encounter this code path. I mean sure during development you are bound to fuck up... but your business MUST respond in production... else we shut everything the fuck down (TODOreq: this is a good as fuck reason to keep applications... servers... whatever.... appIds.... AWAY FROM EACH OTHER. so if one service goes down it doesn't bring down everything else that is working perfectly fine. I guess I could just do it on a per-appId basis... if I really want to keep everything "one-giant-binary-to-rule-them-all" roflrofl good luck stealing my source if the source never touches the server, only binaries do. even though that has nothing to do with me doing them in separate binaries lol. but yea I mean I think you get more efficiency and therefore scalability and therefore profits by having all your services on ONE GIANT DHT. I don't even want to think about 'providing low latency access to overseas countries' though. I guess those would just be copy/pasta different servers/clusters altogether? It'd work for services like email etc where the user has their own account... but for shit like ABC AdDoubles, we need worldwide coordination so it's MUCH easier to just have a giant ass single dht like I said. The latency has to be dealth with either way. Either inter-cluster realtime synchronization (NO-FUCKING-THANKS (not to be confused with cross-cluster backing up, which yea i'll have running 24/7 both ways (or 3 ways etc), but they are DELAYED, not realtime!!! (also: still dunno how rolling back or whatever would work with a delayed/lagged copy. but shit i know one thing: it's better to have a few seconds/minutes outdated copy than NO COPY AT ALL (in the case of fire/explosion/etc lmfao)))) or have the user do it (yes))
            -}
            +else
            +{
                ////TODOmessageErrors: non-error case, first time seeing this MessageId
                //dispatchCreateBankAccountMessageToBusiness(createBankAccountMessage);
                +actionSpecificPendingInBusinessList->insert(actionMessage->Header.MessageId, actionMessage);
                +return true;
            +}
        }
        return false;


        //TODOreq: somewhere in the above I need to actually toggle the bit. Or wait no, client does that right???? still worth noting.


        //check in business still if retry bit is set? actually maybe i don't even need the retry bit anymore if I'm using a toggling-per-messageId-use design. if I get the same messageId without the bit toggled, then retry is IMPLIED????


        //m_UniqueRpcClientIdsByPendingCreateBankAccountMessagePointer.insert(createBankAccountMessage, uniqueRpcClientId);

        //I think this is old, idfk:
        //TODOreq (no idea what the fuck i'm talking about so yes this is probably old): perhaps check the list already by value to see if we're already working on the same request?
        //if we are, it still might fail and then we should process this one. so i need to atomically access a list in that case, a list of "pendingCollissionsAwaitingFinalizatioin"
        //the list is mutex-grabbed+cleared AFTER we transmit the message back to
        //actually this idea sucks
        //maybe we can instead connect the clearing of the list to the chosen/conflicting/got-their-first recycle signal after it's finished? so no additional processing needs to be done

        //emit createBankAccount(createBankAccountMessage);
#endif
    }
    inline bool processRetryRpcBankServerActionMessage_AND_ReturnTrueIfNeedEmitToBusiness(IActionMessage *actionMessage, QHash<quint32, IActionMessage*> *actionSpecificAckList, QHash<quint32, IActionMessage*> *actionSpecificPendingInBusinessList)
    {
        //TO DOnereq: we still need to use toggleBit because there is a false positive where an Action completes successfully, but then when the messageId is used again on the next message, the initial request fails on the network. The client then sends a retry for that messageId, but the server can't tell if the client wants the old Action Request/Response, or if it's a new message that got lost on the network. For some reason I thought my design refactor obsoleted toggleBit, but I guess not as this problem is the same one that made me think of it to begin with!
        //^^^^^hahahaha now I see why I was using toggleBit as an implicit retry 'bit' (or enum now). It IS... but however does still complicate the design to the point where I don't know how to do responseRetries and yada yada. I might re-optimize this later once I get it up and running TODOoptimization... but oh zeus (had: god) I don't even know if I'll ever see that day


        //first just see if it's there. it will most likely be there. if it isn't, it's very likely in business pending. if not there, then we lost the original request on the network and this is our first time seeing the messageId. we may have also lost the original request on the network and have seen the messageId before, in which case there's a false positive that toggleBit is used to solve (to differentiate old and new)
        quint16 messageId = actionMessage->Header.MessageId;
        IActionMessage *messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero = actionSpecificAckList->value(messageId, 0);
        if(messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero)
        {
            //Since it's in the ack awaiting ack list and we're in the retry method, we want to re-send the response. this is why we lazy ack to begin with... but first we check the toggleBit to make sure that we don't accidentally send the response to a previous message. TODOreq (pretty much done except for status2 implications, so leaving here): remember, this could be our 'entry point' to a new Action Request (if the initial request got lost on the network (or hell, if the retry1 also got lost on the network (assuming status2 also uses this method, but idfk yet)))

            //edit: for example right here the retry bit should be specified because what if the first request just didn't make it?
            if(messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero->toggleBit() == actionMessage->toggleBit())
            {
                //if they are the same we just return the one that's already existing.

                //TODOmessageErrors: this is our response retry case

                actionMessage->doneWithMessage(); //doesn't have response filled out anyways~ piss off

                //old: (??)
                //we re-send the old one, but still don't remove it from the ack pending ack. what if this one gets lost too? actually TODOreq: since this would be our second attempt at a response, there is definitely not going to be a third SO it might be ok to say doneWithMessage at this point??????? it would mean that the next time we see the MessageId, it will be treated as though it was the first~ <---- all of this is only true if we only retry exactly once. err, try exactly twice.

                //re-transmit
                //messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero->deliver(); //TO DOnereq: the handler of deliver() expects only to see messages coming from business. It removes them from the pending in business. It shouldn't matter that we remove our messageId/message from a hash that we aren't in, but it is a worthwhile optimization to NOT do so. It also re-appends us to our ack-awaiting-ack list... so maybe I should just call myTransmit here directly instead???? just don't have that clientId, but I think I'm going to be adding that as a member of IActionMessage anyways???? (it does not apply to broadcast I don't think? (aside from them special client-prioritized broadcasts that I'm not even sure I can use yet lmfao, which'd probably include a db lookup of some sort to figure out who that client is anyways? really no fucking clue))
                if(!static_cast<AbstractClientConnection*>(m_AbstractConnection)->queueActionResponsesBecauseTheyMightBeReRequestedInNewConnection())
                {
                    messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero->Header.GenericRpcMessageType = RpcBankServerMessageHeader::ActionResponseHereItIsAgainGenericServer2ClientMessageType;
                    streamToByteArrayAndTransmitToClient(messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero);
                }
                //else: we got a retry from a connection that has since failed or is about to merge, so meh don't bother trying to send it back because the new connection will be the one to do that

               return false; //the response simply didn't make it back, so it would be erroneous to re-process the request
            }
            else
            {
                //tl;dr: initial request failed over network, request retry made it, message id being re-used
                //(sdfouwer08324083): if we get here, it means that a re-used messageId's associated _initial_ Action Request failed over the network. Getting here means that the received message is a brand new Action Request (the request *retry* of it) so it should be processed in business. It also means that we need to ack the previous Action using this messageId (we don't need to check the toggleBit again, it was checked to get us into this else statement!)? Some of this code should probably be shared with the processNew* class. Perhaps they both just call another inline method.

                //TODOoptional: could consolidate these two lines into an "ack" method, but they seem simple enough. if however we start checking that remove returns exactly 1 as it _SHOULD_, then we should probably consolidate them to simplify error cases etc
                actionSpecificAckList->remove(messageId);
                messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero->doneWithMessage();

                //ditto as optional above, but for a 'addToBusinessPending' method. probably less necessary as no error cases (???)
                actionSpecificPendingInBusinessList->insert(messageId, actionMessage);

                return true;
            }
        }
        else
        {
            //not in ack lazy awaiting ack list, so it might be in business pending... or TODOreq (pretty sure it's done but i don't trust my binary brain atm because of that dangling 'else' i cant remember): it might be our first time seeing it and in neither!!! That case is similar to the above else (comment with string 'sdfouwer08324083'), except that it's our first time seeing that messageId (perhaps the two can be consolidated? idfk)

            //hack: the name of the pointer doesn't match our usage, but no point in allocating another one lol. is 'messageOnlyIfPendingInBusiness' from now on
            messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero = actionSpecificPendingInBusinessList->value(messageId, 0);

            //if messageOnlyIfPendingInBusiness really
            if(messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero)
            {
                //so we don't really want to return the ASSOCIATED message, but we want to create a message that simply says "hey bitch, the message with messageId is still in business". Where the fuck do I get/allocate/etc such a message? Do I want to ack it? I'm thinking not since the client is already going to be timing out for retry2status2. And once it sends THAT and times out for THAT, then it simply cancels/errors out the request. So acking is err... 'implicit'?? Still dunno where to get it etc. Should I just stack create it and not give a fuck :)? Suddenly I want to write a whitepaper on "deciding when to recycle objects (common cases, such as messages) vs. create them on the fly (uncommon cases, such as 'retry reports')" xD. That one quote pretty much sums it up though. I _COULD_ recycle the retry reports (retryResponses in a sense, but bah that is getting ambiguious as fuck (and ambiguity is the downfall of any programmer (fuck C-style function naming)). I would say that what should really be called a retryResponse would be when a message in the ack list is re-sent to the client without invoking business... but clearly both have ambiguity toward each other so should be overly specific!). Still dunno what the specific type that I'm going to allocate will be. It KIND OF doesn't matter until I remember that the client is very likely going to organize it's messages based on their rpc service specific action type as an optimization =o. Fuck.
                //Especially considering my message delivery system involves complex parenting and 'rigging' etc, I need to now decide whether to create dispensers (next to the already existing dispensers)... or if I should just make a custom/hacked-together message type. Hmm here's another idea: I could use the "existing" message (TO DOnereq: thread safety is a must and if it's in business pending, there are certainly risks!) but use it carefully and set the enum as "status report", and then customize the QDS operators (again, to BE thread safe. by not touching them i am being thread safe heh) so that the "message values" aren't streamed (they aren't ready yet anyways). This might be expensive though because now my streaming logic has an extra bool check. Matter? IDFK doubt it but... maybe? TO/* DO LEFT */OFF AND THE REST OF THIS METHOD SINCE I THINK THERE ARE MORE ELSE STATEMENTS TO COME

                //mfw I thought up the next day the fact that we already have a [disposable/temporary] message to use for the response: the request retry! TO DOnereq I should probably use a custom method for the custom streaming, don't add more code paths (and therefore slowdown) to the typical non-retry case (but if you need to for simplicity's sake, go for it)

                //TODOreq: there was some special 'if' case that I thought up around here (or it may have been an else to an already existing if, idfk) when my comp was off and now, weeks later, I can't remember what the fuck it was. GOD DAMNIT I SHOULDN'T TAKE SUCH HUGE BREAKS WHEN WORKING ON SUCH IMPORTANT CODE. On that note, my grandpa has died since and I have decided to move out: my dad is a fucking noisy whiny child and it's distracting as fuck: both when I sleep and when I code (I don't code when he's home, ever). Hopefully going to move in with my quiet grandparents. The fact that they'll be home most of the time is irrelevant. I just need to explain to them that I need to be left alone: my grandma will try to use me as her handyman (in the same way she used my [other] grandpa back when he was younger). She'll (they'll) also think I'm cold/detached and lazy as fuck... so the arrangement might not work. I need to explain it well and we need to agree on a list of "chores". What the fuck does this have to do with the req? Get a blog man this is code haha dipshit

                if(!static_cast<AbstractClientConnection*>(m_AbstractConnection)->queueActionResponsesBecauseTheyMightBeReRequestedInNewConnection())
                {
                    //set 'still in business'
                    actionMessage->Header.GenericRpcMessageType = RpcBankServerMessageHeader::ActionResponseToRequestRetry1StillInBusiness1GenericServer2ClientMessageType;

                    //stream it back directly. no need for acks etc. i should maybe use/make a different method than the typical streamToByteArrayAndTransmitToClient(), because that one is set up to stream all the parameters of the Action... and really all i need to stream back is like the message id (header) and the fact that it's still in business (bitch)
                    streamToByteArrayAndTransmitToClient_ButDontStreamTheMessageBecauseWeDontNeedItInThisSpecialCase(actionMessage);

                    //since we were hackily using the message that was read (the request retry) in order to send the response (still in business), that particular message isn't really relevant since we've confirmed that the actual message is still in business (and it wouldn't be thread safe to use THAT one (the real one))
                    actionMessage->doneWithMessage();
                }
                return false; //the message is still in business, so we return false so that another one isn't dispatched to business lol
            }
            else
            {
                //tl;dr: initial request failed over network, request retry made it, first time seeing message id
                //if it's not in the pending ack list and it's not in pending business, then we've never seen this message ID before otherwise it would be in one of the two lists. The fact that we got a retry for a new message Id is similar to comment with string 'sdfouwer08324083', except that we don't need to ack previous message with the same messageId. It basically means that the request was lost over the network. The retry made it (otherwise we wouldn't be here/now) but the initial request did not

                //I have no idea if this is the else statement that the TODOreq above is describing... or if it's another one. Maybe it is, maybe it isn't.

                actionSpecificPendingInBusinessList->insert(messageId, actionMessage);

                return true;
            }
        }
    }

    //Action Responses -- sending them back to the client
    inline void removeFromPendingInBusiness_AND_addToAwaitingAck_And_StreamToByteArray_And_Transmit(IActionMessage *message, QHash<quint32, IActionMessage*> *actionSpecificAckList, QHash<quint32, IActionMessage*> *actionSpecificPendingInBusinessList, QList<quint32> *actionSpecificBusinessPendingDuringMergeList)
    {
        //TODOreq (done methinks (but maybe enum shit needs to be set in this method? idfk)): isn't this method missing a header send? i think i've refactored header shit so many times that it now doesn't get streamed when sending back to client. i need to figure out this in order to finish up my 'request is still in business' response (which is header ONLY methinks?). it is also worth noting that the header needs to come after both magic and the "hello" header ("DoneHelloing" typically)... so i might have to butcher my code to make that work

        //remove it from pending business
        if(actionSpecificPendingInBusinessList->remove(message->Header.MessageId) != 1)
        {
            //TODOreq: it was never pending in business or there were > 1 in business? wtf? Since we're using 'insert()' I'm pretty sure when inserting them, old ones would have gotten overwritten anyways... Not simply added into. So this condition would probably only occur if remove returns < 1... which would mean it never got put into the business pending in the first place (probably erroneous code path then?)
        }

        //add it to pending lazy ack ack
        actionSpecificAckList->insert(message->Header.MessageId, message);

        //don't send it if queue mode is enabled (merge in progress or socket error detected). our ack list becomes the queue
        if(!static_cast<AbstractClientConnection*>(m_AbstractConnection)->queueActionResponsesBecauseTheyMightBeReRequestedInNewConnection())
        {
            //our flagging tells us there isn't a merge in progress, but that doesn't mean there wasn't one while this message was in pending! so we check that now
            //if(message->Header.MessageId)
            if(!actionSpecificBusinessPendingDuringMergeList->isEmpty())
            {
                if(actionSpecificBusinessPendingDuringMergeList->contains(message->Header.MessageId))
                {
                    actionSpecificBusinessPendingDuringMergeList->removeAll(message->Header.MessageId);
                    //TODOoptimization: verify removeAll only returned one

                    return; //we don't want to stream/transmit this one
                }
            }
            message->Header.GenericRpcMessageType = RpcBankServerMessageHeader::ActionResponseGenericServer2ClientMessageType;
            streamToByteArrayAndTransmitToClient(message);            
        }
    }
public slots:
    //thought about making these slots preprocessor macros since they're so similar, but that's just silly since they're auto-generated anyways! rofl @ my brain seeing them both as being incredibly similar. they both generate code based on input before entering the compiler. It wouldn't accomplish a single thing (since generated code is or can be temporary anyways (HMMMMM, RIGHT? idfk lol... guess the generate step can/should be a part of the toolchain naw mean? TODOoptimization... but still the user needs to be able to #include some generated files... so not sure that's possible and fuck it it's just an optimization anyways... (but if we don't include the rpc generation in the toolchain, then making it a preprocessor macro does have the benefit of saving some space))
    void createBankAccountDelivery();
    void getAddFundsKeyDelivery();
    //etc for each Action
};

#endif // RPCBANKSERVERPROTOCOLKNOWER_H
