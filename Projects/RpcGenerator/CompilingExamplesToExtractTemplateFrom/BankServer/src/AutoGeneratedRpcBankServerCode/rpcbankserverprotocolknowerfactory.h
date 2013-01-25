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
    explicit RpcBankServerProtocolKnowerFactory(IAcceptRpcBankServerBroadcastDeliveries *signalRelayHackEmitter, QObject *parent) : m_SignalRelayHackEmitter(signalRelayHackEmitter), IProtocolKnowerFactory(parent) { }
private:
    IAcceptRpcBankServerBroadcastDeliveries *m_SignalRelayHackEmitter;
protected:
    IProtocolKnower *getNewProtocolKnower()
    {
        return new RpcBankServerProtocolKnower(m_SignalRelayHackEmitter, this);
    }
};

#endif // RPCBANKSERVERPROTOCOLKNOWERFACTORY_H
