#ifndef RPCBANKSERVERBROADCASTDISPENSERS_H
#define RPCBANKSERVERBROADCASTDISPENSERS_H

#include <QObject>

#include "ibankserverprotocolknower.h"
#include "MessagesAndDispensers/Dispensers/Broadcasts/pendingbalancedetectedmessagedispenser.h"
#include "MessagesAndDispensers/Dispensers/Broadcasts/confirmedbalancedetectedmessagedispenser.h"

class RpcBankServerBroadcastDispensers //client one doesn't have a destination object of any kind, but still uses takeOwnership to handle recycling
{
public:
    RpcBankServerBroadcastDispensers(IAcceptMessageDeliveries *destination);
    PendingBalanceDetectedMessageDispenser *takeOwnershipOfPendingBalanceDetectedMessageDispenserRiggedForDelivery(QObject *owner);
    ConfirmedBalanceDetectedMessageDispenser *takeOwnershipOfConfirmedBalanceDetectedMessageDispenserRiggedForDelivery(QObject *owner);
    bool everyDispenserIsCreated();
private:
    IAcceptMessageDeliveries *m_Destination;
    PendingBalanceDetectedMessageDispenser *m_PendingBalanceDetectedMessageDispenser;
    ConfirmedBalanceDetectedMessageDispenser *m_ConfirmedBalanceDetectedMessageDispenser;
};

#endif // RPCBANKSERVERBROADCASTDISPENSERS_H
