#include "rpcbankserverclientshelper.h"

RpcBankServerClientsHelper::RpcBankServerClientsHelper(QObject *parent)
    : IAcceptRpcBankServerBroadcastDeliveries_AND_IEmitActionsForSignalRelayHack(parent), m_Initialized(false), m_RpcBankServerProtocolKnowerFactory(this), m_MultiServerAbstraction(&m_RpcBankServerProtocolKnowerFactory, this)
{
    RpcBankServerProtocolKnowerFactory::setSignalRelayHackEmitter(this);
#if 0
    m_ActionDispensers = new RpcBankServerActionDispensers(this);
    takeOwnershipOfActionsAndSetupDelivery();
#endif
    m_BroadcastDispensers = new RpcBankServerBroadcastDispensers(this);
}
void RpcBankServerClientsHelper::sendBroadcastToArandomClient(IMessage *broadcastMessage)
{
    AbstractClientConnection *clientConnectionToUseForBroadcast_OR_Zero = m_MultiServerAbstraction.getSuitableClientConnectionNextInRoundRobinToUseForBroadcast_OR_Zero();

    if(clientConnectionToUseForBroadcast_OR_Zero)
    {
        static_cast<RpcBankServerProtocolKnower*>(clientConnectionToUseForBroadcast_OR_Zero->protocolKnower())->streamToByteArrayAndTransmitToClient(broadcastMessage);
        //TODOreq: m_MultiServerAbstraction.bulkBroadcastToAllClients(pendingBalanceDetectedMessage);
    }
    //else: no clients connected this might be a good place to cache broadcasts if i ever choose to, but for now i think i'll drop em :-/
}
#if 0
void RpcBankServerClientsHelper::takeOwnershipOfActionsAndSetupDelivery()
{
    //This method is actually quite worthless now that ClientsHelper and ProtocolKnower have merged... However I'll keep the functionality there JUST IN CASE the destination isn't hte same as the owner in the future. Right now it is, so there doesn't need to be a distinction between the two (as there is in the underlying code... just like broadcast dispensers (except those actually need it))

    m_CreateBankAccountMessageDispenser = m_ActionDispensers->takeOwnershipOfCreateBankAccountMessageDispenserRiggedForDelivery(this);
    m_GetAddFundsKeyMessageDispenser = m_ActionDispensers->takeOwnershipOfGetAddFundsKeyMessageDispenserRiggedForDelivery(this);
}
#endif
#if 0
void RpcBankServerClientsHelper::messageReceived(QByteArray *message, quint32 clientId)
{
    //this design sucks. I don't need clientId to be passed around like this, I just need the deliver() signals to be connected to the right "connection objects" depending on which "connection object" read the request. the only exception to that really is broadcasts, which would iterate/round-robin a list typically not used. It saves me a hash lookup for each message because the signals/slot mechanism performs the lookup (that has to be performed anyways) for me. It would take some redesigning to the dispener logic, but is definitely do-able :-/. Man I've got like 20 fucking optimizations/changes in progress right now, my brain's going to explode. And to think, this application worked at one point. There was one commit where it fucking worked! No ack scheme (aside from shitty TCP ack), but it worked! I'm starting to wonder if I'll ever see it in working state ever again. This is a mess. Sorting this mess out SEEMS like a prerequisite to my life. Perhaps it is, but perhaps a complete startover from scratch would save me a lot of sanity? Meh I doubt it, perhaps a design from scratch accomodating all these pending changes? Followed of course by a new implementation of it? Or do I keep hacking/chipping away at this POS? This is a great point to sync/backup/stop/etc for the holidays (going out of town etc). Fuck My Life
}
#endif
#if 0
void RpcBankServerClientsHelper::processCreateBankAccountMessage(CreateBankAccountMessage *createBankAccountMessage, uint uniqueRpcClientId)
{

}
void RpcBankServerClientsHelper::dispatchCreateBankAccountMessageToBusiness(CreateBankAccountMessage *createBankAccountMessage)
{
    //add it to pending in business...
    m_PendingCreateBankAccountMessagesInBusiness.insert(createBankAccountMessage->Header.MessageId, createBankAccountMessage);

    emit createBankAccount(createBankAccountMessage);
}
void RpcBankServerClientsHelper::processGetAddFundsKeyMessage(GetAddFundsKeyMessage *getAddFundsKeyMessage, uint uniqueRpcClientId)
{
    //m_UniqueRpcClientIdsByPendingGetAddFundsKeyMessagePointer.insert(getAddFundsKeyMessage, uniqueRpcClientId);
    emit getAddFundsKey(getAddFundsKeyMessage);
}
#endif
#if 0
void RpcBankServerClientsHelper::createBankAccountDelivery()
{
    //perhaps here is best place to keep track of pendings? because after this spot they have all converged and i'd have to do a switch or something (not necessarily if i increaes the size of each message by keeping a pointer to it's pending list. bad idea. better to do it here)

    //so these transmits right now make it APPEAR i should just connect every .deliverSignal() to a single slot... but for keeping track of pending messages (which i have yet to do), that will turn out not to be the case

    CreateBankAccountMessage *createBankAccountMessage = static_cast<CreateBankAccountMessage*>(sender());
    uint uniqueRpcClientId = m_UniqueRpcClientIdsByPendingCreateBankAccountMessagePointer.take(createBankAccountMessage);

    //Should this go after myTransmit? It probably doesn't make a difference...
    m_CreateBankAccountMessagesAwaitingLazyResponseAck.append(createBankAccountMessage->Header.MessageId, createBankAccountMessage);
    m_PendingCreateBankAccountMessagesInBusiness.remove(createBankAccountMessage->Header.MessageId);

    myTransmit(createBankAccountMessage, uniqueRpcClientId);
    //createBankAccountMessage->doneWithMessage();

    //I think the below is old too, but shit I can't remember the ACK scheme solution I came up with xD. Wasn't it two lists: m_PendingInBusiness and m_AlreadyProcessedAndReturnedButWeCheckInHereWhenTheSameIdComesWeKnowClientGotIt --- or was that for broadcasts? Where the fuck did I put that design. How much of it do I have coded :-/?????? Fuck.

    //^^^^^^^^^^^^^^
    //TODOreq: unsure of ordering of transmit and removing from pending hash. probably doesn't matter? no fucking clue tbh. changed it from .remove to .value and then to .take -- i guess i don't have a choice on the ordering? i think it only matters that the backend impl has in fact finished with the pending. so even if one does get through, the backend impl will catch it definitely
    //^^i like having them overlap by 1 whole line. I doubt it does ANYTHING AT ALL, but i like to know that it's in the ack-awaiting-ack list before we remove it from the business pending one. we check the ack-awaiting-ack list first... so if for some weird random rare as fuck case we crash in between those two calls, and still somehow manage to get back in here, the check for ack-awaiting-ack will come true and we'll simply overwrite the pending in business one (this is desired)
}
void RpcBankServerClientsHelper::getAddFundsKeyDelivery()
{
    GetAddFundsKeyMessage *getAddFundsKeyMessage = static_cast<GetAddFundsKeyMessage*>(sender());
    uint uniqueRpcClientId = m_UniqueRpcClientIdsByPendingGetAddFundsKeyMessagePointer.take(getAddFundsKeyMessage);
    myTransmit(getAddFundsKeyMessage, uniqueRpcClientId);
    getAddFundsKeyMessage->doneWithMessage();
}
#endif
void RpcBankServerClientsHelper::pendingBalanceDetectedDelivery()
{
    //TODOreq: package as QByteArray and sned to MultiServerAbstraction. old code below

    ServerPendingBalanceDetectedMessage *pendingBalanceDetectedMessage = static_cast<ServerPendingBalanceDetectedMessage*>(sender());
    pendingBalanceDetectedMessage->Header.RpcServiceSpecificMessageType = RpcBankServerMessageHeader::PendingBalanceDetectedMessageType; //TODOreq: shouldn't this be set by the dispenser or something? somewhere in the message itself? where the fuck am i setting the MessageType for actions (find MessageType usages = solution). The answer lies somewhere in the client code, as the server never sets a message type (except when reading a message type off the network from the client)
    pendingBalanceDetectedMessage->Header.MessageId = 0; //TODOreq: going to need a messageId scheme for broadcasts after all, since I'm going to be manually ack'ing them (and lazy acking the ack itself). But this is for later on after I get actions working...
    //myBroadcast(pendingBalanceDetectedMessage);
    //pendingBalanceDetectedMessage->doneWithMessage(); //old TODOreq: shouldn't this be after the ACK? might have to re-send it... i guess it depends on the guarantees made by myBroadcast. if before it returns it writes to a couchbase db and WAL promises the delivery, then yes calling doneWithMessage() now is probably* ok. just make sure you know to allocate one whenever we are walking the WAL (either as us or a neighbor [same code, different machine]). getNewOrRecycled _cannot_ be used (bitcoin thread owns dispenser). so maybe we shouldn't do doneWithMessage until the ack IS here??? idfk

    //TODOreq: Pretty sure my broadcasts still need their ACK'ing scheme to be implemented... but I'm going to wait until my Action's ACK-ing scheme is in place + working to do it (since I'll steal it's code :-P)
    sendBroadcastToArandomClient(pendingBalanceDetectedMessage);

    //TODOreq: still need to fix MessageId stuff for broadcasts, but yea want to get actions up and acking before fixing broadcasts
}
void RpcBankServerClientsHelper::confirmedBalanceDetectedDelivery()
{
    ServerConfirmedBalanceDetectedMessage *confirmedBalanceDetectedMessage = static_cast<ServerConfirmedBalanceDetectedMessage*>(sender());
    confirmedBalanceDetectedMessage->Header.RpcServiceSpecificMessageType = RpcBankServerMessageHeader::ConfirmedBalanceDetectedMessageType;
    confirmedBalanceDetectedMessage->Header.MessageId = 0;
    //myBroadcast(confirmedBalanceDetectedMessage);
    //confirmedBalanceDetectedMessage->doneWithMessage();

    sendBroadcastToArandomClient(confirmedBalanceDetectedMessage);
}
void RpcBankServerClientsHelper::initialize(MultiServerAbstractionArgs multiServerAbstractionArgs)
{
    emit d("RpcBankServerClientsHelper received init message");
    m_MultiServerAbstraction.initialize(multiServerAbstractionArgs);
    m_Initialized = true;
    emitInitializedSignalIfReady();
}
void RpcBankServerClientsHelper::start()
{
    emit d("RpcBankServerClientsHelper received start message");
    m_MultiServerAbstraction.start();
    emit started();
}
void RpcBankServerClientsHelper::beginStoppingProcedure()
{
    //had below todo a req, but made it optimization because for now i'll be the one coding the client so i won't send more later. but to be more robust when it is released to end users, it can respond with error code to new messages after receiving this slot/message
    //TODOoptimization: make it so incoming Action Requests are rejected with special error code, perhaps by calling a method on m_MultiServerAbstraction... or just by setting up a flag and responding directly to any incoming Action Requests right away in 'this'
    //^^ It makes more sense to have 'this' take care of it, since that 2-stage server-shutdown procedure is specific to the rpc generator. HOWEVER it might be a race condition that a message is in MultiServerAbstraction still? I don't even know what the fuck I mean by this, probably wrong...

    emit beginningStopProcedureInitiated();
}
void RpcBankServerClientsHelper::stop()
{
    emit d("RpcBankServerClientsHelper received stop message");
    m_MultiServerAbstraction.stop();
    emit stopped();
}
void RpcBankServerClientsHelper::handleDoneClaimingBroadcastDispensers()
{
    emitInitializedSignalIfReady();
}
#if 0 // leaving this huge chunk of code here because i see a bunch of TODOreqs that I don't feel like reading
void IBankServerProtocolKnower::createBankAccountFailedUsernameAlreadyExists()
{
    CreateBankAccountMessage *createBankAccountMessage = static_cast<CreateBankAccountMessage*>(sender());
    uint uniqueRpcClientId = m_UniqueRpcClientIdsByPendingCreateBankAccountMessagePointer.take(createBankAccountMessage);
    //TODOreq: handle error where the message wasn't in pending for some reason :-/. do it for regular delivery() too...
    createBankAccountMessage->fail(CreateBankAccountMessage::FailedUsernameAlreadyExists);
    myTransmit(createBankAccountMessage, uniqueRpcClientId);
    createBankAccountMessage->doneWithMessage();
}
void IBankServerProtocolKnower::createBankAccountFailedPersistError()
{
}
void IBankServerProtocolKnower::getAddFundsKeyFailedUsernameDoesntExist()
{
    GetAddFundsKeyMessage *getAddFundsKeyMessage = static_cast<GetAddFundsKeyMessage*>(sender());
    uint uniqueRpcClientId = m_UniqueRpcClientIdsByPendingGetAddFundsKeyMessagePointer.take(getAddFundsKeyMessage);
    getAddFundsKeyMessage->fail(GetAddFundsKeyMessage::FailedUsernameDoesntExist); //sets IMessage->Header.Success to false, which causes IMessage::quint32 failedReason to be set with the enum and also streamed. it isn't streamed when success is true. broadcasts have no use for the Success/failReason shit, so maybe it should be in a class that only Actions inherit? Actions on both client and server need it, and neither Broadcasts on client nor server need it. errors are action specific. FUCK, Success is in the _header_...... i could just not stream it for broadcasts, but then i have to do expensive switching... or stream an additional bool 'isAction'.... which would cost the same as just streaming Success in the first place rofl...

    myTransmit(getAddFundsKeyMessage, uniqueRpcClientId);
    getAddFundsKeyMessage->doneWithMessage();
}
void IBankServerProtocolKnower::getAddFundsKeyFailedUseExistingKeyFirst()
{
    GetAddFundsKeyMessage *getAddFundsKeyMessage = static_cast<GetAddFundsKeyMessage*>(sender());
    uint uniqueRpcClientId = m_UniqueRpcClientIdsByPendingGetAddFundsKeyMessagePointer.take(getAddFundsKeyMessage);
    getAddFundsKeyMessage->fail(GetAddFundsKeyMessage::FailedUseExistingKeyFirst);
    myTransmit(getAddFundsKeyMessage, uniqueRpcClientId);
    getAddFundsKeyMessage->doneWithMessage();
}
void IBankServerProtocolKnower::getAddFundsKeyFailedWaitForPendingToBeConfirmed()
{
    GetAddFundsKeyMessage *getAddFundsKeyMessage = static_cast<GetAddFundsKeyMessage*>(sender());
    uint uniqueRpcClientId = m_UniqueRpcClientIdsByPendingGetAddFundsKeyMessagePointer.take(getAddFundsKeyMessage);
    getAddFundsKeyMessage->fail(GetAddFundsKeyMessage::FailedWaitForPendingToBeConfirmed);
    myTransmit(getAddFundsKeyMessage, uniqueRpcClientId);
    getAddFundsKeyMessage->doneWithMessage();
}
#endif
