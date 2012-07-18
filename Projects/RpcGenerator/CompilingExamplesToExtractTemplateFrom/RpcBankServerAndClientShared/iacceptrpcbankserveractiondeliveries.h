#ifndef IACCEPTRPCBANKSERVERACTIONDELIVERIES_H
#define IACCEPTRPCBANKSERVERACTIONDELIVERIES_H

#include <QObject>

class IAcceptRpcBankServerActionDeliveries : public QObject
{
    Q_OBJECT
public:
    explicit IAcceptRpcBankServerActionDeliveries(QObject *parent = 0);
public slots:
    virtual void createBankAccountDelivery()=0;
    virtual void getAddFundsKeyDelivery()=0;
};

#endif // IACCEPTRPCBANKSERVERACTIONDELIVERIES_H
