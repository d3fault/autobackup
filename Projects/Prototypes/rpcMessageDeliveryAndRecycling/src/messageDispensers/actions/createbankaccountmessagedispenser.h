#ifndef CREATEBANKACCOUNTMESSAGEDISPENSER_H
#define CREATEBANKACCOUNTMESSAGEDISPENSER_H

#include "../../irpcclientshelper.h"
#include "../imessagedispenser.h"
#include "../../messages/actions/createbankaccountmessage.h"

class CreateBankAccountMessageDispenser : public IMessageDispenser
{
public:
    CreateBankAccountMessageDispenser(IRpcClientsHelper *destinationObject)
        : IMessageDispenser(destinationObject)
    { }
    CreateBankAccountMessage *getNewOrRecycled() { return static_cast<CreateBankAccountMessage*>(privateGetNewOrRecycled()); }
protected:
    IMessage *getNewOfTypeAndConnectToDestinationObject();
};

#endif // CREATEBANKACCOUNTMESSAGEDISPENSER_H
