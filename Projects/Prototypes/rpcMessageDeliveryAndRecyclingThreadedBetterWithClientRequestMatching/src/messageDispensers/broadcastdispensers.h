#ifndef BROADCASTDISPENSERS_H
#define BROADCASTDISPENSERS_H

#include "broadcasts/pendingbalanceaddedmessagedispenser.h"

class BroadcastDispensers
{
public:
    BroadcastDispensers();
    void setPendingBalanceAddedMessageDispenser(PendingBalanceAddedMessageDispenser *pendingBalancedAddedMessageDispenser);
    PendingBalanceAddedMessageDispenser *pendingBalanceAddedMessageDispenser();
    //etc

    bool everyDispenserIsCreated();
private:
    PendingBalanceAddedMessageDispenser *m_PendingBalanceAddedMessageDispenser;
};

#endif // BROADCASTDISPENSERS_H
