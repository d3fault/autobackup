#ifndef RPCBANKSERVERCLIENTSHELPER_H
#define RPCBANKSERVERCLIENTSHELPER_H

#include <QObject>
#include <QHash>

#include "iacceptrpcbankserverbroadcastdeliveries_and_iemitactionsforsignalrelayhack.h"
//#include "../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/createbankaccountmessage.h"
//#include "../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/getaddfundskeymessage.h"
#include "MessagesAndDispensers/Messages/Broadcasts/serverpendingbalancedetectedmessage.h"
#include "MessagesAndDispensers/Messages/Broadcasts/serverconfirmedbalancedetectedmessage.h"
//#include "MessagesAndDispensers/Dispensers/rpcbankserveractiondispensers.h"
#include "MessagesAndDispensers/Dispensers/rpcbankserverbroadcastdispensers.h"
//#include "MessagesAndDispensers/Dispensers/Actions/servercreatebankaccountmessagedispenser.h"
//#include "MessagesAndDispensers/Dispensers/Actions/servergetaddfundskeymessagedispenser.h"
#include "rpcbankserverprotocolknowerfactory.h"
#include "multiserverabstraction.h"

//hack: RpcBankServerClientsHelper (this) is a protocol knower too, but only deals with Broadcasts because they need a single connection-apathetic destination, whereas actions will be connected directly to their protocol knower instantiated for their connection. could make a random BroadcastProtocolKnower and ActionProtocolKnower, but who cares for now
class RpcBankServerClientsHelper : public IAcceptRpcBankServerBroadcastDeliveries_AND_IEmitActionsForSignalRelayHack
{
    Q_OBJECT
public:
    explicit RpcBankServerClientsHelper(QObject *parent = 0);
    inline RpcBankServerBroadcastDispensers *broadcastDispensers() { return m_BroadcastDispensers; }
private:
    void sendBroadcastToArandomClient(IMessage *broadcastMessage);
    //void takeOwnershipOfActionsAndSetupDelivery();
    inline void emitInitializedSignalIfReady() { if(checkInitializedAndAllBroadcastDispensersClaimed()) emit initialized(); }
    inline bool checkInitializedAndAllBroadcastDispensersClaimed() { return (m_Initialized && m_BroadcastDispensers->everyDispenserIsCreated()); }
    bool m_Initialized;

    //RpcBankServerActionDispensers *m_ActionDispensers;
    RpcBankServerBroadcastDispensers *m_BroadcastDispensers;

    RpcBankServerProtocolKnowerFactory m_RpcBankServerProtocolKnowerFactory;
    MultiServerAbstraction m_MultiServerAbstraction;


    //TODOreq: probably doesn't belong here, but: the second message with the retryBit set _MIGHT_ (won't with TCP tho.. right?) come before the first message without the retry bit set. My toggling bit solution should be able to detect this I think. Could be wrong but oh fuck that would suck. UDP doesn't guarantee ordering so if I use it, I need to add a message ordering thing to detect YET ANOTHER FALSE POSITIVE lmfao (a retry for a message (which already uses a toggleBit to evade a false positive for a previous message) that comes before an initial message with a lower 'send order number' (to be invented) should should just ignore the initial/late message)



    //these two hashes are used for figuring out wich client requested it..... BUT....
    //-QHash<CreateBankAccountMessage*, uint> m_UniqueRpcClientIdsByPendingCreateBankAccountMessagePointer;
    //-QHash<GetAddFundsKeyMessage*, uint> m_UniqueRpcClientIdsByPendingGetAddFundsKeyMessagePointer;

    //....perhaps instead I should store the entire message as pending in the business? (maybe I still need those above for clientIDs only, idfk. doubt it though, just need to consolidate the clientId into the message or header or something). All I'm saying is that right now, they have very similar purposes so should probably be combined somehow
    //QHash<quint32 /*MessageId*/, CreateBankAccountMessage* /*pending-in-business*/> m_PendingCreateBankAccountMessagesInBusiness;

    //....and again when it's pending an ACK. If it's in this list, all it means is we think we've sent it back to the client. Our (the server's!!!) 'ACK' is when we get the same MessageId again without the 'retryBit' set. If it is set, it didn't get our response, so we pull it out of the list and send it again :-P. TO DOnereq: there is a race condition where we receive a message with a retry bit set because the client has sent it just before receiving our first/original delivery. If we get it with retryBit set, we'd send it again :-/ and the client will receive the same message twice!!!!! Once just moments after sending message again with retryBit set, and another when we get the message with retryBit set and send it again. A solution to this is to have truly unique IDs per-message, but then my server's ACK'ing scheme for actions needs to be redesigned. FML
    //QHash<quint32 /*MessageId*/, CreateBankAccountMessage* /*ack-awaiting-ack*/> m_CreateBankAccountMessagesAwaitingLazyResponseAck;

    //TODOoptimization^: I can use the QIODevice* as a key into a hash to optimize the above two 'types' (pending vs. ack-ing).. but for every action type of course. It'd look like: QHash<QIODevice*, QHash<quint32, CreateBankAccountMessage*> > m_PendingCreateBankAccountMessagesInBusinessByMessageIdByClientPointer; OR SOMETHING???? and of course the same thing for acks awaiting acks...

protected:
#if 0
    inline void copyLocalHeaderToMessageHeader(const RpcBankServerMessageHeader &localHeader, IRecycleableAndStreamable *message)
    {
        //message->Header.MessageSize = localHeader.MessageSize;
        message->Header.RpcServiceId = localHeader.RpcServiceId;
        message->Header.MessageId = localHeader.MessageId;
        message->Header.RpcServiceSpecificMessageType = localHeader.RpcServiceSpecificMessageType;
    }
#endif

    //ServerCreateBankAccountMessageDispenser *m_CreateBankAccountMessageDispenser;
    //ServerGetAddFundsKeyMessageDispenser *m_GetAddFundsKeyMessageDispenser;

    //void messageReceived(QByteArray *message, quint32 clientId);
    //virtual void myTransmit(IMessage *message, uint uniqueRpcClientId)=0;
    //virtual void myBroadcast(IMessage *message)=0;

    //we only have process* classes for Action requests
    //void processCreateBankAccountMessage(CreateBankAccountMessage *createBankAccountMessage, uint uniqueRpcClientId);
    //void dispatchCreateBankAccountMessageToBusiness(CreateBankAccountMessage *createBankAccountMessage);

    //void processGetAddFundsKeyMessage(GetAddFundsKeyMessage *getAddFundsKeyMessage, uint uniqueRpcClientId);
    //the point of the process* is to add them to a pending list. and then after we append to the list we also emit. since we are auto-generated, process* doesn't have to. it makes no difference who emits, but by having a process() for each, we simplify the design imo
signals:
    void d(const QString &);
    void initialized();
    void started();
    void beginningStopProcedureInitiated(); //does not accept incoming Action Requests, but still allows business to give it Action Responses and Broadcasts
    void stopped();
public slots:
    void initialize(MultiServerAbstractionArgs multiServerAbstractionArgs);
    void start();

    //TODOreq: While it would be nice to be able to call beginStoppingProcedure on the server using some server GUI etc, for now I'm going to have the client send a "clean disconnect" message and the server just respond to it [when ready]. there seems to be a lot of overlap between "server shutdown" (all clients dc) and "clean disconnect a single client", which needs to be fixed
    //^^^when implementing the server ability to shutdown and dc all clients, it should basically just "request" to the client a "clean disconnect" message from each client, and it should do it to each one asynchronously of course
    void beginStoppingProcedure(); //Stop accepting Action Requests. TODOreq: probably need a special message code for this 'shutdown in progress' telling them that (a) the server is still online for a BIT longer [broadcasts (actually we can stop broadcasts right away (but we could also keep them coming if and only if this connection is the very last connection lol, BLAH)), action requests] (b) it's being shut down so try your action request on another server (or just error out if there aren't any)
    void stop();

    void handleDoneClaimingBroadcastDispensers();




    //outgoing Action responses
    //void createBankAccountDelivery(); //deliver'd from rpc server impl. our IRpcBankServerClientProtocolKnower on rpc client also inherits IAcceptRpcBankServerActionDeliveries
    //void getAddFundsKeyDelivery();

    //outgoing Broadcasts
    void pendingBalanceDetectedDelivery();
    void confirmedBalanceDetectedDelivery();





    //OLD AS FUCK:
    //my IAcceptRpcBankServerActionDeliveries doesn't account for errors atm, fml. actually i don't think it matters. actually yes it does. because my CreateBankAccountMessage on the rpc client has no need for rigging the errors like deliver()'ing... we just emit them (the errors) as a parameter to the rpc client impl
    //i don't want to have 2 sets of messages. bullshit. reallly though, who gives a shit. auto-generated is auto-generated
    //fml i am overcomplicating the FUCK out of this


    //---------------------------------
    //RpcClientActionRequest -- no need for errors or even for the client to see "recycle" (but no need to hide it). the response is allocated but hidden for now. it does not get sent across the network, only the request params do

    //RpcServerActionMessage -- request params const (read-only), response params accessible, expected to be filled out (can be forced (throwing errors if improperly configured))
    //RpcServerBroadcast -- errors? delivering && recycling definitely.

    //RpcClientActionResponse -- both request and response values read-only
    //RpcClientBroadcast -- errors (if applicable). NO NEED FOR DELIVERING. recycling still used

    //from those 5 message types / message use cases, we should be able to define a perfect class inheritence heirchy

    //similarities amongst all 5: recycling, serializable. this is 2/3 features that currently compose IMessage. the third being .deliver()'ing... which only the RpcClientBroadcast doesn't need. 4/5 it might just be simplest to just leave it in there~

    //similarities amongst Client Code: same as the similarities between all 5. just recycling, serializable. there is a level of abstraction where the response is hidden in the case we are an action request (there doesn't have to be)
    //similarities amongst Server Code: every message is .deliver()'able. maybe that's an inherent property of being a server. and basically the other 2 (recycleable,serializable) apply and that's it.

    //similarities amongst Action Messages: the request params are always read-able at the very least, always recycleable
    //similarities amongst Broadcast Messages: always recycleable -- really that's it. broadcasts on the client are completely different. the params are read-only and there is no deliver. though having both of those not coded is not a big deal.

    //^^wrote "similarities amongst" for every category before actually filling them in. ok filled in the 'all 5' one first. but liked the idea a lot and figured i'd forget the 4 distinct categories


    //time for a mother fucking grid box, my eyes are too lazy and my brain is too disconnected and lazy to want to manually figure out the class inheritence shits

    /*

                                Deliverable     Recycleable     Serializable    OtherHalfHidden     CertainHalfReadOnly     BothHalvesReadOnly
      RpcClientActionRequest    Y               Y               Y               Y                   N                       N
      RpcServerActionMessage    Y               Y               Y               N                   Y                       N
      RpcServerBroadcast        Y               Y               Y               n/a                 n/a                     n/a
      RpcClientActionResponse   N               Y               Y               N                   N                       Y
      RpcClientBroadcast        N               Y               Y               n/a                 n/a                     n/a || Y

      brain is in autopilot mode. relaxing.

      so i think I've decided  to not have Request/Response separation, nor Hidden/Readonly-able which means the following table applies:

      RpcClientActionMessage    Y               Y               Y -- the deliverable ability is simply ignored on the client
      RpcServerActionMessage    Y               Y               Y
      RpcServerBroadcastMessage Y               Y               Y
      RpcClientBroadcastMessage

      err no fuck that shit. it's even simpler. you've had it once before.

      ==========JACKPOT=========

      RpcActionMessage          Y               Y               Y -- the recycle ability is simply ignored on the server. deliver is used on both client and server
      RpcBroadcastMessage       Y               Y               Y -- the deliver ability is simply ignored on the client. recycle is only used on the client.

      so it is IMessage after all...
      TODOoptimization: you can rig .deliver and .recycle to check if they are being called from the right place (a recycle in broadcast server impl re-interprets, based on the fact that we're IN the recycle thread (and therefore haven't been delivered yet), the doneWithMessage to actually emit the doneSignal() message. there are similar optimizations using the deliver() in the wrong places that can make it recycle instead. the rpc client when it receives something, for example. this prevents/saves user error. but there might be a case where the destination and recycler are the same thing or something? idfk and don't feel like exploring this any further


      old:


      so among the Actions vs. Broadcasts, the main characteristic is that Actions actually have an answer to the Hidden/Read-Only "public API nice'ness"

      err nvm just realized the BothHalvesReadOnly dilema for RpcClientBroadcast. such trivial problems i am overcomplicating

      but how do i want to deal with it?

      man i was stupid for even thinking they'd all be able to use one interface: IMessage

      unless IMessage just does the Recycling and Serialization

      class IMessage : IRecyclable, ISerializable <-- really the code for it is the IMessage impl
      {
      }

      class IDeliverable : IMessage
      {
      }

      class IRpcClientActionRequest : IDeliverable
      {
        request params public, response allocated but private
      }

      class IRpcServerActionMessage : IDeliverable
      {
        both request and response params public in impl
      }

      class RpcServerBroadcast : IDeliverable
      {
        broadcast params public, very basic impl
      }

      class RpcClientActionResponse : IMessage??????????????? methinks not, since the Request is the recycling 'parent'
      {
        both request and response are read-only. doneWithMessage() just calls the request's doneWithMessage. so that means we don't inherit IMesssage because our "parent" request (which is IMessage FOR us) will take care of the recycling. if we try to do it, it will just add another layer of *unecessary* complication
      }

      this makes me want to just make them "Message" again... ignoring the RequestResponse hiding/write-protecting. not worth the coding overhead

      but i feel like i might not be able to

      who cares if my ending broadcast is deliverable? i could make the destination be the same as the recycle owner. and even add code on the rpc client to handle a deliver as a recycle. fuck the police

      jesus i'm fucking stupid

      yes it's "dangerous"
      no it isn't worth coding it out. at least not right now. maybe in the future if other people use the code generator ( i license it or who knows what i decide.. TODOoptional)

      didn't i just call myself stupid for saying they could all be IMessages?

      yep., guess i changed my mind

    */

};

#endif // RPCBANKSERVERCLIENTSHELPER_H
