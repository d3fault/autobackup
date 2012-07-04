#ifndef PENDINGBALANCEADDEDDETECTEDMESSAGEDISPENSER_H
#define PENDINGBALANCEADDEDDETECTEDMESSAGEDISPENSER_H

#include "../../irpcclientshelper.h"
#include "../imessagedispenser.h"
#include "../../messages/broadcasts/pendingbalanceaddeddetectedmessage.h"

class PendingBalanceAddedDetectedMessageDispenser : public IMessageDispenser
{
public:
    PendingBalanceAddedDetectedMessageDispenser(IRpcClientsHelper *destinationObject)
        : IMessageDispenser(destinationObject)
    { }
    PendingBalanceAddedDetectedMessage *getNewOrRecycled() { return static_cast<PendingBalanceAddedDetectedMessage*>(privateGetNewOrRecycled()); }
protected:
    IMessage *getNewOfTypeAndConnectToDestinationObject();
};

#endif // PENDINGBALANCEADDEDDETECTEDMESSAGEDISPENSER_H
