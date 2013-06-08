#ifndef RPCBANKSERVERCLIENTPROTOCOLKNOWERFACTORY_H
#define RPCBANKSERVERCLIENTPROTOCOLKNOWERFACTORY_H

//interface for self
#include "iprotocolknowerfactory.h"

//implementation for object i am factory'ing
#include "rpcbankserverclientprotocolknower.h"

//no-signal-relaying-hack
#include "iacceptrpcbankserverclientactiondeliveries_and_iemitactionsandbroadcastsbothwithmessageasparamforsignalrelayhack.h"

class RpcBankServerClientProtocolKnowerFactory : public IProtocolKnowerFactory
{
    Q_OBJECT
public:
    explicit RpcBankServerClientProtocolKnowerFactory(QObject *parent = 0) : IProtocolKnowerFactory(parent) { }
    static void setSignalRelayHackEmitter(IAcceptRpcBankServerClientActionDeliveries_AND_IEmitActionsAndBroadcastsBothWithMessageAsParamForSignalRelayHack *signalRelayHackEmitter) { RpcBankServerClientProtocolKnower::setSignalRelayHackEmitter(signalRelayHackEmitter); }
    virtual IProtocolKnower *getNewProtocolKnower() /*TODOreq: owner takes ownership (already dealt with this in server, but now need to make sure i do in client)*/
    {
        return new RpcBankServerClientProtocolKnower(this);
    }
};

#endif // RPCBANKSERVERCLIENTPROTOCOLKNOWERFACTORY_H
