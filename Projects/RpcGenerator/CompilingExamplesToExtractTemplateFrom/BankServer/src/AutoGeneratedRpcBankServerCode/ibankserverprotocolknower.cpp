#include "ibankserverprotocolknower.h"

IBankServerProtocolKnower::IBankServerProtocolKnower(QObject *parent) :
    IEmitRpcBankServerClientActionRequestSignalsWithMessageAsParam(parent)
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
void IBankServerProtocolKnower::processCreateBankAccountMessage(ServerCreateBankAccountMessage *createBankAccountMessage, uint uniqueRpcClientId)
{
    m_UniqueRpcClientIdsByPendingCreateBankAccountMessagePointer.insert(createBankAccountMessage, uniqueRpcClientId);

    //TODOreq: perhaps check the list already by value to see if we're already working on the same request?
    //if we are, it still might fail and then we should process this one. so i need to atomically access a list in that case, a list of "pendingCollissionsAwaitingFinalizatioin"
    //the list is mutex-grabbed+cleared AFTER we transmit the message back to
    //actually this idea sucks
    //maybe we can instead connect the clearing of the list to the chosen/conflicting/got-their-first recycle signal after it's finished? so no additional processing needs to be done

    emit createBankAccount(createBankAccountMessage);
}
void IBankServerProtocolKnower::processGetAddFundsKeyMessage(ServerGetAddFundsKeyMessage *getAddFundsKeyMessage, uint uniqueRpcClientId)
{
    m_UniqueRpcClientIdsByPendingGetAddFundsKeyMessagePointer.insert(getAddFundsKeyMessage, uniqueRpcClientId);
    emit getAddFundsKey(getAddFundsKeyMessage);
}
void IBankServerProtocolKnower::createBankAccountDelivery()
{
    //perhaps here is best place to keep track of pendings? because after this spot they have all converged and i'd have to do a switch or something (not necessarily if i increaes the size of each message by keeping a pointer to it's pending list. bad idea. better to do it here)

    //so these transmits right now make it APPEAR i should just connect every .deliverSignal() to a single slot... but for keeping track of pending messages (which i have yet to do), that will turn out not to be the case

    ServerCreateBankAccountMessage *createBankAccountMessage = static_cast<ServerCreateBankAccountMessage*>(sender());
    uint uniqueRpcClientId = m_UniqueRpcClientIdsByPendingCreateBankAccountMessagePointer.take(createBankAccountMessage);
    myTransmit(createBankAccountMessage, uniqueRpcClientId);
    createBankAccountMessage->doneWithMessage();

    //TODOreq: unsure of ordering of transmit and removing from pending hash. probably doesn't matter? no fucking clue tbh. changed it from .remove to .value and then to .take -- i guess i don't have a choice on the ordering? i think it only matters that the backend impl has in fact finished with the pending. so even if one does get through, the backend impl will catch it definitely
}
void IBankServerProtocolKnower::getAddFundsKeyDelivery()
{
    ServerGetAddFundsKeyMessage *getAddFundsKeyMessage = static_cast<ServerGetAddFundsKeyMessage*>(sender());
    uint uniqueRpcClientId = m_UniqueRpcClientIdsByPendingGetAddFundsKeyMessagePointer.take(getAddFundsKeyMessage);
    myTransmit(getAddFundsKeyMessage, uniqueRpcClientId);
    getAddFundsKeyMessage->doneWithMessage();
}
void IBankServerProtocolKnower::pendingBalanceDetectedDelivery()
{
    ServerPendingBalanceDetectedMessage *pendingBalanceDetectedMessage = static_cast<ServerPendingBalanceDetectedMessage*>(sender());
    pendingBalanceDetectedMessage->Header.MessageType = RpcBankServerHeader::PendingBalanceDetectedMessageType;
    pendingBalanceDetectedMessage->Header.MessageId = 0;
    pendingBalanceDetectedMessage->Header.Success = true;
    myBroadcast(pendingBalanceDetectedMessage);
    pendingBalanceDetectedMessage->doneWithMessage(); //TODOreq: shouldn't this be after the ACK? might have to re-send it... i guess it depends on the guarantees made by myBroadcast. if before it returns it writes to a couchbase db and WAL promises the delivery, then yes calling doneWithMessage() now is probably* ok. just make sure you know to allocate one whenever we are walking the WAL (either as us or a neighbor [same code, different machine]). getNewOrRecycled _cannot_ be used (bitcoin thread owns dispenser). so maybe we shouldn't do doneWithMessage until the ack IS here??? idfk
}
void IBankServerProtocolKnower::confirmedBalanceDetectedDelivery()
{
    ServerConfirmedBalanceDetectedMessage *confirmedBalanceDetectedMessage = static_cast<ServerConfirmedBalanceDetectedMessage*>(sender());
    confirmedBalanceDetectedMessage->Header.MessageType = RpcBankServerHeader::ConfirmedBalanceDetectedMessageType;
    confirmedBalanceDetectedMessage->Header.MessageId = 0;
    confirmedBalanceDetectedMessage->Header.Success = true;
    myBroadcast(confirmedBalanceDetectedMessage);
    confirmedBalanceDetectedMessage->doneWithMessage();
}
void IBankServerProtocolKnower::createBankAccountFailedUsernameAlreadyExists()
{
    //TODOreq: handle errors, perhaps a bool Success at IMessage level and an int reserved for failed enum (only transmitted if that bool Success is false)
    //unsure and have yet to come up with a design for errors

    ServerCreateBankAccountMessage *createBankAccountMessage = static_cast<ServerCreateBankAccountMessage*>(sender());
    uint uniqueRpcClientId = m_UniqueRpcClientIdsByPendingCreateBankAccountMessagePointer.take(createBankAccountMessage);
    //TODOreq: handle error where the message wasn't in pending for some reason :-/. do it for regular delivery() too...
    //TODOreq: idfk what to do. stream an enum or something? should it be part of the header (or a success/failed bool)?
    createBankAccountMessage->fail(ServerCreateBankAccountMessage::FailedUsernameAlreadyExists);
    myTransmit(createBankAccountMessage, uniqueRpcClientId);
    createBankAccountMessage->doneWithMessage();
}
void IBankServerProtocolKnower::createBankAccountFailedPersistError()
{
}
void IBankServerProtocolKnower::getAddFundsKeyFailedUsernameDoesntExist()
{
    ServerGetAddFundsKeyMessage *getAddFundsKeyMessage = static_cast<ServerGetAddFundsKeyMessage*>(sender());
    uint uniqueRpcClientId = m_UniqueRpcClientIdsByPendingGetAddFundsKeyMessagePointer.take(getAddFundsKeyMessage);
    getAddFundsKeyMessage->fail(ServerGetAddFundsKeyMessage::FailedUsernameDoesntExist); //sets IMessage->Header.Success to false, which causes IMessage::quint32 failedReason to be set with the enum and also streamed. it isn't streamed when success is true. broadcasts have no use for the Success/failReason shit, so maybe it should be in a class that only Actions inherit? Actions on both client and server need it, and neither Broadcasts on client nor server need it. errors are action specific. FUCK, Success is in the _header_...... i could just not stream it for broadcasts, but then i have to do expensive switching... or stream an additional bool 'isAction'.... which would cost the same as just streaming Success in the first place rofl...

    myTransmit(getAddFundsKeyMessage, uniqueRpcClientId);
    getAddFundsKeyMessage->doneWithMessage();
}
void IBankServerProtocolKnower::getAddFundsKeyFailedUseExistingKeyFirst()
{
    ServerGetAddFundsKeyMessage *getAddFundsKeyMessage = static_cast<ServerGetAddFundsKeyMessage*>(sender());
    uint uniqueRpcClientId = m_UniqueRpcClientIdsByPendingGetAddFundsKeyMessagePointer.take(getAddFundsKeyMessage);
    getAddFundsKeyMessage->fail(ServerGetAddFundsKeyMessage::FailedUseExistingKeyFirst);
    myTransmit(getAddFundsKeyMessage, uniqueRpcClientId);
    getAddFundsKeyMessage->doneWithMessage();
}
void IBankServerProtocolKnower::getAddFundsKeyFailedWaitForPendingToBeConfirmed()
{
    ServerGetAddFundsKeyMessage *getAddFundsKeyMessage = static_cast<ServerGetAddFundsKeyMessage*>(sender());
    uint uniqueRpcClientId = m_UniqueRpcClientIdsByPendingGetAddFundsKeyMessagePointer.take(getAddFundsKeyMessage);
    getAddFundsKeyMessage->fail(ServerGetAddFundsKeyMessage::FailedWaitForPendingToBeConfirmed);
    myTransmit(getAddFundsKeyMessage, uniqueRpcClientId);
    getAddFundsKeyMessage->doneWithMessage();
}
