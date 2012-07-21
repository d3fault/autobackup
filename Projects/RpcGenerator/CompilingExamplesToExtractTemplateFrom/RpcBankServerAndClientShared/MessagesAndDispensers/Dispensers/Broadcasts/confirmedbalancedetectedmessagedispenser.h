#ifndef CONFIRMEDBALANCEDETECTEDMESSAGEDISPENSER_H
#define CONFIRMEDBALANCEDETECTEDMESSAGEDISPENSER_H

#include "../imessagedispenser.h"

#include "../../Messages/Broadcasts/confirmedbalancedetectedmessage.h"

#include "../../../iacceptrpcbankserveractiondeliveries.h"

class ConfirmedBalanceDetectedMessageDispenser : public IMessageDispenser
{
public:
    ConfirmedBalanceDetectedMessageDispenser(QObject *destination, QObject *owner);
    ConfirmedBalanceDetectedMessage *getNewOrRecycled();
protected:
    IMessage *getNewOfTypeAndConnectToDestinationObject();
};

#endif // CONFIRMEDBALANCEDETECTEDMESSAGEDISPENSER_H
