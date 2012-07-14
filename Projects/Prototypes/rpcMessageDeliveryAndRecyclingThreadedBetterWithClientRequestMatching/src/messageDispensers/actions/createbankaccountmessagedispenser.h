#ifndef CREATEBANKACCOUNTMESSAGEDISPENSER_H
#define CREATEBANKACCOUNTMESSAGEDISPENSER_H

#include "../imessagedispenser.h"
#include "../../messages/actions/createbankaccountmessage.h"

class CreateBankAccountMessageDispenser : public IMessageDispenser
{
public:
    explicit CreateBankAccountMessageDispenser(QObject *mandatoryParent);
    CreateBankAccountMessage *getNewOrRecycled();
protected:
    IMessage *getNewOfTypeAndConnectToDestinationObject();
};

#endif // CREATEBANKACCOUNTMESSAGEDISPENSER_H
