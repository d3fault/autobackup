#include "rpcclientshelperanddeliveryacceptorandnetwork.h"

RpcClientsHelperAndDeliveryAcceptorAndNetwork::RpcClientsHelperAndDeliveryAcceptorAndNetwork(IRpcServerImpl *rpcServerImpl)
{
    setupBroadcastDispensers(rpcServerImpl->broadcastDispensers());

    setupInternalActionDispensers();

    connect(this, SIGNAL(createBankAccountRequested(CreateBankAccountMessage*)), rpcServerImpl, SLOT(createBankAccount(CreateBankAccountMessage*)));
}
void RpcClientsHelperAndDeliveryAcceptorAndNetwork::handleSimulateActionButtonClicked()
{
    //blah blah do internal getting of CreateBankAccountMessage from a dispenser

    emit createBankAccountRequested(new CreateBankAccountMessage());
}
void RpcClientsHelperAndDeliveryAcceptorAndNetwork::handleCreateBankAccountDelivery()
{
    CreateBankAccountMessage *tehMessage = static_cast<CreateBankAccountMessage*>(sender());

    //write response/results to the network

    tehMessage->doneWithMessage();
}
void RpcClientsHelperAndDeliveryAcceptorAndNetwork::init()
{
    emit initialized();
}
void RpcClientsHelperAndDeliveryAcceptorAndNetwork::handlePendingBalancedAddedDetectedDelivery()
{
    PendingBalanceAddedDetectedMessage *message = static_cast<PendingBalanceAddedDetectedMessage*>(sender());

    //write the broadcast itself to the network

    message->doneWithMessage();
}
void RpcClientsHelperAndDeliveryAcceptorAndNetwork::setupBroadcastDispensers(QHash<BroadcastEnum, BroadcastDispenser *> *broadcastDispensers)
{
    BroadcastDispenser *reUsedBasePointer;

    //pending balanced added detected
    reUsedBasePointer = new PendingBalanceAddedDetectedMessageDispenser(this, SLOT(handlePendingBalancedAddedDetectedDelivery())); //so perhaps IDispenseDeliverables has a constructor that accepts a QObject pointer and a const char* slot?
    broadcastDispensers->insert(PendingBalanceAddedDetected, reUsedBasePointer);
    //the message dispenser remembers 'this' as a qobject, and connects each message's deliverSignal() to the slot specified on creation

    //reUsedBasePointer = new ConfirmedBalanceAddedDetected.... same shit
}
void RpcClientsHelperAndDeliveryAcceptorAndNetwork::setupInternalActionDispensers()
{
    //still yet to decide if i should use internal action dispensers on the rpc client side
    //it might actually help with pipelining / minimizing memory usage
    //for example, in our appdb rpc server impl, we could read directly off the message passed to us onto the relevant rpc client message
    //this instead of passing in each variable by arg which makes temp copies just for the args... (i think)

    //my brain hurts more than it should
    //i fucking hate this
    //so simple yet so overcomplicated
    //what the fuck
    //i just want to sit braindead
    //in a state of pleasure
    //infinitely
}
