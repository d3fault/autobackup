#include "rpcbankserverclientactiondispensers.h"

RpcBankServerClientActionDispensers::RpcBankServerClientActionDispensers(IAcceptRpcBankServerActionDeliveries *destination)
    : m_Destination(destination), m_CreateBankAccountMessageDispenser(0), m_GetAddFundsKeyMessageDispenser(0)
{ }
ClientCreateBankAccountMessageDispenser *RpcBankServerClientActionDispensers::takeOwnershipOfCreateBankAccountMessageDispenserRiggedForDelivery(QObject *owner)
{
    m_CreateBankAccountMessageDispenser = new ClientCreateBankAccountMessageDispenser(m_Destination, owner);
    return m_CreateBankAccountMessageDispenser;
}
ClientGetAddFundsKeyMessageDispenser * RpcBankServerClientActionDispensers::takeOwnershipOfGetAddFundsKeyMessageDispenserRiggedForDelivery(QObject *owner)
{
    m_GetAddFundsKeyMessageDispenser = new ClientGetAddFundsKeyMessageDispenser(m_Destination, owner);
    return m_GetAddFundsKeyMessageDispenser;
}
bool RpcBankServerClientActionDispensers::everyDispenserIsCreated()
{
    if(!m_CreateBankAccountMessageDispenser)
        return false;
    if(!m_GetAddFundsKeyMessageDispenser)
        return false;

    return true;
}
