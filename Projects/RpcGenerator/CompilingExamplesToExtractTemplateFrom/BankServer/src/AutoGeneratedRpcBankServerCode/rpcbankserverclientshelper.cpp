#include "rpcbankserverclientshelper.h"

RpcBankServerClientsHelper::RpcBankServerClientsHelper(IRpcBankServer *rpcBankServer)
    : m_RpcBankServer(rpcBankServer)
{
    m_Transporter = new SslTcpServerAndProtocolKnower();

    m_ActionDispensers = new ActionDispensers(m_Transporter);
    m_BroadcastDispensers = new BroadcastDispensers(m_Transporter);

    m_RpcBankServer->setBroadcastDispensers(m_BroadcastDispensers);

    m_RpcBankServer->instructBackendObjectsToClaimRelevantDispensers();

    if(!m_BroadcastDispensers->everyDispenserIsCreated())
        return; //TODOreq: return false. even though shit, it's called from a constructor. maybe just set up an initFailed flag and throw an error?

    m_RpcBankServer->moveBackendBusinessObjectsToTheirOwnThreadsAndStartTheThreads();
}
