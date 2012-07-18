#ifndef IACCEPTMESSAGEDELIVERIESGOINGTORPCBANKCLIENT_H
#define IACCEPTMESSAGEDELIVERIESGOINGTORPCBANKCLIENT_H

#include "iacceptrpcbankservermessagedeliveries.h"

class IAcceptMessageDeliveriesGoingToRpcBankClient : public IAcceptRpcBankServerMessageDeliveries
{
public:
    IAcceptMessageDeliveriesGoingToRpcBankClient();
public slots:
    //Actions -- their regular responses
    virtual void createBankAccountCompleted(CreateBankAccountMessage *createBankAccountMessage)=0;
    virtual void getAddFundsKeyCompleted(GetAddFundsKeyMessage *getAddFundsKeyMessage)=0; //first key or previous key has confirmed amount > 0.0

    //Actions -- their error responses
    virtual void createBankAccountFailedPersistError(CreateBankAccountMessage *createBankAccountMessage)=0;
    virtual void createBankAccountFailedUsernameAlreadyExists(CreateBankAccountMessage *createBankAccountMessage)=0;

    //persist error for every action? i mean i do kinda want a generic fail (in case i can't figure out why i failed) but idfk
    virtual void getAddFundsKeyFailedUseExistingKeyFirst(GetAddFundsKeyMessage *getAddFundsKeyMessage)=0;
    virtual void getAddFundsKeyFailedWaitForPendingToBeConfirmed(GetAddFundsKeyMessage *getAddFundsKeyMessage)=0;

    //Broadcasts
    virtual void pendingBalanceDetected(PendingBalanceDetectedMessage *pendingBalanceDetectedMessage)=0;
    virtual void confirmedBalanceDetected(ConfirmedBalanceDetectedMessage *confirmedBalanceDetectedMessage)=0;
};

#endif // IACCEPTMESSAGEDELIVERIESGOINGTORPCBANKCLIENT_H
