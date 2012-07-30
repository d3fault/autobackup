#ifndef PENDINGBALANCEDETECTEDMESSAGEDISPENSER_H
#define PENDINGBALANCEDETECTEDMESSAGEDISPENSER_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/irecycleabledispenser.h"

#include "../../Messages/Broadcasts/serverpendingbalancedetectedmessage.h"

#include "../../../iacceptrpcbankserveractionandactionerrorandbroadcastmessagedeliveries.h"

class ServerPendingBalanceDetectedMessageDispenser : public IRecycleableDispenser
{
    Q_OBJECT
public:
    ServerPendingBalanceDetectedMessageDispenser(IAcceptRpcBankServerActionAndActionErrorAndBroadcastMessageDeliveries *destination, QObject *owner);
    ServerPendingBalanceDetectedMessage *getNewOrRecycled();
protected:
    IRecycleableAndStreamable *newOfTypeAndConnectToDestinationObjectIfApplicable();
};

#endif // PENDINGBALANCEDETECTEDMESSAGEDISPENSER_H
