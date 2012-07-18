#ifndef PENDINGBALANCEDETECTEDMESSAGEDISPENSER_H
#define PENDINGBALANCEDETECTEDMESSAGEDISPENSER_H

#include "../imessagedispenser.h"

#include "../../Messages/Broadcasts/pendingbalancedetectedmessage.h"

#include "../../../iacceptrpcbankservermessagedeliveries.h"
#include "../../../iacceptmessagedeliveriesgoingtorpcbankclient.h"

class PendingBalanceDetectedMessageDispenser : public IMessageDispenser
{
public:
    PendingBalanceDetectedMessageDispenser(IAcceptRpcBankServerMessageDeliveries *destination, QObject *owner);
    PendingBalanceAddedMessage *getNewOrRecycled();
protected:
    void getNewOfTypeAndConnectToDestinationObject();
};

#endif // PENDINGBALANCEDETECTEDMESSAGEDISPENSER_H
