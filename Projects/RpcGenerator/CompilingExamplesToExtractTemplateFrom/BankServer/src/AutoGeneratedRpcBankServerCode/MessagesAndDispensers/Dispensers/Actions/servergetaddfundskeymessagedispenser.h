#ifndef SERVERGETADDFUNDSKEYMESSAGEDISPENSER_H
#define SERVERGETADDFUNDSKEYMESSAGEDISPENSER_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/irecycleabledispenser.h"

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/getaddfundskeymessage.h"
#include "../../../iemitrpcbankserveractionrequestsignalswithmessageasparamandiacceptalldeliveries.h"

class ServerGetAddFundsKeyMessageDispenser : public IRecycleableDispenser
{
    Q_OBJECT
public:
    ServerGetAddFundsKeyMessageDispenser(IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptActionDeliveries *destination, QObject *owner);
    GetAddFundsKeyMessage *getNewOrRecycled();
protected:
    IRecycleableAndStreamable *newOfTypeAndConnectToDestinationObjectIfApplicable();
};

#endif // SERVERGETADDFUNDSKEYMESSAGEDISPENSER_H
