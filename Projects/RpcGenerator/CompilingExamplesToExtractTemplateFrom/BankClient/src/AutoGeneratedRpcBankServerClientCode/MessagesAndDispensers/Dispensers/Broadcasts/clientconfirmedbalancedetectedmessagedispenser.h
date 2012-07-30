#ifndef CLIENTCONFIRMEDBALANCEDETECTEDMESSAGEDISPENSER_H
#define CLIENTCONFIRMEDBALANCEDETECTEDMESSAGEDISPENSER_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/irecycleabledispenser.h"

#include "../../Messages/Broadcasts/clientconfirmedbalancedetectedmessage.h"

class ClientConfirmedBalanceDetectedMessageDispenser : public IRecycleableDispenser
{
    Q_OBJECT
public:
    explicit ClientConfirmedBalanceDetectedMessageDispenser(QObject *destination, QObject *owner);
    ClientConfirmedBalanceDetectedMessage *getNewOrRecycled();
protected:
    IRecycleableAndStreamable *newOfTypeAndConnectToDestinationObjectIfApplicable();
};

#endif // CLIENTCONFIRMEDBALANCEDETECTEDMESSAGEDISPENSER_H
