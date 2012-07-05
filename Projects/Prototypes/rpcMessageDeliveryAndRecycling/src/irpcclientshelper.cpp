#include "irpcclientshelper.h"

#include "messageDispensers/broadcastdispensers.h"
#include "messageDispensers/actiondispensers.h"
#include "irpcserverimpl.h"
#include "messages/broadcasts/pendingbalanceaddeddetectedmessage.h"

IRpcClientsHelper::IRpcClientsHelper(IRpcServerImpl *rpcServerImpl)
    : m_RpcServerImpl(rpcServerImpl)
{
    setupBroadcastDispensers();
    setupInternalActionDispensers();

    //the main connections for action requests (the response is setup in setupInternalActionDispensers). we emit the signals right after we read from the message transmit source (network/thread/ipc). only the impl itself emits the signals... but we connect them right here
    //one request per action. there will be a lot more here later on... but setting them up is trivial, as you can see
    connect(this, SIGNAL(createBankAccount(CreateBankAccountMessage*)), (QObject*)rpcServerImpl, SLOT(createBankAccount(CreateBankAccountMessage*)));
}
void IRpcClientsHelper::setupBroadcastDispensers()
{
    BroadcastDispensers *broadcastDispensers = new BroadcastDispensers(this);
    m_RpcServerImpl->setBroadcastDispensers(broadcastDispensers);
}
void IRpcClientsHelper::setupInternalActionDispensers()
{
    m_ActionDispensers = new ActionDispensers(this);
}
void IRpcClientsHelper::createBankAccountCompleted()
{
    CreateBankAccountMessage *message = static_cast<CreateBankAccountMessage*>(sender());
    //remove from pending
    //send to transport
    //see the broadcast example below and note how similar they are. collapse as capable
    //but don't forget failed<Reason> shit...
    if(message)
    {
        emit d("create bank account response sent to network");
    }
    message->doneWithMessage();
}
void IRpcClientsHelper::createBankAccountFailedUsernameAlreadyExists()
{
}
void IRpcClientsHelper::pendingBalanceAddedDetected()
{
    PendingBalanceAddedDetectedMessage *message = static_cast<PendingBalanceAddedDetectedMessage*>(sender());
    //send to transport. perhaps a pure virtual for both the transport (sending to network), which operates on an IMessage... which has pure virtual stream operators?
    //then we could do:
    //sendToClientViaRpcTransport(static_cast<IMessage*>(sender())); //TODOreq: another thing, we might need that function to take the message out of our 'pending' requests. not applicable to broadcasts, but for actions it is. also, since this is so generic allegedly, why even have this slot? why not just connect the signal directly to the sendToClientViaRpc slot?
    if(message) //sanity check for debugging, but shouldn't be needed in production (famous last words)
    {
        emit d("pending balance added detected broadcast sent to network");
    }
    message->doneWithMessage();
}
