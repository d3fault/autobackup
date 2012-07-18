#ifndef BROADCASTDISPENSERS_H
#define BROADCASTDISPENSERS_H

#include <QObject>

#include "MessagesAndDispensers/Dispensers/Broadcasts/pendingbalancedetectedmessagedispenser.h"
#include "MessagesAndDispensers/Dispensers/Broadcasts/confirmedbalancedetectedmessagedispenser.h"

class BroadcastDispensers
{
public:
    BroadcastDispensers(IAcceptMessageDeliveries *destination);
    PendingBalanceDetectedMessageDispenser *takeOwnershipOfPendingBalanceDetectedMessageDispenserRiggedForDelivery(QObject *owner);
    ConfirmedBalanceDetectedMessageDispenser *takeOwnershipOfConfirmedBalanceDetectedMessageDispenserRiggedForDelivery(QObject *owner);
    bool everyDispenserIsCreated();
private:
    IAcceptMessageDeliveries *m_Destination;
    PendingBalanceDetectedMessageDispenser *m_PendingBalanceDetectedMessageDispenser;
    ConfirmedBalanceDetectedMessageDispenser *m_ConfirmedBalanceDetectedMessageDispenser;
};

#endif // BROADCASTDISPENSERS_H
