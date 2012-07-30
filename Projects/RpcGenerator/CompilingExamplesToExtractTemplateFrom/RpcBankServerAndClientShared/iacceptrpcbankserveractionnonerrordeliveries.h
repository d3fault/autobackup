#ifndef IACCEPTRPCBANKSERVERACTIONNONERRORDELIVERIES_H
#define IACCEPTRPCBANKSERVERACTIONNONERRORDELIVERIES_H

#include <QObject>

class IAcceptRpcBankServerActionNonErrorDeliveries : public QObject
{
    Q_OBJECT
public:
    explicit IAcceptRpcBankServerActionNonErrorDeliveries(QObject *parent = 0);
public slots:
    //Action deliveries -- used by client for the request, and server for the response
    virtual void createBankAccountDelivery()=0;
    virtual void getAddFundsKeyDelivery()=0;
};

#endif // IACCEPTRPCBANKSERVERACTIONNONERRORDELIVERIES_H
