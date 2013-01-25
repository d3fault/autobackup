#ifndef IACCEPTRPCBANKSERVERBROADCASTDELIVERIES_AND_IEMITACTIONSFORSIGNALRELAYHACK_H
#define IACCEPTRPCBANKSERVERBROADCASTDELIVERIES_AND_IEMITACTIONSFORSIGNALRELAYHACK_H

#include <QObject>

//includes part of below hack
#include "../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/createbankaccountmessage.h"
#include "../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/getaddfundskeymessage.h"


class IAcceptRpcBankServerBroadcastDeliveries_AND_IEmitActionsForSignalRelayHack : public QObject
{
    Q_OBJECT
public:
    explicit IAcceptRpcBankServerBroadcastDeliveries_AND_IEmitActionsForSignalRelayHack(QObject *parent = 0);
public slots:
    virtual void pendingBalanceDetectedDelivery()=0;
    virtual void confirmedBalanceDetectedDelivery()=0;


    //hacks so I don't have to connect the rpcbankserverprotocolknower[s]-per-connection signals to rpcbankserverclientshelper(implements this) signals which stay connected to the business, otherwise the business would need to know to connect to our signals for each new connection OR our rpcbankserverprotocolknowers would have to know about the business. this allows the "test" class that knows both of us to just connect the two together. It's a hack because it's not proper design: but it will work at optimal speeds without signal relaying :). Actually the 'test' class won't (but it could i guess). The business will connect to it during the broadcast claiming period where he already knows about us. damn forgot again that he already knows about us anyways, habit of wanting to put that stuff in 'test'
private:
    inline void emitCreateBankAccountRequested(CreateBankAccountMessage *createBankAccountMessage) { emit createBankAccountRequested(createBankAccountMessage); }
    inline void emitGetAddFundsKeyRequested(GetAddFundsKeyMessage *getAddFundsKeyMessage) { emit getAddFundsKeyRequested(getAddFundsKeyMessage); }
signals:
    void createBankAccountRequested(CreateBankAccountMessage*);
    void getAddFundsKeyRequested(GetAddFundsKeyMessage*);
};

#endif // IACCEPTRPCBANKSERVERBROADCASTDELIVERIES_AND_IEMITACTIONSFORSIGNALRELAYHACK_H
