#ifndef IACCEPTRPCBANKSERVERACTIONREQUESTDELIVERIES_H
#define IACCEPTRPCBANKSERVERACTIONREQUESTDELIVERIES_H

#include <QObject>

//TODOoptimization: we _could_ make IAcceptRpcBankServerMessageDeliveries (on the server) inherit this class and move this class to a shared. the only purpose of IAcceptRpcBankServerMessageDeliveries after that would be to add the broadcast deliveries
class IAcceptRpcBankServerActionRequestDeliveries : public QObject
{
    Q_OBJECT
public:
    explicit IAcceptRpcBankServerActionRequestDeliveries(QObject *parent = 0);
public slots:
    virtual void createBankAccountDelivery()=0;
    virtual void getAddFundsKeyDelivery()=0;
};

#endif // IACCEPTRPCBANKSERVERACTIONREQUESTDELIVERIES_H
