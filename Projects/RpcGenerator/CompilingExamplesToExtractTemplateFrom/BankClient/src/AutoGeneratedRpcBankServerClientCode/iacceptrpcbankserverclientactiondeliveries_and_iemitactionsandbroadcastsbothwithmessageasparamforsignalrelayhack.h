#ifndef IACCEPTRPCBANKSERVERCLIENTACTIONDELIVERIES_AND_IEMITACTIONSANDBROADCASTSBOTHWITHMESSAGEASPARAMFORSIGNALRELAYHACK_H
#define IACCEPTRPCBANKSERVERCLIENTACTIONDELIVERIES_AND_IEMITACTIONSANDBROADCASTSBOTHWITHMESSAGEASPARAMFORSIGNALRELAYHACK_H

#include <QObject>

#include "../../../RpcBankServerAndClientShared/iacceptrpcbankserveractiondeliveries.h"

#include "../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/createbankaccountmessage.h"
#include "../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/getaddfundskeymessage.h"
#include "MessagesAndDispensers/Messages/Broadcasts/clientpendingbalancedetectedmessage.h"
#include "MessagesAndDispensers/Messages/Broadcasts/clientconfirmedbalancedetectedmessage.h"

class IAcceptRpcBankServerClientActionDeliveries_AND_IEmitActionsAndBroadcastsBothWithMessageAsParamForSignalRelayHack : public IAcceptRpcBankServerActionDeliveries
{
    Q_OBJECT
public:
    explicit IAcceptRpcBankServerClientActionDeliveries_AND_IEmitActionsAndBroadcastsBothWithMessageAsParamForSignalRelayHack(QObject *parent = 0);
    inline void emitCreateBankAccountCompleted(CreateBankAccountMessage *createBankAccountMessage) { emit createBankAccountCompleted(createBankAccountMessage); }
    inline void emitGetAddFundsKeyCompleted(GetAddFundsKeyMessage *getAddFundsKeyMessage) { emit getAddFundsKeyCompleted(getAddFundsKeyMessage); }
    inline void emitPendingBalanceDetected(ClientPendingBalanceDetectedMessage *pendingBalanceDetectedMessage) { emit pendingBalanceDetectedBroadcasted(pendingBalanceDetectedMessage); }
    inline void emitConfirmedBalanceDetected(ClientConfirmedBalanceDetectedMessage *confirmedBalanceDetectedMessage) { emit confirmedBalanceDetectedBroadcasted(confirmedBalanceDetectedMessage); }
signals:
    void createBankAccountCompleted(CreateBankAccountMessage *createBankAccountMessage);
    void getAddFundsKeyCompleted(GetAddFundsKeyMessage *getAddFundsKeyMessage);

    void pendingBalanceDetectedBroadcasted(ClientPendingBalanceDetectedMessage*);
    void confirmedBalanceDetectedBroadcasted(ClientConfirmedBalanceDetectedMessage*);
};

#endif // IACCEPTRPCBANKSERVERCLIENTACTIONDELIVERIES_AND_IEMITACTIONSANDBROADCASTSBOTHWITHMESSAGEASPARAMFORSIGNALRELAYHACK_H
