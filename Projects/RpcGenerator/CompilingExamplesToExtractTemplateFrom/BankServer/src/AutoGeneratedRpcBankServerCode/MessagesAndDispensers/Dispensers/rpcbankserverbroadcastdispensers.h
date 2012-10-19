#ifndef RPCBANKSERVERBROADCASTDISPENSERS_H
#define RPCBANKSERVERBROADCASTDISPENSERS_H

#include "../../iemitrpcbankserveractionrequestsignalswithmessageasparamandiacceptalldeliveries.h"

#include "Broadcasts/serverpendingbalancedetectedmessagedispenser.h"
#include "Broadcasts/serverconfirmedbalancedetectedmessagedispenser.h"

class RpcBankServerBroadcastDispensers //client one doesn't have a destination object of any kind, but still uses takeOwnership to handle recycling
{
public:
    RpcBankServerBroadcastDispensers(IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptAllDeliveries *destination);
    ServerPendingBalanceDetectedMessageDispenser *takeOwnershipOfPendingBalanceDetectedMessageDispenserRiggedForDelivery(QObject *owner);
    ServerConfirmedBalanceDetectedMessageDispenser *takeOwnershipOfConfirmedBalanceDetectedMessageDispenserRiggedForDelivery(QObject *owner);

    bool everyDispenserIsCreated();
private:
    QObject *m_Destination;

    ServerPendingBalanceDetectedMessageDispenser *m_PendingBalanceDetectedMessageDispenser;
    ServerConfirmedBalanceDetectedMessageDispenser *m_ConfirmedBalanceDetectedMessageDispenser;
};

#endif // RPCBANKSERVERBROADCASTDISPENSERS_H
