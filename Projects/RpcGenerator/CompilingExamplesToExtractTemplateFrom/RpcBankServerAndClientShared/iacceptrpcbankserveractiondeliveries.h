#ifndef IACCEPTRPCBANKSERVERACTIONDELIVERIES_H
#define IACCEPTRPCBANKSERVERACTIONDELIVERIES_H

#include <QObject>

class IAcceptRpcBankServerActionDeliveries : public QObject
{
    Q_OBJECT
public:
    explicit IAcceptRpcBankServerActionDeliveries(QObject *parent = 0);
public slots:
    //Action deliveries -- used by client for the request, and server for the response
    virtual void createBankAccountDelivery()=0;
    virtual void getAddFundsKeyDelivery()=0;
};

#endif // IACCEPTRPCBANKSERVERACTIONDELIVERIES_H
