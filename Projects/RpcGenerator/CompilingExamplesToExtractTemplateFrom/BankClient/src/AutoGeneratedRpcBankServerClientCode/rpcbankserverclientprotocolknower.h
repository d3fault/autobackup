#ifndef RPCBANKSERVERCLIENTPROTOCOLKNOWER_H
#define RPCBANKSERVERCLIENTPROTOCOLKNOWER_H

#include "../../../RpcBankServerAndClientShared/iacceptrpcbankserveractiondeliveries.h"
#include "iprotocolknower.h"

#include "../../RpcBankServerAndClientShared/rpcbankserverheader.h"

#include "MessagesAndDispensers/Dispensers/Broadcasts/clientpendingbalancedetectedmessagedispenser.h"
#include "MessagesAndDispensers/Dispensers/Broadcasts/clientconfirmedbalancedetectedmessagedispenser.h"


//signal relay hack
#include "iacceptrpcbankserverclientactiondeliveries_and_iemitbroadcastsforsignalrelayhack.h"

class RpcBankServerClientProtocolKnower : public IAcceptRpcBankServerActionDeliveries, public IProtocolKnower
{
    Q_OBJECT
public:
    static void setSignalRelayHackEmitter(IAcceptRpcBankServerClientActionDeliveries_AND_IEmitBroadcastsForSignalRelayHack *signalRelayHackEmitter) { m_SignalRelayHackEmitter = signalRelayHackEmitter; }
    explicit RpcBankServerClientProtocolKnower(QObject *parent = 0);
    virtual void messageReceived();
private:
    static IAcceptRpcBankServerClientActionDeliveries_AND_IEmitBroadcastsForSignalRelayHack *m_SignalRelayHackEmitter;

    //Dispensers
    ClientPendingBalanceDetectedMessageDispenser *m_PendingBalanceDetectedMessageDispenser;
    ClientConfirmedBalanceDetectedMessageDispenser *m_ConfirmedBalanceDetectedMessageDispenser;
signals:
    
public slots:
    
};

#endif // RPCBANKSERVERCLIENTPROTOCOLKNOWER_H
