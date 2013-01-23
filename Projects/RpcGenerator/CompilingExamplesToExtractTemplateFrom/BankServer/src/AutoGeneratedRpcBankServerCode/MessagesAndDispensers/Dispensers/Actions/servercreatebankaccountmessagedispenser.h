#ifndef SERVERCREATEBANKACCOUNTMESSAGEDISPENSER_H
#define SERVERCREATEBANKACCOUNTMESSAGEDISPENSER_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/irecycleabledispenser.h"

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/createbankaccountmessage.h"
#include "../../../iemitrpcbankserveractionrequestsignalswithmessageasparamandiacceptactiondeliveries.h"

class ServerCreateBankAccountMessageDispenser : public IRecycleableDispenser
{
    Q_OBJECT
public:
    ServerCreateBankAccountMessageDispenser(IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptActionDeliveries *destination, QObject *owner);
    CreateBankAccountMessage *getNewOrRecycled();
protected:
    IRecycleableAndStreamable *newOfTypeAndConnectToDestinationObjectIfApplicable();
};

#endif // SERVERCREATEBANKACCOUNTMESSAGEDISPENSER_H
