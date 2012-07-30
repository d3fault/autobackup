#ifndef IACCEPTRPCBANKSERVERACTIONANDACTIONERRORANDBROADCASTMESSAGEDELIVERIES_H
#define IACCEPTRPCBANKSERVERACTIONANDACTIONERRORANDBROADCASTMESSAGEDELIVERIES_H

#include "../../../RpcBankServerAndClientShared/iacceptrpcbankserveractionnonerrordeliveries.h"

class IAcceptRpcBankServerActionAndActionErrorAndBroadcastMessageDeliveries : public IAcceptRpcBankServerActionNonErrorDeliveries
{
    Q_OBJECT
public:
    explicit IAcceptRpcBankServerActionAndActionErrorAndBroadcastMessageDeliveries(QObject *parent = 0);
public slots:
    //Broadcasts
    virtual void pendingBalanceDetectedDelivery()=0;
    virtual void confirmedBalanceDetectedDelivery()=0;

    //Errors
    virtual void createBankAccountFailedUsernameAlreadyExists()=0;
    virtual void createBankAccountFailedPersistError()=0;

    virtual void getAddFundsKeyFailedUsernameDoesntExist()=0;
    virtual void getAddFundsKeyFailedUseExistingKeyFirst()=0;
    virtual void getAddFundsKeyFailedWaitForPendingToBeConfirmed()=0;
};

#endif // IACCEPTRPCBANKSERVERACTIONANDACTIONERRORANDBROADCASTMESSAGEDELIVERIES_H
