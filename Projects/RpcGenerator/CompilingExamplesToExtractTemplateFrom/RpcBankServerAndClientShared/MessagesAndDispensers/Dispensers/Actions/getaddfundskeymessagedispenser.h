#ifndef GETADDFUNDSKEYMESSAGEDISPENSER_H
#define GETADDFUNDSKEYMESSAGEDISPENSER_H

#include "../imessagedispenser.h"

#include "../../Messages/Actions/getaddfundskeymessage.h"

#include "../../../iacceptrpcbankservermessagedeliveries.h"
#include "../../../iacceptmessagedeliveriesgoingtorpcbankserver.h"

class GetAddFundsKeyMessageDispenser : public IMessageDispenser
{
public:
    GetAddFundsKeyMessageDispenser(IAcceptRpcBankServerMessageDeliveries *destination, QObject *owner);
    GetAddFundsKeyMessage *getNewOrRecycled();
protected:
    void getNewOfTypeAndConnectToDestinationObject();
};

#endif // GETADDFUNDSKEYMESSAGEDISPENSER_H
