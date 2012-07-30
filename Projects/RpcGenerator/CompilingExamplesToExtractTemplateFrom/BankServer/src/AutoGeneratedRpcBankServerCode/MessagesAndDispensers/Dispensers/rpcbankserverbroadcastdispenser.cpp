#include "rpcbankserverbroadcastdispensers.h"

RpcBankServerBroadcastDispensers::RpcBankServerBroadcastDispensers(IAcceptRpcBankServerActionAndActionErrorAndBroadcastMessageDeliveries *destinatioin)
    : m_Destination(destinatioin), m_PendingBalanceDetectedMessageDispenser(0), m_ConfirmedBalanceDetectedMessageDispenser(0)
{ }
ServerPendingBalanceDetectedMessageDispenser *RpcBankServerBroadcastDispensers::takeOwnershipOfPendingBalanceDetectedMessageDispenserRiggedForDelivery(QObject *owner)
{
    m_PendingBalanceDetectedMessageDispenser = new ServerPendingBalanceDetectedMessageDispenser(static_cast<IAcceptRpcBankServerActionAndActionErrorAndBroadcastMessageDeliveries*>(m_Destination), owner);
    return m_PendingBalanceDetectedMessageDispenser;
}
ServerConfirmedBalanceDetectedMessageDispenser *RpcBankServerBroadcastDispensers::takeOwnershipOfConfirmedBalanceDetectedMessageDispenserRiggedForDelivery(QObject *owner)
{
    m_ConfirmedBalanceDetectedMessageDispenser = new ServerConfirmedBalanceDetectedMessageDispenser(static_cast<IAcceptRpcBankServerActionAndActionErrorAndBroadcastMessageDeliveries*>(m_Destination), owner);
    return m_ConfirmedBalanceDetectedMessageDispenser;
}
bool RpcBankServerBroadcastDispensers::everyDispenserIsCreated()
{
    if(!m_PendingBalanceDetectedMessageDispenser)
        return false;
    if(!m_ConfirmedBalanceDetectedMessageDispenser)
        return false;

    return true;
}
