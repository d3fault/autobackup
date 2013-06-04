#ifndef IACCEPTRPCBANKSERVERCLIENTACTIONDELIVERIES_AND_IEMITBROADCASTSFORSIGNALRELAYHACK_H
#define IACCEPTRPCBANKSERVERCLIENTACTIONDELIVERIES_AND_IEMITBROADCASTSFORSIGNALRELAYHACK_H

#include <QObject>

#include "MessagesAndDispensers/Messages/Broadcasts/clientpendingbalancedetectedmessage.h"
#include "MessagesAndDispensers/Messages/Broadcasts/clientconfirmedbalancedetectedmessage.h"

class IAcceptRpcBankServerClientActionDeliveries_AND_IEmitBroadcastsForSignalRelayHack : public QObject
{
    Q_OBJECT
public:
    explicit IAcceptRpcBankServerClientActionDeliveries_AND_IEmitBroadcastsForSignalRelayHack(QObject *parent = 0);
    inline void emitPendingBalanceDetected(ClientPendingBalanceDetectedMessage *pendingBalanceDetectedMessage) { emit pendingBalanceDetectedBroadcasted(pendingBalanceDetectedMessage); }
    inline void emitConfirmedBalanceDetected(ClientConfirmedBalanceDetectedMessage *confirmedBalanceDetectedMessage) { emit confirmedBalanceDetectedBroadcasted(confirmedBalanceDetectedMessage); }
signals:
    void pendingBalanceDetectedBroadcasted(ClientPendingBalanceDetectedMessage*);
    void confirmedBalanceDetectedBroadcasted(ClientConfirmedBalanceDetectedMessage*);
public slots:
    virtual void createBankAccountDelivery()=0;
    virtual void getAddFundsKeyDelivery()=0;
};

#endif // IACCEPTRPCBANKSERVERCLIENTACTIONDELIVERIES_AND_IEMITBROADCASTSFORSIGNALRELAYHACK_H
