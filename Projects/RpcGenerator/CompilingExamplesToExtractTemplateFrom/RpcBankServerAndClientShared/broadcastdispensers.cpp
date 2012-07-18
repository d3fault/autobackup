#include "broadcastdispensers.h"

BroadcastDispensers::BroadcastDispensers(IAcceptMessageDeliveries *destinatioin)
    : m_Destination(destinatioin), m_PendingBalanceDetectedMessageDispenser(0), m_ConfirmedBalanceDetectedMessageDispenser(0)
{ }
PendingBalanceDetectedMessageDispenser *BroadcastDispensers::takeOwnershipOfPendingBalanceDetectedMessageDispenserRiggedForDelivery(QObject *owner)
{
    m_PendingBalanceDetectedMessageDispenser = new PendingBalanceDetectedMessageDispenser(m_Destination, owner);
    return m_PendingBalanceDetectedMessageDispenser;
}
ConfirmedBalanceDetectedMessageDispenser *BroadcastDispensers::takeOwnershipOfConfirmedBalanceDetectedMessageDispenserRiggedForDelivery(QObject *owner)
{
    m_ConfirmedBalanceDetectedMessageDispenser = new ConfirmedBalanceDetectedMessageDispenser(m_Destination, owner);
    return m_ConfirmedBalanceDetectedMessageDispenser;
}
bool BroadcastDispensers::everyDispenserIsCreated()
{
    if(!m_PendingBalanceDetectedMessageDispenser)
        return false;
    if(!m_ConfirmedBalanceDetectedMessageDispenser)
        return false;

    return true;
}
