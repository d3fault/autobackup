#ifndef RPCCLIENTSHELPERANDDELIVERYACCEPTORANDNETWORK_H
#define RPCCLIENTSHELPERANDDELIVERYACCEPTORANDNETWORK_H

#include <QObject>
#include <QHash>

#include "irpcserverimpl.h"

class RpcClientsHelperAndDeliveryAcceptorAndNetwork : public QObject
{
    Q_OBJECT
public:
    explicit RpcClientsHelperAndDeliveryAcceptorAndNetwork(IRpcServerImpl *rpcServerImpl);
private:
    void setupBroadcastDispensers(QHash<BroadcastEnum, BroadcastDispenser*> *broadcastDispensers);
    void setupInternalActionDispensers();
signals:
    void initialized();
    void createBankAccountRequested(CreateBankAccountMessage*);
public slots:
    void handleSimulateActionButtonClicked();

    void init();

    void handleCreateBankAccountDelivery();
    void handlePendingBalancedAddedDetectedDelivery();
};

#endif // RPCCLIENTSHELPERANDDELIVERYACCEPTORANDNETWORK_H
