#ifndef CONFIRMEDBALANCEDETECTEDMESSAGEDISPENSER_H
#define CONFIRMEDBALANCEDETECTEDMESSAGEDISPENSER_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/irecycleabledispenser.h"

#include "../../Messages/Broadcasts/serverconfirmedbalancedetectedmessage.h"

#include "../../../iacceptrpcbankserverbroadcastdeliveries_and_iemitactionsforsignalrelayhack.h"

class ServerConfirmedBalanceDetectedMessageDispenser : public IRecycleableDispenser
{
    Q_OBJECT
public:
    ServerConfirmedBalanceDetectedMessageDispenser(IAcceptRpcBankServerBroadcastDeliveries_AND_IEmitActionsForSignalRelayHack *destination, QObject *owner);
    ServerConfirmedBalanceDetectedMessage *getNewOrRecycled();
protected:
    IRecycleableAndStreamable *newOfTypeAndConnectToDestinationObjectIfApplicable();
};

#endif // CONFIRMEDBALANCEDETECTEDMESSAGEDISPENSER_H
