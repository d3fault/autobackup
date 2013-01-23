#ifndef IACCEPTRPCBANKSERVERBROADCASTDELIVERIES_H
#define IACCEPTRPCBANKSERVERBROADCASTDELIVERIES_H

#include <QObject>

class IAcceptRpcBankServerBroadcastDeliveries : public QObject
{
    Q_OBJECT
public:
    explicit IAcceptRpcBankServerBroadcastDeliveries(QObject *parent = 0);
public slots:
    virtual void pendingBalanceDetectedDelivery()=0;
    virtual void confirmedBalanceDetectedDelivery()=0;
};

#endif // IACCEPTRPCBANKSERVERBROADCASTDELIVERIES_H
