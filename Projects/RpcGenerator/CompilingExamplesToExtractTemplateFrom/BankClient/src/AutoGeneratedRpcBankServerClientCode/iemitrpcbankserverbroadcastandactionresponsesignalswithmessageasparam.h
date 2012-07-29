#ifndef IEMITRPCBANKSERVERBROADCASTANDACTIONRESPONSESIGNALSWITHMESSAGEASPARAM_H
#define IEMITRPCBANKSERVERBROADCASTANDACTIONRESPONSESIGNALSWITHMESSAGEASPARAM_H

#include "../../../RpcBankServerAndClientShared/iacceptrpcbankservermessagedeliveries.h"
#include "../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/createbankaccountmessage.h"
#include "../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/getaddfundskeymessage.h"
#include "../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Broadcasts/pendingbalancedetectedmessage.h"
#include "../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Broadcasts/confirmedbalancedetectedmessage.h"

class IEmitRpcBankServerBroadcastAndActionResponseSignalsWithMessageAsParam : public IAcceptRpcBankServerMessageDeliveries
{
    Q_OBJECT
public:
    explicit IEmitRpcBankServerBroadcastAndActionResponseSignalsWithMessageAsParam(QObject *parent = 0);
signals:
    //Action Responses
    void createBankAccountCompleted(CreateBankAccountMessage *createBankAccountMessage);
    void getAddFundsKeyCompleted(GetAddFundsKeyMessage *getAddFundsKeyMessage);

    //Broadcasts
    void pendingBalanceDetected(PendingBalanceDetectedMessage *pendingBalanceDetectedMessage);
    void confirmedBalanceDetected(ConfirmedBalanceDetectedMessage *confirmedBalanceDetectedMessage);
};

#endif // IEMITRPCBANKSERVERBROADCASTANDACTIONRESPONSESIGNALSWITHMESSAGEASPARAM_H
