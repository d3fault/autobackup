#ifndef CLIENTCREATEBANKACCOUNTMESSAGEDISPENSER_H
#define CLIENTCREATEBANKACCOUNTMESSAGEDISPENSER_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/irecycleabledispenser.h"

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/createbankaccountmessage.h"
#include "../../../../../../RpcBankServerAndClientShared/iacceptrpcbankserveractiondeliveries.h"

class ClientCreateBankAccountMessageDispenser : public IRecycleableDispenser
{
    Q_OBJECT
public:
    explicit ClientCreateBankAccountMessageDispenser(IAcceptRpcBankServerActionDeliveries *destination, QObject *owner);
    CreateBankAccountMessage *getNewOrRecycled();
protected:
    IRecycleableAndStreamable *newOfTypeAndConnectToDestinationObjectIfApplicable();
};

#endif // CLIENTCREATEBANKACCOUNTMESSAGEDISPENSER_H
