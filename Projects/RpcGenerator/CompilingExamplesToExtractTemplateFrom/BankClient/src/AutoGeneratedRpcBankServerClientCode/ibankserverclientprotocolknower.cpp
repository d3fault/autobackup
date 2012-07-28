#include "ibankserverclientprotocolknower.h"

IBankServerClientProtocolKnower::IBankServerClientProtocolKnower(QObject *parent) :
    IEmitRpcBankServerBroadcastAndActionResponseSignalsWithMessageAsParam(parent)
{ }
void IBankServerClientProtocolKnower::setBroadcastDispensers(RpcBankServerBroadcastDispensers *rpcBankServerBroadcastDispensers)
{
    m_RpcBankServerBroadcastDispensers = rpcBankServerBroadcastDispensers;
}
void IBankServerClientProtocolKnower::takeOwnershipOfBroadcastsAndSetupDelivery()
{
    m_PendingBalanceDetectedMessageDispenser = m_RpcBankServerBroadcastDispensers->takeOwnershipOfPendingBalanceDetectedMessageDispenserRiggedForDelivery(this);
    m_ConfirmedBalanceDetectedMessageDispenser = m_RpcBankServerBroadcastDispensers->takeOwnershipOfConfirmedBalanceDetectedMessageDispenserRiggedForDelivery(this);
}
void IBankServerClientProtocolKnower::processCreateBankAccountResponseReceived(CreateBankAccountMessage *createBankAccountMessage)
{
    int index = m_PendingCreateBankAccountMessagePointers.indexOf(createBankAccountMessage);
    if(index > -1)
    {
        m_PendingCreateBankAccountMessagePointers.removeAt(index);
        //TODOreq: errors etc?
        emit createBankAccountCompleted(createBankAccountMessage);
    }
    else
    {
        //TODOreq: deal with error where for some reason the response was never requested???
    }
}
void IBankServerClientProtocolKnower::processGetAddFundsKeyResponseReceived(GetAddFundsKeyMessage *getAddFundsKeyMessage)
{
    int index = m_PendingGetAddFundsKeyMessagePointers.indexOf(getAddFundsKeyMessage);
    if(index > -1)
    {
        m_PendingGetAddFundsKeyMessagePointers.removeAt(index);
        emit getAddFundsKeyCompleted(getAddFundsKeyMessage);
    }
    else
    {

    }
}
void IBankServerClientProtocolKnower::createBankAccountDelivery()
{
    //TODOreq: checking conflicting pending
    m_PendingCreateBankAccountMessagePointers.append(createBankAccountMessage);
    myTransmit(createBankAccountMessage);
    createBankAccountMessage->doneWithMessage();
}
void IBankServerClientProtocolKnower::getAddFundsKeyDelivery()
{
    m_PendingGetAddFundsKeyMessagePointers.append(getAddFundsKeyMessage);
    myTransmit(getAddFundsKeyMessage);
    getAddFundsKeyMessage->doneWithMessage();
}
