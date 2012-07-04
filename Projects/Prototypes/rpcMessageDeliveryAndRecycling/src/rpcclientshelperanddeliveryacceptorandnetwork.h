#ifndef RPCCLIENTSHELPERANDDELIVERYACCEPTORANDNETWORK_H
#define RPCCLIENTSHELPERANDDELIVERYACCEPTORANDNETWORK_H

#include <QObject>
#include <QHash>

#include "irpcclientshelper.h"

//most of this is going to move into IRpcClientsHelper

//but what DOES belong in here is the impl-specific (even though auto-generated) message transport
//so this class hasA SslTcpSocket/Server in the final. in this demo, it will be quite empty (except my simulate action button clicked shit)

class RpcClientsHelperAndDeliveryAcceptorAndNetwork : public IRpcClientsHelper
{
    Q_OBJECT
public:
    explicit RpcClientsHelperAndDeliveryAcceptorAndNetwork(IRpcServerImpl *rpcServerImpl);
signals:
    void initialized();
public slots:
    void handleSimulateActionButtonClicked();

    void init();
};

#endif // RPCCLIENTSHELPERANDDELIVERYACCEPTORANDNETWORK_H
