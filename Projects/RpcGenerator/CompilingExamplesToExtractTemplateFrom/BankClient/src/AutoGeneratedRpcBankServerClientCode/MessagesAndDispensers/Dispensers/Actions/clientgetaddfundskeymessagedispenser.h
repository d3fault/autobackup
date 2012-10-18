#ifndef CLIENTGETADDFUNDSKEYMESSAGEDISPENSER_H
#define CLIENTGETADDFUNDSKEYMESSAGEDISPENSER_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/irecycleabledispenser.h"

#include "../../Messages/Actions/clientgetaddfundskeymessage.h"
#include "../../../../../../RpcBankServerAndClientShared/iacceptrpcbankserveractionnonerrordeliveries.h"

class ClientGetAddFundsKeyMessageDispenser : public IRecycleableDispenser
{
    Q_OBJECT
public:
    explicit ClientGetAddFundsKeyMessageDispenser(IAcceptRpcBankServerActionDeliveries *destination, QObject *owner);
    ClientGetAddFundsKeyMessage *getNewOrRecycled();
protected:
    IRecycleableAndStreamable *newOfTypeAndConnectToDestinationObjectIfApplicable();
};

#endif // CLIENTGETADDFUNDSKEYMESSAGEDISPENSER_H
