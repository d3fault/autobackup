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
void IBankServerProtocolKnower::processCreateBankAccountMessage(CreateBankAccountMessage *createBankAccountMessage, uint uniqueRpcClientId)
{
    m_UniqueRpcClientIdsByCreateBankAccountMessagePointer.insert(createBankAccountMessage, uniqueRpcClientId);

    //TODOreq: perhaps check the list already by value to see if we're already working on the same request?
    //if we are, it still might fail and then we should process this one. so i need to atomically access a list in that case, a list of "pendingCollissionsAwaitingFinalizatioin"
    //the list is mutex-grabbed+cleared AFTER we transmit the message back to
    //actually this idea sucks
    //maybe we can instead connect the clearing of the list to the chosen/conflicting/got-their-first recycle signal after it's finished? so no additional processing needs to be done

    emit createBankAccount(createBankAccountMessage);
}
void IBankServerProtocolKnower::processGetAddFundsKeyMessage(GetAddFundsKeyMessage *getAddFundsKeyMessage, uint uniqueRpcClientId)
{
    m_UniqueRpcClientIdsByGetAddFundsKeyMessagePointer.insert(getAddFundsKeyMessage, uniqueRpcClientId);
    emit getAddFundsKey(getAddFundsKeyMessage);
}
void IBankServerProtocolKnower::createBankAccountDelivery()
{
    //perhaps here is best place to keep track of pendings? because after this spot they have all converged and i'd have to do a switch or something (not necessarily if i increaes the size of each message by keeping a pointer to it's pending list. bad idea. better to do it here)

    //so these transmits right now make it APPEAR i should just connect every .deliverSignal() to a single slot... but for keeping track of pending messages (which i have yet to do), that will turn out not to be the case

    CreateBankAccountMessage *createBankAccountMessage = static_cast<CreateBankAccountMessage*>(sender());
    uint uniqueRpcClientId = m_UniqueRpcClientIdsByCreateBankAccountMessagePointer.take(createBankAccountMessage);
    myTransmit(createBankAccountMessage, uniqueRpcClientId);
    createBankAccountMessage->doneWithMessage();

    //TODOreq: unsure of ordering of transmit and removing from pending hash. probably doesn't matter? no fucking clue tbh. changed it from .remove to .value and then to .take -- i guess i don't have a choice on the ordering? i think it only matters that the backend impl has in fact finished with the pending. so even if one does get through, the backend impl will catch it definitely
}
void IBankServerProtocolKnower::getAddFundsKeyDelivery()
{
    GetAddFundsKeyMessage *getAddFundsKeyMessage = static_cast<GetAddFundsKeyMessage*>(sender());
    uint uniqueRpcClientId = m_UniqueRpcClientIdsByGetAddFundsKeyMessagePointer.take(getAddFundsKeyMessage);
    myTransmit(getAddFundsKeyMessage, uniqueRpcClientId);
    getAddFundsKeyMessage->doneWithMessage();
}
void IBankServerProtocolKnower::pendingBalanceDetectedDelivery()
{
    PendingBalanceDetectedMessage *pendingBalanceDetectedMessage = static_cast<PendingBalanceDetectedMessage*>(sender());
    myTransmit(pendingBalanceDetectedMessage, 0);
    //there is no pending queue because we are not 'responding' to anyone
    pendingBalanceDetectedMessage->doneWithMessage();
}
void IBankServerProtocolKnower::confirmedBalanceDetectedDelivery()
{
    ConfirmedBalanceDetectedMessage *confirmedBalanceDetectedMessage = static_cast<ConfirmedBalanceDetectedMessage*>(sender());
    myTransmit(confirmedBalanceDetectedMessage, 0);
    confirmedBalanceDetectedMessage->doneWithMessage();
}
void IBankServerProtocolKnower::createBankAccountFailedUsernameAlreadyExists()
{
    //TODOreq: handle errors, perhaps a bool Success at IMessage level and an int reserved for failed enum (only transmitted if that bool Success is false)
    //unsure and have yet to come up with a design for errors
}
void IBankServerProtocolKnower::createBankAccountFailedPersistError()
{
}
void IBankServerProtocolKnower::getAddFundsKeyFailedUseExistingKeyFirst()
{
}
void IBankServerProtocolKnower::getAddFundsKeyFailedWaitUntilConfirmed()
{
}
