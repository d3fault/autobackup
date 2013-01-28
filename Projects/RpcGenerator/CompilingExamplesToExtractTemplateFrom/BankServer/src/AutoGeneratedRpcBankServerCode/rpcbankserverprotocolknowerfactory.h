#ifndef RPCBANKSERVERPROTOCOLKNOWERFACTORY_H
#define RPCBANKSERVERPROTOCOLKNOWERFACTORY_H

//interface for self
#include "iprotocolknowerfactory.h"

//implementation for object i am factory'ing
#include "rpcbankserverprotocolknower.h"

//no-signal-relaying-hack
#include "iacceptrpcbankserverbroadcastdeliveries_and_iemitactionsforsignalrelayhack.h"

class RpcBankServerProtocolKnowerFactory : public IProtocolKnowerFactory
{
    Q_OBJECT
public:
    explicit RpcBankServerProtocolKnowerFactory(IAcceptRpcBankServerBroadcastDeliveries_AND_IEmitActionsForSignalRelayHack *signalRelayHackEmitter, QObject *parent) : IProtocolKnowerFactory(parent), m_SignalRelayHackEmitter(signalRelayHackEmitter) { }
    IProtocolKnower *getNewProtocolKnower()
    {
        return new RpcBankServerProtocolKnower(m_SignalRelayHackEmitter, this);
    }
private:
    IAcceptRpcBankServerBroadcastDeliveries_AND_IEmitActionsForSignalRelayHack *m_SignalRelayHackEmitter;
};

#endif // RPCBANKSERVERPROTOCOLKNOWERFACTORY_H
