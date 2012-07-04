#ifndef PENDINGBALANCEADDEDDETECTEDMESSAGEDISPENSER_H
#define PENDINGBALANCEADDEDDETECTEDMESSAGEDISPENSER_H

#include "../../irpcclientshelper.h"
#include "../imessagedispenser.h"

class PendingBalanceAddedDetectedMessage;

class PendingBalanceAddedDetectedMessageDispenser : public IMessageDispenser
{
public:
    PendingBalanceAddedDetectedMessageDispenser(IRpcClientsHelper *destinationObject)
        : IMessageDispenser(destinationObject)
    { }
    PendingBalanceAddedDetectedMessage *getNewOrRecycled();
protected:
    IMessage *getNewOfTypeAndConnectToDestinationObject();
};

#endif // PENDINGBALANCEADDEDDETECTEDMESSAGEDISPENSER_H
