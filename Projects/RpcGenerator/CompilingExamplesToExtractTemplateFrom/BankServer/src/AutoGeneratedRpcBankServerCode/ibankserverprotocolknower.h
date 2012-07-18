#ifndef IBANKSERVERPROTOCOLKNOWER_H
#define IBANKSERVERPROTOCOLKNOWER_H

#include <QObject>

#include "../../../RpcBankServerAndClientShared/iacceptrpcbankserveractiondeliveries.h"
#include "../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/createbankaccountmessage.h"
#include "../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/getaddfundskeymessage.h"

class IBankServerProtocolKnower : public IAcceptRpcBankServerActionDeliveries
{
    Q_OBJECT
public:
    explicit IBankServerProtocolKnower(QObject *parent = 0);
signals:
    //incoming action requests
    void createBankAccount(CreateBankAccountMessage *createBankAccountMessage);
    void getAddFundsKey(GetAddFundsKeyMessage *getAddFundsKeyMessage);
public slots:
    void createBankAccountDelivery(); //deliver'd from rpc server impl. our IRpcBankServerClientProtocolKnower on rpc client also inherits IAcceptRpcBankServerActionDeliveries
    void getAddFundsKeyDelivery();

    //errorrs, delivered similarly to .deliver()
    void createBankAccountFailedUsernameAlreadyExists();
    void createBankAccountFailedPersistError(); //for some reason i don't like this error anymore. perhaps because it is not an error you'd want to show a user
    void getAddFundsKeyFailedUseExistingKeyFirst();
    void getAddFundsKeyFailedWaitUntilConfirmed();

    //my IAcceptRpcBankServerActionDeliveries doesn't account for errors atm, fml. actually i don't think it matters. actually yes it does. because my CreateBankAccountMessage on the rpc client has no need for rigging the errors like deliver()'ing... we just emit them (the errors) as a parameter to the rpc client impl
    //i don't want to have 2 sets of messages. bullshit. reallly though, who gives a shit. auto-generated it auto-generated
    //fml i am overcomplicating the FUCK out of this

    TODO LEFT OFF ^^^^^^
};

#endif // IBANKSERVERPROTOCOLKNOWER_H
