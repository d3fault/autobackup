#ifndef RPCBANKSERVERBROADCASTDISPENSERS_H
#define RPCBANKSERVERBROADCASTDISPENSERS_H

#include <QMutex>
#include <QMutexLocker>

#include "../../iemitrpcbankserveractionrequestsignalswithmessageasparamandiacceptalldeliveries.h"

#include "Broadcasts/serverpendingbalancedetectedmessagedispenser.h"
#include "Broadcasts/serverconfirmedbalancedetectedmessagedispenser.h"

//ThreadSafe to take ownership and to check if they're all created
class RpcBankServerBroadcastDispensers //client one doesn't have a destination object of any kind, but still uses takeOwnership to handle recycling
{
public:
    RpcBankServerBroadcastDispensers(IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptActionDeliveries *destination);
    ServerPendingBalanceDetectedMessageDispenser *takeOwnershipOfPendingBalanceDetectedMessageDispenserRiggedForDelivery(QObject *owner);
    ServerConfirmedBalanceDetectedMessageDispenser *takeOwnershipOfConfirmedBalanceDetectedMessageDispenserRiggedForDelivery(QObject *owner);

    bool everyDispenserIsCreated();
private:
    QObject *m_Destination;
    QMutex m_CreateAndCheckCreatedMutex;

    ServerPendingBalanceDetectedMessageDispenser *m_PendingBalanceDetectedMessageDispenser;
    ServerConfirmedBalanceDetectedMessageDispenser *m_ConfirmedBalanceDetectedMessageDispenser;
};

#endif // RPCBANKSERVERBROADCASTDISPENSERS_H
