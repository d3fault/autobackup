#ifndef IACCEPTMESSAGEDELIVERIESGOINGTORPCBANKSERVER_H
#define IACCEPTMESSAGEDELIVERIESGOINGTORPCBANKSERVER_H

#include <QObject>

#include "iacceptrpcbankservermessagedeliveries.h"

class IAcceptMessageDeliveriesGoingToRpcBankServer : public IAcceptRpcBankServerMessageDeliveries
{
    Q_OBJECT
public slots:
    //Actions -- their requests
    virtual void createBankAccount(CreateBankAccountMessage *createBankAccountMessage)=0;
    virtual void getAddFundsKey(GetAddFundsKeyMessage *getAddFundsKeyMessage)=0;
};

#endif // IACCEPTMESSAGEDELIVERIESGOINGTORPCBANKSERVER_H
