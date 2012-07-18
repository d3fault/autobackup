#ifndef CREATEBANKACCOUNTMESSAGEDISPENSER_H
#define CREATEBANKACCOUNTMESSAGEDISPENSER_H

#include "../imessagedispenser.h"

#include "../../Messages/Actions/createbankaccountmessage.h"

class CreateBankAccountMessageDispenser : public IMessageDispenser
{
public:
    CreateBankAccountMessageDispenser(QObject *destination, QObject *owner);
    CreateBankAccountMessage *getNewOrRecycled();
protected:
    void getNewOfTypeAndConnectToDestinationObject();
};

#endif // CREATEBANKACCOUNTMESSAGEDISPENSER_H
