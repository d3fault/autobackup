#ifndef IACCEPTRPCBANKSERVERMESSAGEDELIVERIES_H
#define IACCEPTRPCBANKSERVERMESSAGEDELIVERIES_H

#include <QObject>

//I accept both Actions and Broadcast deliveries

//BAH, i'm going to make both client and server implement this. client has no use for the broadcasts, but it needs to be done anyways unless i redesign shit lots. too lazy. i'll just make a delivery() on teh client call .doneWithMessage() in case the user calls it accidentally

//OLD: //we _could_ make IAcceptRpcBankServerMessageDeliveries (on the server) inherit this class and move this class to a shared. the only purpose of IAcceptRpcBankServerMessageDeliveries after that would be to add the broadcast deliveries
//bah now i HAVE to do that optimization lol. i shouldn't differentiate between requests and responses. they both need it (on server and client)
//only server needs to accept broadcast deliveries though..

class IAcceptRpcBankServerMessageDeliveries : public QObject
{
    Q_OBJECT
public:
    explicit IAcceptRpcBankServerMessageDeliveries(QObject *parent = 0);
public slots:
    //Actions are inherited, because client uses them too. It's in shared. <-- see BAH above. getting rid of Actions-Only interface... no use for it anymore
    //Actions
    virtual void createBankAccountDelivery()=0;
    virtual void getAddFundsKeyDelivery()=0;

    //Broadcasts are only applicable on server. client does not do broadcast deliveries (not to be confused with emitting the broadcast with the message as a param)
    virtual void pendingBalanceDetectedDelivery()=0;
    virtual void confirmedBalanceDetectedDelivery()=0;
};

#endif // IACCEPTRPCBANKSERVERMESSAGEDELIVERIES_H
