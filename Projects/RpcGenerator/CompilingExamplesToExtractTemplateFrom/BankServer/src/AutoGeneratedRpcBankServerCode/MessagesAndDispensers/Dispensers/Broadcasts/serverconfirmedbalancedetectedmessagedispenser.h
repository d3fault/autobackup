#ifndef CONFIRMEDBALANCEDETECTEDMESSAGEDISPENSER_H
#define CONFIRMEDBALANCEDETECTEDMESSAGEDISPENSER_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/irecycleabledispenser.h"

#include "../../Messages/Broadcasts/serverconfirmedbalancedetectedmessage.h"

#include "../../../iemitrpcbankserveractionrequestsignalswithmessageasparamandiacceptalldeliveries.h"

class ServerConfirmedBalanceDetectedMessageDispenser : public IRecycleableDispenser
{
    Q_OBJECT
public:
    ServerConfirmedBalanceDetectedMessageDispenser(IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptActionDeliveries *destination, QObject *owner);
    ServerConfirmedBalanceDetectedMessage *getNewOrRecycled();
protected:
    IRecycleableAndStreamable *newOfTypeAndConnectToDestinationObjectIfApplicable();
};

#endif // CONFIRMEDBALANCEDETECTEDMESSAGEDISPENSER_H
