#ifndef SERVERGETADDFUNDSKEYMESSAGEDISPENSER_H
#define SERVERGETADDFUNDSKEYMESSAGEDISPENSER_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/irecycleabledispenser.h"

#include "../../Messages/Actions/servergetaddfundskeymessage.h"
#include "../../../iacceptrpcbankserveractionandactionerrorandbroadcastmessagedeliveries.h"

class ServerGetAddFundsKeyMessageDispenser : public IRecycleableDispenser
{
    Q_OBJECT
public:
    ServerGetAddFundsKeyMessageDispenser(IAcceptRpcBankServerActionAndActionErrorAndBroadcastMessageDeliveries *destination, QObject *owner);
    ServerGetAddFundsKeyMessage *getNewOrRecycled();
protected:
    IRecycleableAndStreamable *newOfTypeAndConnectToDestinationObjectIfApplicable();
};

#endif // SERVERGETADDFUNDSKEYMESSAGEDISPENSER_H
