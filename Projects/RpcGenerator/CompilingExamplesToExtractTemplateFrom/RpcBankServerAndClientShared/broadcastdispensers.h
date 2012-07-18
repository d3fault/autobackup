#ifndef BROADCASTDISPENSERS_H
#define BROADCASTDISPENSERS_H

#include <QObject>

#include "MessagesAndDispensers/Dispensers/Broadcasts/pendingbalancedetectedmessagedispenser.h"

class BroadcastDispensers
{
public:
    BroadcastDispensers(IAcceptMessageDeliveries *destination);
    PendingBalanceDetectedMessageDispenser *takeOwnershipOfPendingBalanceDetectedMessageDispenserRiggedForDelivery(QObject *owner);
    bool everyDispenserIsCreated();
private:
    IAcceptMessageDeliveries *m_Destination;
    PendingBalanceDetectedMessageDispenser *m_PendingBalanceDetectedMessageDispenser;
};

#endif // BROADCASTDISPENSERS_H
