#ifndef IRPCCLIENTSHELPER_H
#define IRPCCLIENTSHELPER_H

#include <QObject>
#include <QHash>

class BroadcastDispensers;
class ActionDispensers;
class IRpcServerImpl;

class CreateBankAccountMessage;

//TODOopt: perhaps a IRpcClientsHelper, that declares teh pure virtual "sendToTransport" etc... then a IRpcBankServerClientsHelper that implements that first ClientsHelper, that just adds signals/slots specific to the bank server... then the clients helper impl that is overrides the first base's pure virtual sendToTransport etc
//^^^the benefit we get from that is that the user is allowed to edit the generated clients helper impl and the base IRpcClientsHelper... and we can make the rpc generator not overwrite those files if they already exist (we could check for versioning and overwrite them on new version). only the middle interface that defines the bank server specific stuff gets overwritten on each new generate. we can warn them in the header:
//"auto generated file. editting this file will get your changes overwritten whenever you re-generate the rpc shit (example: adding an action, changing it)
//vs
//"auto generated file. you can edit this file, as it is only ever overwritten when the rpc generator version changes" <- that last part is an additional optimization on top of this optimization. we could initially just never overwrite it. also, we could provide them a way to override the overwriting. their own fault if it doesn't compile afterwards (like a 0 for the version number means don't overwrite because user has taken over)

class IRpcClientsHelper : public QObject
{
    Q_OBJECT
public:
    explicit IRpcClientsHelper(IRpcServerImpl *rpcServerImpl);
private:
    IRpcServerImpl *m_RpcServerImpl;
    void setupBroadcastDispensers();
    void setupInternalActionDispensers();
protected:
    ActionDispensers *m_ActionDispensers; //within the server side of the rpc, only the clients helper needs access to action dispensers. therefore we own it and the rpc server impl does not have access to it. on the client side, the 'rpc client impl' can/will have access to it. but conversely, they will NOT have access to the broadcast dispensers
public slots:
    //create bank account response as it comes back from business impl
    void createBankAccountCompleted();
    void createBankAccountFailedUsernameAlreadyExists();
    //etc: persist error or w/e

    //broadcast handling to be dispatched onto network
    void pendingBalanceAddedDetected();
signals:
    void d(const QString &);
    //create bank account request as it comes in from network
    void createBankAccount(CreateBankAccountMessage *createBankAccountMessage);
};

#endif // IRPCCLIENTSHELPER_H
