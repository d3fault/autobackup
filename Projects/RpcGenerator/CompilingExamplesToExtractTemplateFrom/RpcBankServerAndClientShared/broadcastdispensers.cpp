#include "broadcastdispensers.h"

BroadcastDispensers::BroadcastDispensers(IAcceptMessageDeliveries *destinatioin)
    : m_Destination(destinatioin), m_PendingBalanceDetectedMessageDispenser(0)
{
}
PendingBalanceDetectedMessageDispenser *BroadcastDispensers::takeOwnershipOfPendingBalanceDetectedMessageDispenserRiggedForDelivery(QObject *owner)
{
    m_PendingBalanceDetectedMessageDispenser = new PendingBalanceDetectedMessageDispenser(m_Destination, owner);
}
bool BroadcastDispensers::everyDispenserIsCreated()
{
    if(!m_PendingBalanceDetectedMessageDispenser)
        return false;

    return true;
}
