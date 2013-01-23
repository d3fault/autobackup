#ifndef PENDINGBALANCEDETECTEDMESSAGEDISPENSER_H
#define PENDINGBALANCEDETECTEDMESSAGEDISPENSER_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/irecycleabledispenser.h"

#include "../../Messages/Broadcasts/serverpendingbalancedetectedmessage.h"

#include "../../../iemitrpcbankserveractionrequestsignalswithmessageasparamandiacceptalldeliveries.h"

class ServerPendingBalanceDetectedMessageDispenser : public IRecycleableDispenser
{
    Q_OBJECT
public:
    ServerPendingBalanceDetectedMessageDispenser(IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptActionDeliveries *destination, QObject *owner);
    ServerPendingBalanceDetectedMessage *getNewOrRecycled();
protected:
    IRecycleableAndStreamable *newOfTypeAndConnectToDestinationObjectIfApplicable();
};

#endif // PENDINGBALANCEDETECTEDMESSAGEDISPENSER_H
