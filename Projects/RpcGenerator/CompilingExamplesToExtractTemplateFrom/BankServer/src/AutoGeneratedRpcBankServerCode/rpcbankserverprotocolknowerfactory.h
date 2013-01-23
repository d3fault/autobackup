#ifndef RPCBANKSERVERPROTOCOLKNOWERFACTORY_H
#define RPCBANKSERVERPROTOCOLKNOWERFACTORY_H

//interface for self
#include "iprotocolknowerfactory.h"

//implementation for object i am factory'ing
#include "rpcbankserverprotocolknower.h"

class RpcBankServerProtocolKnowerFactory : public IProtocolKnowerFactory
{
    Q_OBJECT
public:
    explicit RpcBankServerProtocolKnowerFactory(QObject *parent) : IProtocolKnowerFactory(parent) { }
protected:
    IProtocolKnower *getNewProtocolKnower()
    {
        return new RpcBankServerProtocolKnower(this);
    }
};

#endif // RPCBANKSERVERPROTOCOLKNOWERFACTORY_H
