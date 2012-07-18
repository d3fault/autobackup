#include "rpcbankserverbroadcastdispensers.h"

RpcBankServerBroadcastDispensers::RpcBankServerBroadcastDispensers(IAcceptMessageDeliveries *destinatioin)
    : m_Destination(destinatioin), m_PendingBalanceDetectedMessageDispenser(0), m_ConfirmedBalanceDetectedMessageDispenser(0)
{ }
PendingBalanceDetectedMessageDispenser *RpcBankServerBroadcastDispensers::takeOwnershipOfPendingBalanceDetectedMessageDispenserRiggedForDelivery(QObject *owner)
{
    m_PendingBalanceDetectedMessageDispenser = new PendingBalanceDetectedMessageDispenser(m_Destination, owner);
    return m_PendingBalanceDetectedMessageDispenser;
}
ConfirmedBalanceDetectedMessageDispenser *RpcBankServerBroadcastDispensers::takeOwnershipOfConfirmedBalanceDetectedMessageDispenserRiggedForDelivery(QObject *owner)
{
    m_ConfirmedBalanceDetectedMessageDispenser = new ConfirmedBalanceDetectedMessageDispenser(m_Destination, owner);
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
