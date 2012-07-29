#ifndef PENDINGBALANCEDETECTEDMESSAGEDISPENSER_H
#define PENDINGBALANCEDETECTEDMESSAGEDISPENSER_H

#include "../imessagedispenser.h"

#include "../../Messages/Broadcasts/pendingbalancedetectedmessage.h"

#include "../../../iacceptrpcbankservermessagedeliveries.h"

class PendingBalanceDetectedMessageDispenser : public IMessageDispenser
{
public:
    PendingBalanceDetectedMessageDispenser(QObject *destination, QObject *owner);
    PendingBalanceDetectedMessage *getNewOrRecycled();
protected:
    IMessage *getNewOfTypeAndConnectToDestinationObject();
};

#endif // PENDINGBALANCEDETECTEDMESSAGEDISPENSER_H
