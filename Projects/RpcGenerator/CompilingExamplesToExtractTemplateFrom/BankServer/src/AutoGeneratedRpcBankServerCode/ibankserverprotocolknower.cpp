#include "ibankserverprotocolknower.h"

IBankServerProtocolKnower::IBankServerProtocolKnower(QObject *parent) :
    IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptAllDeliveries(parent)
{ }
void IBankServerProtocolKnower::setActionDispensers(RpcBankServerActionDispensers *rpcBankServerActionDispensers)
{
    m_RpcBankServerActionDispensers = rpcBankServerActionDispensers;
}
void IBankServerProtocolKnower::takeOwnershipOfActionsAndSetupDelivery()
{
    m_CreateBankAccountMessageDispenser = m_RpcBankServerActionDispensers->takeOwnershipOfCreateBankAccountMessageDispenserRiggedForDelivery(this);
    m_GetAddFundsKeyMessageDispenser = m_RpcBankServerActionDispensers->takeOwnershipOfGetAddFundsKeyMessageDispenserRiggedForDelivery(this);
}
void IBankServerProtocolKnower::processCreateBankAccountMessage(CreateBankAccountMessage *createBankAccountMessage, uint uniqueRpcClientId)
{
    m_UniqueRpcClientIdsByPendingCreateBankAccountMessagePointer.insert(createBankAccountMessage, uniqueRpcClientId);

    //I think this is old, idfk:
    //TODOreq: perhaps check the list already by value to see if we're already working on the same request?
    //if we are, it still might fail and then we should process this one. so i need to atomically access a list in that case, a list of "pendingCollissionsAwaitingFinalizatioin"
    //the list is mutex-grabbed+cleared AFTER we transmit the message back to
    //actually this idea sucks
    //maybe we can instead connect the clearing of the list to the chosen/conflicting/got-their-first recycle signal after it's finished? so no additional processing needs to be done

    emit createBankAccount(createBankAccountMessage);
}
void IBankServerProtocolKnower::processGetAddFundsKeyMessage(GetAddFundsKeyMessage *getAddFundsKeyMessage, uint uniqueRpcClientId)
{
    m_UniqueRpcClientIdsByPendingGetAddFundsKeyMessagePointer.insert(getAddFundsKeyMessage, uniqueRpcClientId);
    emit getAddFundsKey(getAddFundsKeyMessage);
}
void IBankServerProtocolKnower::createBankAccountDelivery()
{
    //perhaps here is best place to keep track of pendings? because after this spot they have all converged and i'd have to do a switch or something (not necessarily if i increaes the size of each message by keeping a pointer to it's pending list. bad idea. better to do it here)

    //so these transmits right now make it APPEAR i should just connect every .deliverSignal() to a single slot... but for keeping track of pending messages (which i have yet to do), that will turn out not to be the case

    CreateBankAccountMessage *createBankAccountMessage = static_cast<CreateBankAccountMessage*>(sender());
    uint uniqueRpcClientId = m_UniqueRpcClientIdsByPendingCreateBankAccountMessagePointer.take(createBankAccountMessage);
    myTransmit(createBankAccountMessage, uniqueRpcClientId);
    createBankAccountMessage->doneWithMessage();

    //I think the below is old too, but shit I can't remember the ACK scheme solution I came up with xD. Wasn't it two lists: m_PendingInBusiness and m_AlreadyProcessedAndReturnedButWeCheckInHereWhenTheSameIdComesWeKnowClientGotIt --- or was that for broadcasts? Where the fuck did I put that design. How much of it do I have coded :-/?????? Fuck.

    //^^^^^^^^^^^^^^
    //TODOreq: unsure of ordering of transmit and removing from pending hash. probably doesn't matter? no fucking clue tbh. changed it from .remove to .value and then to .take -- i guess i don't have a choice on the ordering? i think it only matters that the backend impl has in fact finished with the pending. so even if one does get through, the backend impl will catch it definitely
}
void IBankServerProtocolKnower::getAddFundsKeyDelivery()
{
    GetAddFundsKeyMessage *getAddFundsKeyMessage = static_cast<GetAddFundsKeyMessage*>(sender());
    uint uniqueRpcClientId = m_UniqueRpcClientIdsByPendingGetAddFundsKeyMessagePointer.take(getAddFundsKeyMessage);
    myTransmit(getAddFundsKeyMessage, uniqueRpcClientId);
    getAddFundsKeyMessage->doneWithMessage();
}
void IBankServerProtocolKnower::pendingBalanceDetectedDelivery()
{
    ServerPendingBalanceDetectedMessage *pendingBalanceDetectedMessage = static_cast<ServerPendingBalanceDetectedMessage*>(sender());
    pendingBalanceDetectedMessage->Header.MessageType = RpcBankServerMessageHeader::PendingBalanceDetectedMessageType; //TODOreq: shouldn't this be set by the dispenser or something? somewhere in the message itself? where the fuck am i setting the MessageType for actions (find MessageType usages = solution). The answer lies somewhere in the client code, as the server never sets a message type (except when reading a message type off the network from the client)
    pendingBalanceDetectedMessage->Header.MessageId = 0;
    myBroadcast(pendingBalanceDetectedMessage);
    pendingBalanceDetectedMessage->doneWithMessage(); //old TODOreq: shouldn't this be after the ACK? might have to re-send it... i guess it depends on the guarantees made by myBroadcast. if before it returns it writes to a couchbase db and WAL promises the delivery, then yes calling doneWithMessage() now is probably* ok. just make sure you know to allocate one whenever we are walking the WAL (either as us or a neighbor [same code, different machine]). getNewOrRecycled _cannot_ be used (bitcoin thread owns dispenser). so maybe we shouldn't do doneWithMessage until the ack IS here??? idfk

    //TODOreq: Pretty sure my broadcasts still need their ACK'ing scheme to be implemented...
}
void IBankServerProtocolKnower::confirmedBalanceDetectedDelivery()
{
    ServerConfirmedBalanceDetectedMessage *confirmedBalanceDetectedMessage = static_cast<ServerConfirmedBalanceDetectedMessage*>(sender());
    confirmedBalanceDetectedMessage->Header.MessageType = RpcBankServerMessageHeader::ConfirmedBalanceDetectedMessageType;
    confirmedBalanceDetectedMessage->Header.MessageId = 0;
    myBroadcast(confirmedBalanceDetectedMessage);
    confirmedBalanceDetectedMessage->doneWithMessage();
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
