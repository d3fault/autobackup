#include "rpcbankserveractiondispensers.h"

RpcBankServerActionDispensers::RpcBankServerActionDispensers(QObject *destination)
    : m_Destination(destination), m_CreateBankAccountMessageDispenser(0), m_GetAddFundsKeyMessageDispenser(0)
{ }
CreateBankAccountMessageDispenser *RpcBankServerActionDispensers::takeOwnershipOfCreateBankAccountMessageDispenserRiggedForDelivery(QObject *owner)
{
    m_CreateBankAccountMessageDispenser = new CreateBankAccountMessageDispenser(m_Destination, owner);
    return m_CreateBankAccountMessageDispenser;
}
GetAddFundsKeyMessageDispenser * RpcBankServerActionDispensers::takeOwnershipOfGetAddFundsKeyMessageDispenserRiggedForDelivery(QObject *owner)
{
    m_GetAddFundsKeyMessageDispenser = new GetAddFundsKeyMessageDispenser(m_Destination, owner);
    return m_GetAddFundsKeyMessageDispenser;
}
bool RpcBankServerActionDispensers::everyDispenserIsCreated()
{
    if(!m_CreateBankAccountMessageDispenser)
        return false;
    if(!m_GetAddFundsKeyMessageDispenser)
        return false;

    return true;
}
