#include "broadcastdispensers.h"

BroadcastDispensers::BroadcastDispensers()
    : m_PendingBalanceAddedMessageDispenser(0) //etc
{ }
void BroadcastDispensers::setPendingBalanceAddedMessageDispenser(PendingBalanceAddedMessageDispenser *pendingBalancedAddedMessageDispenser)
{
    m_PendingBalanceAddedMessageDispenser = pendingBalancedAddedMessageDispenser;
}
PendingBalanceAddedMessageDispenser * BroadcastDispensers::pendingBalanceAddedMessageDispenser()
{
    return m_PendingBalanceAddedMessageDispenser;
}
bool BroadcastDispensers::everyDispenserIsCreated()
{
    if(!m_PendingBalanceAddedMessageDispenser)
        return false;

    //etc for every broadcast dispenser

    //then:
    return true;
}


