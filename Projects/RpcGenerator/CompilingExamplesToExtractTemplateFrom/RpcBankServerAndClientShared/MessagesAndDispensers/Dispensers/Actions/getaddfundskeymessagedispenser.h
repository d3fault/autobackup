#ifndef GETADDFUNDSKEYMESSAGEDISPENSER_H
#define GETADDFUNDSKEYMESSAGEDISPENSER_H

#include "../imessagedispenser.h"

#include "../../Messages/Actions/getaddfundskeymessage.h"

class GetAddFundsKeyMessageDispenser : public IMessageDispenser
{
public:
    GetAddFundsKeyMessageDispenser(QObject *destination, QObject *owner);
    GetAddFundsKeyMessage *getNewOrRecycled();
protected:
    IMessage *getNewOfTypeAndConnectToDestinationObject();
};

#endif // GETADDFUNDSKEYMESSAGEDISPENSER_H
