#include "broadcastdispensers.h"

BroadcastDispensers::BroadcastDispensers()
    : m_PendingBalancedAddedMessageDispenser(0)
{ }
void BroadcastDispensers::setPendingBalancedAddedMessageDispenser(PendingBalancedAddedMessageDispenser *pendingBalancedAddedMessageDispenser)
{
    m_PendingBalancedAddedMessageDispenser = pendingBalancedAddedMessageDispenser;
}
PendingBalancedAddedMessageDispenser * BroadcastDispensers::pendingBalancedAddedMessageDispenser()
{
    return m_PendingBalancedAddedMessageDispenser;
}
bool BroadcastDispensers::everyDispenserIsCreated()
{
    //TODOreq: check this bool/method somewhere in init/construction (idk where)

    if(!m_PendingBalancedAddedMessageDispenser)
        return false;

    //etc for every broadcast dispenser

    //then:
    return true;
}


