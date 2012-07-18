#include "rpcbankserverclientshelper.h"

#include "ssltcpserverandbankserverprotocolknower.h"

RpcBankServerClientsHelper::RpcBankServerClientsHelper(IRpcBankServerBusiness *rpcBankServer)
    : m_RpcBankServer(rpcBankServer)
{
    m_Transporter = new SslTcpServerAndBankServerProtocolKnower();

    //as opposed to, on the rpc client: m_Transporter = new SslTcpSocketAndBankServerClientProtocolKnower
    //IBankServerProtocolKnower is a layer of separation between the protocol and the network, just like before
    //IBankServerClientProtocolKnower exists too, serves the same purpose... but is comprised of different signatures and slots instead of signals vice versa etc. they APPEAR similar but are not.

    m_ActionDispensers = new RpcBankServerActionDispensers(m_Transporter);
    m_BroadcastDispensers = new RpcBankServerBroadcastDispensers(m_Transporter);

    m_RpcBankServer->setBroadcastDispensers(m_BroadcastDispensers);

    m_RpcBankServer->instructBackendObjectsToClaimRelevantDispensers();

    if(!m_BroadcastDispensers->everyDispenserIsCreated())
        return; //TODOreq: return false. even though shit, it's called from a constructor. maybe just set up an initFailed flag and throw an error?

    takeOwnershipOfActionsAndSetupDelivery();

    m_RpcBankServer->moveBackendBusinessObjectsToTheirOwnThreadsAndStartTheThreads();
}
void RpcBankServerClientsHelper::takeOwnershipOfActionsAndSetupDelivery()
{
    m_CreateBankAccountMessageDispenser = m_ActionDispensers->takeOwnershipOfCreateBankAccountMessageDispenserRiggedForDelivery(m_Transporter);

}
