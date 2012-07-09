#ifndef PENDINGBALANCEDADDEDMESSAGEDISPENSER_H
#define PENDINGBALANCEDADDEDMESSAGEDISPENSER_H

#include "../imessagedispenser.h"
#include "../../messages/broadcasts/pendingbalanceaddedmessage.h"

class PendingBalanceAddedMessageDispenser : public IMessageDispenser
{
public:
    explicit PendingBalanceAddedMessageDispenser(QObject *mandatoryParent);
    PendingBalanceAddedMessage *getNewOrRecycled();
protected:
    IMessage *getNewOfTypeAndConnectToDestinationObject();
};

#endif // PENDINGBALANCEDADDEDMESSAGEDISPENSER_H
