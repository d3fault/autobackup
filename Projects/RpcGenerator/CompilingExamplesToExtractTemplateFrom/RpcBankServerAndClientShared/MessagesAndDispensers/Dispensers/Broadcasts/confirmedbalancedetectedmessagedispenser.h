#ifndef CONFIRMEDBALANCEDETECTEDMESSAGEDISPENSER_H
#define CONFIRMEDBALANCEDETECTEDMESSAGEDISPENSER_H

#include "../imessagedispenser.h"

#include "../../Messages/Broadcasts/confirmedbalancedetectedmessage.h"

#include "../../../iacceptrpcbankservermessagedeliveries.h"
#include "../../../iacceptmessagedeliveriesgoingtorpcbankclient.h"

class ConfirmedBalanceDetectedMessageDispenser : public IMessageDispenser
{
public:
    ConfirmedBalanceDetectedMessageDispenser(IAcceptRpcBankServerMessageDeliveries *destination, QObject *owner);
    ConfirmedBalanceDetectedMessage *getNewOrRecycled();
protected:
    void getNewOfTypeAndConnectToDestinationObject();
};

#endif // CONFIRMEDBALANCEDETECTEDMESSAGEDISPENSER_H
