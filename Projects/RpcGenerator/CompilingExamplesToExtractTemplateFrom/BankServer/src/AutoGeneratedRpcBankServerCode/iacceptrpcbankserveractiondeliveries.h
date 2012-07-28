#ifndef IACCEPTRPCBANKSERVERACTIONDELIVERIES_H
#define IACCEPTRPCBANKSERVERACTIONDELIVERIES_H

#include <QObject>

class IAcceptRpcBankServerMessageDeliveries : public QObject
{
    Q_OBJECT
public:
    explicit IAcceptRpcBankServerMessageDeliveries(QObject *parent = 0);
public slots:
    //Actions
    virtual void createBankAccountDelivery()=0;
    virtual void getAddFundsKeyDelivery()=0;

    //Broadcasts
    virtual void pendingBalanceDetectedDelivery()=0;
    virtual void confirmedBalanceDetectedDelivery()=0;
};

#endif // IACCEPTRPCBANKSERVERACTIONDELIVERIES_H
