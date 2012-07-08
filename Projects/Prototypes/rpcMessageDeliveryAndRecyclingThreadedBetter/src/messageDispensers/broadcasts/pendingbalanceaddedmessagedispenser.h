#ifndef PENDINGBALANCEDADDEDMESSAGEDISPENSER_H
#define PENDINGBALANCEDADDEDMESSAGEDISPENSER_H

#include "../imessagedispenser.h"

class PendingBalanceAddedMessageDispenser : public IMessageDispenser
{
public:
    PendingBalanceAddedMessageDispenser(QObject *mandatoryParent);
};

#endif // PENDINGBALANCEDADDEDMESSAGEDISPENSER_H
