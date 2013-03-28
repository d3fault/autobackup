#ifndef RPCBANKSERVERPROTOCOLKNOWER_H
#define RPCBANKSERVERPROTOCOLKNOWER_H

#include <QHash>

#include "iprotocolknower.h"

#include "../../RpcBankServerAndClientShared/rpcbankserverheader.h"

#include "iemitrpcbankserveractionrequestsignalswithmessageasparamandiacceptactiondeliveries.h"
#include "iprotocolknower.h"

#include "MessagesAndDispensers/Dispensers/Actions/servercreatebankaccountmessagedispenser.h"
#include "MessagesAndDispensers/Dispensers/Actions/servergetaddfundskeymessagedispenser.h"
//etc for each Action

//signal relay hack
#include "iacceptrpcbankserverbroadcastdeliveries_and_iemitactionsforsignalrelayhack.h"

class RpcBankServerProtocolKnower : public IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptActionDeliveries, public IProtocolKnower
{
    Q_OBJECT
public:
    static void setSignalRelayHackEmitter(IAcceptRpcBankServerBroadcastDeliveries_AND_IEmitActionsForSignalRelayHack *signalRelayHackEmitter) { m_SignalRelayHackEmitter = signalRelayHackEmitter; }
    explicit RpcBankServerProtocolKnower(QObject *parent);
    virtual void messageReceived(); //QDataStream *messageDataStream);
    virtual void notifyThatQueueActionResponsesHasBeenEnabled();
    inline void streamToByteArrayAndTransmit(IMessage *message)
    {
        //stream to byte array
        resetTransmitMessage();
        m_TransmitMessageDataStream << *message;

        //transmit
        m_AbstractClientConnection->transmitMessage(&m_TransmitMessageByteArray);
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

    //TODOreq: shit our messages get so easily sent back to our rpc bank server clients helper (the specific connection really) that I don't know where to do pendingInBusiness.remove!!! perhaps in the deliver() logic just before the signal is emitted??? Fuck me this really screws with the design, never thought of that before now. I'll probably need a hack to accomodate it. HAHAHAHA rofl oh just realized I am the same person who receives those signals... so it should be easy once I get that coded :-P. Still worth the TODOreq because I simply didn't have that much coded in the part I'm taking the code from (ported/upgraded/optimized/redesigned)
    //TODOreq: figure out if, since it's inline, that they need to be pointers in order to not increase the reference count. since it's inline it might be faster (no deref'ing) to pass by value. idk how inline handles implicitly shared
    inline bool processNewRpcBankServerActionMessage_AND_RecordMessageInBusinessAndReturnTrueIfNeedEmitToBusiness(IActionMessage *actionMessage, QHash<quint32, IActionMessage*> *actionSpecificAckList, QHash<quint32, IActionMessage*> *actionSpecificPendingInBusinessList)
    {
        //TODOreq: even though this function doesn't use my various bits (retry1, retryBecauseConnectionMerge, ahh isn't this sameshit as messageType below?) in their actual names, I do deem it usable for now. I think once I get to the client implementation portion of this it will become more evident if I actually need to change the designs a bit
        //TODOreq: i still think i need to add a messageType to differentiate between actionRequests/actionResponses/broadcasts/cleanDisconnectRequest/actionRequestRetry1(expects response). It needs to go over the network ofc. We could share the field with the hello code, but really don't have to since our protocol is dynamic as fuck :). Still it might be wise to do so?

        //our first check is for the lazy ack ack, as it is the most likely. we COULD check that it's in business still (I guess this depends on if the retry bit is set?), but since that's less likely we'll check it next

        //first just see if it's there. it will most likely be there. if it isn't, it's very likely in business pending...
        IActionMessage *messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero = actionSpecificAckList->value(actionMessage->Header.MessageId, 0);

        if(messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero)
        {
            //so it's in the ack awaiting ack list, now we need to determine if we're lazy ack'ing an old message or if we want to re-send the response. this is why we lazy ack to begin with...

            //edit: for example right here the retry bit should be specified because what if the first request just didn't make it?

            if(messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero->toggleBit() == actionMessage->toggleBit())
            {
                //if they are the same, retryBit is implied and we just return the one that's already existing.
                //TODOreq: wtf do I do with the new one? just recycle it right here right now? I think so but am unsure...

                //it might seem inefficient that we have two copies of the message at this point... BUT WE (client) DON'T KNOW THAT THE FIRST ONE ARRIVED MOTHERFUCKER... so we did have to send the whole request again. HOWEVER, the server has determined it's worthless.
                //TODOmessageErrors: this is our response retry case

                actionMessage->doneWithMessage(); //doesn't have response filled out anyways~ piss off

                //we re-send the old one, but still don't remove it from the ack pending ack. what if this one gets lost too? actually TODOreq: since this would be our second attempt at a response, there is definitely not going to be a third SO it might be ok to say doneWithMessage at this point??????? it would mean that the next time we see the MessageId, it will be treated as though it was the first~ <---- all of this is only true if we only retry exactly once. err, try exactly twice.

                //re-transmit
                //messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero->deliver(); //TODOreq (possibly done already): the handler of deliver() expects only to see messages coming from business. It removes them from the pending in business. It shouldn't matter that we remove our messageId/message from a hash that we aren't in, but it is a worthwhile optimization to NOT do so. It also re-appends us to our ack-awaiting-ack list... so maybe I should just call myTransmit here directly instead???? just don't have that clientId, but I think I'm going to be adding that as a member of IActionMessage anyways???? (it does not apply to broadcast I don't think? (aside from them special client-prioritized broadcasts that I'm not even sure I can use yet lmfao, which'd probably include a db lookup of some sort to figure out who that client is anyways? really no fucking clue))
                if(!m_AbstractClientConnection->queueActionResponsesBecauseTheyMightBeReRequestedInNewConnection())
                {
                    streamToByteArrayAndTransmit(messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero);
                }
                //else: we got a retry from a connection that has since failed or is about to merge, so meh don't bother trying to send it back because the new connection will be the one to do that
            }
            else
            {
                //they are not the same, so we know we're on the next message. We can now recycle the old one: we know we don't need it anymore. The ack has officially been ack'd :-D
                actionSpecificAckList->remove(messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero->Header.MessageId);
                messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero->doneWithMessage();


                //TODOreq: now that that's taken care of, we begin processing the new message

                //TODOmessageErrors: this is our typical non-error case, but for every time after the first time we use that MessageId

                //dispatchCreateBankAccountMessageToBusiness(createBankAccountMessage);
                actionSpecificPendingInBusinessList->insert(actionMessage->Header.MessageId, actionMessage);
                return true;
            }
        }
        else
        {
            //not in ack lazy awaiting ack list, so it might be in business pending... or TODOreq: it might be our first time seeing it and in neither!!!

            //hack: the name of the pointer doesn't match our usage, but no point in allocating another one lol. is 'messageOnlyIfPendingInBusiness' from now on
            messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero = actionSpecificPendingInBusinessList->value(actionMessage->Header.MessageId, 0);

            //if messageOnlyIfPendingInBusiness really
            if(messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero)
            {
                //as of right now, both of the business pending checks come here. despite coming at different times, all we have to do is tell the truth. we are still in business! I guess we don't need the 3 different requests to indicate which status they are, simply sending the same message with the toggle bit not changing three times in a row will indicate it!
                //TODOreq: it is worth noting that the first (initial), second (retry1/status1), and third (retry2,status2) messages that a client sends to the server are the exact same, yet only the second (and POSSIBLY third, though it can be an optimization to know from the second one whether or not the whole message needs to be resent) warrants a response ("still in business"). The response to the first one is the answer/response itself [assuming no errors and normal conditions]! The 3rd message is the same as the second and also warrants the same response. IT IS UP TO THE CLIENT to be able to differentiate between any responses in retryBecauseRequestNetworkTimeout, stillInBusiness[1], retryBecauseBusinessTimedOut, stillInBusiness[2]
                //TODOreq: there is an amount of custom logic that must be performed to be able to deduce on the client what messages are what. if we receive our answer/response just after we dispatch either of our retry* (also 2->3rd might be even more complicated idfk), then we need to know we're definitely going to get the same answer/response and this time with the responseBit set (i might have meant to write "responseRETRYbit" right there, idfk)

#if 0
    ********TODOreq : say "still in business"********** -- twice too
                        TODO LEFT off
                        and i think i need a message type bit. i need it for clean disconnects, but also for an alleged "still in business" message. i think i'll end up doing a request type, requestRetry1(status1) type, requestRetry2(status2) type, requestResponse type, broadcast type, disconnectPlz type, stillInBusiness type (status1 response), stillInBusinessAgainWtfLoLTimeToCrashProllyButThatsNotUpToMe(status2 response), etc as many as needed
                        i am getting so close i can taste it
#endif
                        //messageOnlyIfTheAckIsLazyAwaitingAck__OrElseZero->setErrorCode(); or something similar, but error codes are specific to the user's protocol so DEFINITELY NOT THAT. I think I need to delete my old/outdated "Header" class but not sure how to replace it. My brain fucking hurts.

                        //TODO LEFT OFF REALLY IMPORTANT THIS IS THE LATEST (for now rofl) REQUEST PROTOCOL WHEN ALL SAID AND DONE -- this attempt was written after the helloer/qbapeeker/magic etc rewrite. I'm pretty sure all other attempts were written before those and are therefore outdated
                        //Yet another attempt at finalizing the request protocol on the byte level:

                        //PeekedAndReadInFullAlreadyQByteArray[HelloStatus(Done normally, but we have to re-stream this over and over so that we can eventually get a "DisconnectRequested" status. Other statuses are InitialHello etc),RpcGeneratorAbstractMessageBody[quint32 RpcServiceId, quint32 RpcServiceMessageId(can-repeat-in-other-rpc-service-ids-but-never-within-one), quint32/enum whatThisMessageIs[InitialRequest(fuck toggle bit i guess?),RequestRetry1Status1,RequestRetry2Status2WithMessage,RequestRetry2Status2WithoutMessage], quint32 RpcServiceIdSpecificMessageType (stored as a quint32, but utilized by inheriters to assign special enum types indicating the rpc service methods made available), QByteArray theActualFuckingMessage]]







                    //TODOreq LEFT OFF: say "still in business" motherfucker. the above protocol definition for requests might be my only real accomplishment for the day. still need to implement it and then come back here and USE it... but eh i'm staring at it extra long to make sure i didn't miss anything. fuck yea deftones white pony









                        //TODOreq: Does the hello level handle the "disconnect requested received so ignoring your request", or does the rpc level handle it? If the rpc level handles it like I'm thinking, we need a custom response type TODOreq that says something like "WontRespondBecauseDisconnectRequestedUseAnotherClient" (mindfuck: does that WontRespond message get ack'd?). Still it does seem like the hello'er level would be involved. Perhaps it also has a disconnect message that is only used AFTER we are sure we are ready to perform a clean disconnect? We could be cute and call it Goodbye ;-P

                    //TODOreq: the difference between the first network timeout and the second business timeout (both detected on client via same way) needs to be greater than the average response time for that connection. We need to give ample time for our RequestRetry1Status1 to be responded to before dispatching the RequestRetry2Status2 (this does relate to whether or not RequestRetry2 will contain the full request again, but also makes common sense even without that optimization)

                //TO DONEreq (decided i need to have two status checks and the first one is a retry but the second can know whether or not to send the message again (a small optimization TODOoptimization)): decided that rpc generator will first report the status of the message, and then later ----- actually fuck it that makes unnecessary complications. I should just _ONLY_ check once: the maximum timeout allowed. But the decision remains: I will simply "report" the stuck-in-business error and let the controller-of handle it. There is no need to ask until -----
                //ACTUALLY WAIT THERE IS A REASON
                //If I _DON'T_ have a 2-phase "eh he's still in pending gimmeh more time" method, then TimeUntilResponseLostCheckingIsPerformed has to EQUAL MaximumTimeAllowedInBusiness. Is it ok to make these equal (because we'd only have one check to handle both)?
                //^^^^^It doesn't seem TOOOOO harmful, but I'd like to think that I'm giving my business an ample amount of time (some slow down is acceptable versus shutting down everything after all) while still having snappy "response lost here it is again" recovery. That tells me they shouldn't equal each other.



                //psbly old below, idfk:

                //TODOreq: still in business. should we respond with a 'give us more time plox because if this doens't work itself out, shit is FUCKED and everything should shut down'? TODOreq: so does that imply that the client sends us yet another (3rd!!!) try? And if THAT ONE doesn't work, then we're really really fucked? Or should I ignore it (store it?) and set up a timer on the business and then report fucked? I like the idea of staying in communication with the client at this point, even though we don't have much to say except "it's the business, not us". :-/. This is probably one of the hardest use cases to decide what to do... but I guess I'd say opt towards giving him more time before shutting the fuck down... or even trying another server
                //I'm going to be so ridiculously happy if I never ever encounter this code path. I mean sure during development you are bound to fuck up... but your business MUST respond in production... else we shut everything the fuck down (TODOreq: this is a good as fuck reason to keep applications... servers... whatever.... appIds.... AWAY FROM EACH OTHER. so if one service goes down it doesn't bring down everything else that is working perfectly fine. I guess I could just do it on a per-appId basis... if I really want to keep everything "one-giant-binary-to-rule-them-all" roflrofl good luck stealing my source if the source never touches the server, only binaries do. even though that has nothing to do with me doing them in separate binaries lol. but yea I mean I think you get more efficiency and therefore scalability and therefore profits by having all your services on ONE GIANT DHT. I don't even want to think about 'providing low latency access to overseas countries' though. I guess those would just be copy/pasta different servers/clusters altogether? It'd work for services like email etc where the user has their own account... but for shit like ABC AdDoubles, we need worldwide coordination so it's MUCH easier to just have a giant ass single dht like I said. The latency has to be dealth with either way. Either inter-cluster realtime synchronization (NO-FUCKING-THANKS (not to be confused with cross-cluster backing up, which yea i'll have running 24/7 both ways (or 3 ways etc), but they are DELAYED, not realtime!!! (also: still dunno how rolling back or whatever would work with a delayed/lagged copy. but shit i know one thing: it's better to have a few seconds/minutes outdated copy than NO COPY AT ALL (in the case of fire/explosion/etc lmfao)))) or have the user do it (yes))
            }
            else
            {
                ////TODOmessageErrors: non-error case, first time seeing this MessageId
                //dispatchCreateBankAccountMessageToBusiness(createBankAccountMessage);
                actionSpecificPendingInBusinessList->insert(actionMessage->Header.MessageId, actionMessage);
                return true;
            }
        }
        return false;


        //TODOreq: somewhere in the above I need to actually toggle the bit. Or wait no, client does that right???? still worth noting.


        //check in business still if retry bit is set? actually maybe i don't even need the retry bit anymore if I'm using a toggling-per-messageId-use design. if I get the same messageId without the bit toggled, then retry is IMPLIED????


        //m_UniqueRpcClientIdsByPendingCreateBankAccountMessagePointer.insert(createBankAccountMessage, uniqueRpcClientId);

        //I think this is old, idfk:
        //TODOreq: perhaps check the list already by value to see if we're already working on the same request?
        //if we are, it still might fail and then we should process this one. so i need to atomically access a list in that case, a list of "pendingCollissionsAwaitingFinalizatioin"
        //the list is mutex-grabbed+cleared AFTER we transmit the message back to
        //actually this idea sucks
        //maybe we can instead connect the clearing of the list to the chosen/conflicting/got-their-first recycle signal after it's finished? so no additional processing needs to be done

        //emit createBankAccount(createBankAccountMessage);
    }
    inline void removeFromPendingInBusiness_AND_addToAwaitingAck_And_StreamToByteArray_And_Transmit(IActionMessage *message, QHash<quint32, IActionMessage*> *actionSpecificAckList, QHash<quint32, IActionMessage*> *actionSpecificPendingInBusinessList, QList<quint32> *actionSpecificBusinessPendingDuringMergeList)
    {
        //remove it from pending business
        if(actionSpecificPendingInBusinessList->remove(message->Header.MessageId) != 1)
        {
            //TODOreq: it was never pending in business or there were > 1 in business? wtf?
        }

        //add it to pending lazy ack ack
        actionSpecificAckList->insert(message->Header.MessageId, message);

        //don't send it if queue mode is enabled (merge in progress or socket error detected). our ack list becomes the queue
        if(!m_AbstractClientConnection->queueActionResponsesBecauseTheyMightBeReRequestedInNewConnection())
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
            streamToByteArrayAndTransmit(message);
        }
    }
public slots:
    void createBankAccountDelivery();
    void getAddFundsKeyDelivery();
    //etc for each Action
};

#endif // RPCBANKSERVERPROTOCOLKNOWER_H
