#ifndef RPCBANKSERVERACTIONDISPENSERS_H
#define RPCBANKSERVERACTIONDISPENSERS_H

#include "../../iemitrpcbankserveractionrequestsignalswithmessageasparamandiacceptalldeliveries.h"

#include "Actions/servercreatebankaccountmessagedispenser.h"
#include "Actions/servergetaddfundskeymessagedispenser.h"

class RpcBankServerActionDispensers
{
public:
    RpcBankServerActionDispensers(IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptAllDeliveries *destination);
    ServerCreateBankAccountMessageDispenser *takeOwnershipOfCreateBankAccountMessageDispenserRiggedForDelivery(QObject *owner);
    ServerGetAddFundsKeyMessageDispenser *takeOwnershipOfGetAddFundsKeyMessageDispenserRiggedForDelivery(QObject *owner);
private:
    IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptAllDeliveries *m_Destination;

    ServerCreateBankAccountMessageDispenser *m_CreateBankAccountMessageDispenser;
    ServerGetAddFundsKeyMessageDispenser *m_GetAddFundsKeyMessageDispenser;
};

#endif // RPCBANKSERVERACTIONDISPENSERS_H
