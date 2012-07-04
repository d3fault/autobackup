#ifndef IRPCCLIENTSHELPER_H
#define IRPCCLIENTSHELPER_H

#include <QObject>
#include <QHash>

#include "messageDispensers/broadcastdispensers.h"
#include "messageDispensers/actiondispensers.h"
#include "irpcserverimpl.h"

//TODOopt: perhaps a IRpcClientsHelper, that declares teh pure virtual "sendToTransport" etc... then a IRpcBankServerClientsHelper that implements that first ClientsHelper, that just adds signals/slots specific to the bank server... then the clients helper impl that is overrides the first base's pure virtual sendToTransport etc
//^^^the benefit we get from that is that the user is allowed to edit the generated clients helper impl and the base IRpcClientsHelper... and we can make the rpc generator not overwrite those files if they already exist (we could check for versioning and overwrite them on new version). only the middle interface that defines the bank server specific stuff gets overwritten on each new generate. we can warn them in the header:
//"auto generated file. editting this file will get your changes overwritten whenever you re-generate the rpc shit (example: adding an action, changing it)
//vs
//"auto generated file. you can edit this file, as it is only ever overwritten when the rpc generator version changes" <- that last part is an additional optimization on top of this optimization. we could initially just never overwrite it. also, we could provide them a way to override the overwriting. their own fault if it doesn't compile afterwards (like a 0 for the version number means don't overwrite because user has taken over)

class IRpcClientsHelper : public QObject
{
    Q_OBJECT
public:
    explicit IRpcClientsHelper(IRpcServerImpl *rpcServerImpl)
        : m_RpcServerImpl(rpcServerImpl)
    {
        setupBroadcastDispensers();
        setupInternalActionDispensers();


        //the main connections for action requests (the response is setup in setupInternalActionDispensers). we emit the signals right after we read from the message transmit source (network/thread/ipc). only the impl itself emits the signals... but we connect them right here
        //one request per action. there will be a lot more here later on... but setting them up is trivial, as you can see
        connect(this, SIGNAL(createBankAccount(CreateBankAccountMessage*)), (QObject*)rpcServerImpl, SLOT(createBankAccount(CreateBankAccountMessage*)));
    }
private:
    IRpcServerImpl *m_RpcServerImpl;
    ActionDispensers *m_ActionDispensers; //within the server side of the rpc, only the clients helper needs access to action dispensers. therefore we own it and the rpc server impl does not have access to it. on the client side, the 'rpc client impl' can/will have access to it. but conversely, they will NOT have access to the broadcast dispensers
    void setupBroadcastDispensers()
    {
        BroadcastDispensers *broadcastDispensers = new BroadcastDispensers(this); //maybe just pass the 'this' into the constructor and it'll pass it to each child? sounds easier actually rofl
        //broadcastDispensers->pendingBalanceAddedDetectedMessageDispenser()->setMessageFinishedDestinationObject(this);
        //etc
        m_RpcServerImpl->setBroadcastDispensers(broadcastDispensers);

#if 0
        BroadcastDispenser *reUsedBasePointer;

        //pending balanced added detected
        reUsedBasePointer = new PendingBalanceAddedDetectedMessageDispenser(this); //so perhaps IDispenseDeliverables has a constructor that accepts a QObject pointer and a const char* slot? changed my mind about the slot name, the broadcast-specific messageDispenser accepts an IRpcClientsHelper and connects it's dispenser-specific signals to the dispenser-specific slots on the IRpcClientsHelper. it just knows (auto-generated code)

        broadcastDispensers->insert(PendingBalanceAddedDetected, reUsedBasePointer); //instead of an enum'd hash, i could just have them as public members (or private with getters) that use the name of what they are. broadcastDispensers would be a class just collecting ever type of broadcast dispenser...
        //^^if i do what that comment says, then i probably won't be new'ing the dispenser in here. that will be done in Bank's constructor. instead, i'll just be calling 'set what object/thread to send broadcast to'

        //the message dispenser remembers 'this' as a qobject, and connects each message's deliverSignal() to the slot specified on creation
        //^^^^fuck the deliver() shit because inheriting from 2 QObjects just isn't worth it. i am still in love with IDeliver as an interface, but i'm not going to stick with a worse design just to use it

        //reUsedBasePointer = new ConfirmedBalanceAddedDetected.... same shit
#endif
    }
    void setupInternalActionDispensers()
    {
        m_ActionDispensers = new ActionDispensers(this);
        //m_ActionDispensers->createBankAccountMessageDispenser()->setMessageFinishedDestinationObject(this);
    }
public slots:
    //create bank account response as it comes back from business impl
    void createBankAccountCompleted()
    {
        CreateBankAccountMessage *message = static_cast<CreateBankAccountMessage*>(sender());
        //remove from pending
        //send to transport
        //see the broadcast example below and note how similar they are. collapse as capable
        //but don't forget failed<Reason> shit...
        message->doneWithMessage();
    }
    void createBankAccountFailedUsernameAlreadyExists(CreateBankAccountMessage *createBankAccountMessage);
    //etc: persist error or w/e

    //broadcast handling to be dispatched onto network
    void pendingBalanceAddedDetected()
    {
        PendingBalanceAddedDetectedMessage *message = static_cast<PendingBalanceAddedDetectedMessage*>(sender());
        //send to transport. perhaps a pure virtual for both the transport (sending to network), which operates on an IMessage... which has pure virtual stream operators?
        //then we could do:
        //sendToClientViaRpcTransport(static_cast<IMessage*>(sender())); //TODOreq: another thing, we might need that function to take the message out of our 'pending' requests. not applicable to broadcasts, but for actions it is. also, since this is so generic allegedly, why even have this slot? why not just connect the signal directly to the sendToClientViaRpc slot?
        message->doneWithMessage();
    }
signals:
    //create bank account request as it comes in from network
    void createBankAccount(CreateBankAccountMessage *createBankAccountMessage);
};

#endif // IRPCCLIENTSHELPER_H
