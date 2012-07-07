#ifndef BROADCASTDISPENSERS_H
#define BROADCASTDISPENSERS_H

#include "broadcasts/pendingbalancedaddedmessagedispenser.h"

class BroadcastDispensers
{
public:
    BroadcastDispensers();
    void setPendingBalancedAddedMessageDispenser(PendingBalancedAddedMessageDispenser *pendingBalancedAddedMessageDispenser);
    PendingBalancedAddedMessageDispenser *pendingBalancedAddedMessageDispenser();
    bool everyDispenserIsCreated();
private:
    PendingBalancedAddedMessageDispenser *m_PendingBalancedAddedMessageDispenser;
};

#endif // BROADCASTDISPENSERS_H
