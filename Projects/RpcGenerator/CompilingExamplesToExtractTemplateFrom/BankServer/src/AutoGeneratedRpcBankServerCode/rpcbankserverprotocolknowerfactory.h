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
    explicit RpcBankServerProtocolKnowerFactory(QObject *parent) : IProtocolKnowerFactory(parent)
    { }
    static void setSignalRelayHackEmitter(IAcceptRpcBankServerBroadcastDeliveries_AND_IEmitActionsForSignalRelayHack *signalRelayHackEmitter) { RpcBankServerProtocolKnower::setSignalRelayHackEmitter(signalRelayHackEmitter); }
    IProtocolKnower *getNewProtocolKnower()
    {
        return new RpcBankServerProtocolKnower(this);
    }
};

#endif // RPCBANKSERVERPROTOCOLKNOWERFACTORY_H
