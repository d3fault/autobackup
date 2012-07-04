#include "rpcclientshelperanddeliveryacceptorandnetwork.h"

#include "messageDispensers/actiondispensers.h"

RpcClientsHelperAndDeliveryAcceptorAndNetwork::RpcClientsHelperAndDeliveryAcceptorAndNetwork(IRpcServerImpl *rpcServerImpl)
    : IRpcClientsHelper(rpcServerImpl)
{ }
void RpcClientsHelperAndDeliveryAcceptorAndNetwork::handleSimulateActionButtonClicked()
{
    //blah blah do internal getting of CreateBankAccountMessage from a dispenser

    CreateBankAccountMessage *cbaMessage = m_ActionDispensers->createBankAccountMessageDispenser()->getNewOrRecycled(); //TODOreq: no need for an ActionsDispensers container (actually there is, keep reading)... avoid a double deref (dispensers -> dispenser -> getNew) each time by just making the specific action dispensers be members. we needed to make a BroadcastDispensers because we need to pass it around to rpc server impl. similarly, don't get rid of ActionDispensers class... just optimize the deref'ing. Rpc Client impl will need the ActionDispensers object... and i already have it coded so fuck it. for now just double deref idgaf

    emit createBankAccount(cbaMessage); //weird that our initial request takes the message as an argument, yet the response has the message as a sender(). probably explains a lot of my confusion, though i don't want to waste any more time thinking about it. also, i could change that... but fuck that, not worth it. then the rpc server impl would have to cast the sender(). we only want auto-generated code to ever do static_cast'ing. so yea, the design is great how it is. make it compile/work now..
}
void RpcClientsHelperAndDeliveryAcceptorAndNetwork::init()
{
    emit initialized();
}
#if 0
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

    //pleasure is relative
    //so without pain (or simply lack of pleasure), you can't feel pleasure
}
#endif
