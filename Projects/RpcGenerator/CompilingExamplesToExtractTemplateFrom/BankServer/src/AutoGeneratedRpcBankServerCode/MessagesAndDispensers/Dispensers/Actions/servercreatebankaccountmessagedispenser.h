#ifndef SERVERCREATEBANKACCOUNTMESSAGEDISPENSER_H
#define SERVERCREATEBANKACCOUNTMESSAGEDISPENSER_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/irecycleabledispenser.h"

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/createbankaccountmessage.h"
#include "../../../iemitrpcbankserveractionrequestsignalswithmessageasparamandiacceptalldeliveries.h"

class ServerCreateBankAccountMessageDispenser : public IRecycleableDispenser
{
    Q_OBJECT
public:
    ServerCreateBankAccountMessageDispenser(IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptAllDeliveries *destination, QObject *owner);
    CreateBankAccountMessage *getNewOrRecycled();
protected:
    IRecycleableAndStreamable *newOfTypeAndConnectToDestinationObjectIfApplicable();
};

#endif // SERVERCREATEBANKACCOUNTMESSAGEDISPENSER_H
