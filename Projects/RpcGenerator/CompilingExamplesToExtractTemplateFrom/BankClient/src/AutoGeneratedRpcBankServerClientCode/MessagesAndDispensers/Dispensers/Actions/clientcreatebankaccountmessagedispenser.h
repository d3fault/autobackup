#ifndef CLIENTCREATEBANKACCOUNTMESSAGEDISPENSER_H
#define CLIENTCREATEBANKACCOUNTMESSAGEDISPENSER_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/irecycleabledispenser.h"

#include "../../Messages/Actions/clientcreatebankaccountmessage.h"
#include "../../../../../../RpcBankServerAndClientShared/iacceptrpcbankserveractionnonerrordeliveries.h"

class ClientCreateBankAccountMessageDispenser : public IRecycleableDispenser
{
    Q_OBJECT
public:
    explicit ClientCreateBankAccountMessageDispenser(IAcceptRpcBankServerActionNonErrorDeliveries *destination, QObject *owner);
    ClientCreateBankAccountMessage *getNewOrRecycled();
protected:
    IRecycleableAndStreamable *newOfTypeAndConnectToDestinationObjectIfApplicable();
};

#endif // CLIENTCREATEBANKACCOUNTMESSAGEDISPENSER_H
