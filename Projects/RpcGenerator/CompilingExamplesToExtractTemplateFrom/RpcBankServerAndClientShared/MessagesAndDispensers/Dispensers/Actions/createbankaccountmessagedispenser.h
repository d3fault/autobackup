#ifndef CREATEBANKACCOUNTMESSAGEDISPENSER_H
#define CREATEBANKACCOUNTMESSAGEDISPENSER_H

#include "../imessagedispenser.h"

#include "../../Messages/Actions/createbankaccountmessage.h"
#include "../../../iacceptrpcbankservermessagedeliveries.h"

class CreateBankAccountMessageDispenser : public IMessageDispenser
{
public:
    CreateBankAccountMessageDispenser(QObject *destination, QObject *owner);
    CreateBankAccountMessage *getNewOrRecycled();
protected:
    IMessage *getNewOfTypeAndConnectToDestinationObject();
};

#endif // CREATEBANKACCOUNTMESSAGEDISPENSER_H
