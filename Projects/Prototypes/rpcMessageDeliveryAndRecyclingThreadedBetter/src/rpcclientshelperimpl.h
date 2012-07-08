#ifndef RPCCLIENTSHELPERIMPL_H
#define RPCCLIENTSHELPERIMPL_H

#include "irpcclientshelper.h"

class RpcClientsHelperImpl : public IRpcClientsHelper
{
    Q_OBJECT
public:
    explicit RpcClientsHelperImpl(IRpcBusiness *business);

signals:

public slots:

};

#endif // RPCCLIENTSHELPERIMPL_H
