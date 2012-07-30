#ifndef SERVERCREATEBANKACCOUNTMESSAGEDISPENSER_H
#define SERVERCREATEBANKACCOUNTMESSAGEDISPENSER_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/irecycleabledispenser.h"

#include "../../Messages/Actions/servercreatebankaccountmessage.h"
#include "../../../iacceptrpcbankserveractionandactionerrorandbroadcastmessagedeliveries.h"

class ServerCreateBankAccountMessageDispenser : public IRecycleableDispenser
{
    Q_OBJECT
public:
    ServerCreateBankAccountMessageDispenser(IAcceptRpcBankServerActionAndActionErrorAndBroadcastMessageDeliveries *destination, QObject *owner);
    ServerCreateBankAccountMessage *getNewOrRecycled();
protected:
    IRecycleableAndStreamable *newOfTypeAndConnectToDestinationObjectIfApplicable();
};

#endif // SERVERCREATEBANKACCOUNTMESSAGEDISPENSER_H
