#ifndef PENDINGBALANCEADDEDDETECTEDMESSAGEDISPENSER_H
#define PENDINGBALANCEADDEDDETECTEDMESSAGEDISPENSER_H

#include "../imessagedispenser.h"
#include "../../messages/broadcasts/pendingbalanceaddeddetectedmessage.h"

class PendingBalanceAddedDetectedMessageDispenser : public IMessageDispenser
{
protected:
    IMessage *getNewOfTypeAndConnectToDestinationObject();
};

#endif // PENDINGBALANCEADDEDDETECTEDMESSAGEDISPENSER_H
