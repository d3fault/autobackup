#ifndef CLIENTPENDINGBALANCEDETECTEDMESSAGEDISPENSER_H
#define CLIENTPENDINGBALANCEDETECTEDMESSAGEDISPENSER_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/irecycleabledispenser.h"

#include "../../Messages/Broadcasts/clientpendingbalancedetectedmessage.h"

class ClientPendingBalanceDetectedMessageDispenser : public IRecycleableDispenser
{
    Q_OBJECT
public:
    explicit ClientPendingBalanceDetectedMessageDispenser(QObject *destination, QObject *owner);
    ClientPendingBalanceDetectedMessage *getNewOrRecycled();
protected:
    IRecycleableAndStreamable *newOfTypeAndConnectToDestinationObjectIfApplicable();
};

#endif // CLIENTPENDINGBALANCEDETECTEDMESSAGEDISPENSER_H
