#ifndef CLIENTGETADDFUNDSKEYMESSAGEDISPENSER_H
#define CLIENTGETADDFUNDSKEYMESSAGEDISPENSER_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/irecycleabledispenser.h"

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/getaddfundskeymessage.h"
#include "../../../../../../RpcBankServerAndClientShared/iacceptrpcbankserveractiondeliveries.h"

class ClientGetAddFundsKeyMessageDispenser : public IRecycleableDispenser
{
    Q_OBJECT
public:
    explicit ClientGetAddFundsKeyMessageDispenser(IAcceptRpcBankServerActionDeliveries *destination, QObject *owner);
    GetAddFundsKeyMessage *getNewOrRecycled();
protected:
    IRecycleableAndStreamable *newOfTypeAndConnectToDestinationObjectIfApplicable();
};

#endif // CLIENTGETADDFUNDSKEYMESSAGEDISPENSER_H
