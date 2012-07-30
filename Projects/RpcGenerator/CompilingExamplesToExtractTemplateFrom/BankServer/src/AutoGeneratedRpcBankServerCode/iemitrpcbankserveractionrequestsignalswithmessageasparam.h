#ifndef IEMITRPCBANKSERVERACTIONREQUESTSIGNALSWITHMESSAGEASPARAM_H
#define IEMITRPCBANKSERVERACTIONREQUESTSIGNALSWITHMESSAGEASPARAM_H

#include <QObject>

#include "iacceptrpcbankserveractionandactionerrorandbroadcastmessagedeliveries.h"
#include "MessagesAndDispensers/Messages/Actions/servercreatebankaccountmessage.h"
#include "MessagesAndDispensers/Messages/Actions/servergetaddfundskeymessage.h"

//IEmit is parent of IAccept in this case because. soo... the idea was that i'd have rpc client impl inherit IAcceptRpcBankServerMessageDeliveries... but the rpc client has no reason for the "IEmitActionRequestSignals" ..... WITH PARAMETERS (without and we'd be in good shape. lol subtle differences that were fucking me up a few days ago). so now, is the converse true? IEmitRpcBankServerActionRequestSignalsWithMessageAsParam is used for the server protocol knower... and i haven't thought of a place where it could also be inherited yet but that's the point of this typing to make it easier to think and pretty much so i don't have to lol because i'm fucking lazy as shit and watching House while coding lol i'm not even trying to think right now i'm just assuming it will come to me (it will). The IEmitRpcBankServerActionRequestSignalsWithMessageAsParam _ALMOST_ fits as the rpc client (auto-generated-code) emitting... except it needs paramaeters. lol subtle difference again. but all that really matters is whether IEmit should inherit IAccept of vice versa. i think i figured it out, IEmit inherits IAccept. wait no did i just contradict myself? i'm thinking this might not even be possible because i'll end up with the diamond OO problem (two QObjects). so only ProtocolKnower inherits IEmitIEmitRpcBankServerActionRequestSignalsWithMessageAsParam ... and both ProtocolKnower and RpcClientImpl inherit IAcceptRpcBankServerMessageDeliveries... so that means the IEmitRpcBankServerActionRequestSignalsWithMessageAsParam should be the one inheritting the IAcceptRpcBankServerMessageDeliveries
class IEmitRpcBankServerClientActionRequestSignalsWithMessageAsParam : public IAcceptRpcBankServerActionAndActionErrorAndBroadcastMessageDeliveries
{
    Q_OBJECT
public:
    IEmitRpcBankServerClientActionRequestSignalsWithMessageAsParam(QObject *parent = 0);
signals:
    //incoming action requests
    void createBankAccount(ServerCreateBankAccountMessage *createBankAccountMessage);
    void getAddFundsKey(ServerGetAddFundsKeyMessage *getAddFundsKeyMessage);
};

#endif // IEMITRPCBANKSERVERACTIONREQUESTSIGNALSWITHMESSAGEASPARAM_H
