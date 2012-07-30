#include "rpcbankserverclientbroadcastdispensers.h"

RpcBankServerClientBroadcastDispensers::RpcBankServerClientBroadcastDispensers(QObject *nonUsedDestination)
    : m_Destination(nonUsedDestination)
{ }
ClientPendingBalanceDetectedMessageDispenser *RpcBankServerClientBroadcastDispensers::takeOwnershipOfPendingBalanceDetectedMessageDispenserRiggedForDelivery(QObject *owner)
{
    m_PendingBalanceDetectedMessageDispenser = new ClientPendingBalanceDetectedMessageDispenser(m_Destination, owner);
    return m_PendingBalanceDetectedMessageDispenser;
}
ClientConfirmedBalanceDetectedMessageDispenser *RpcBankServerClientBroadcastDispensers::takeOwnershipOfConfirmedBalanceDetectedMessageDispenserRiggedForDelivery(QObject *owner)
{
    m_ConfirmedBalanceDetectedMessageDispenser = new ClientConfirmedBalanceDetectedMessageDispenser(m_Destination, owner);
    return m_ConfirmedBalanceDetectedMessageDispenser;
}
