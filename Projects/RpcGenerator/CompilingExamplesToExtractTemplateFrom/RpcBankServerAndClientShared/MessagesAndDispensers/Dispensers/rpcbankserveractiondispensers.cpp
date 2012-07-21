#include "rpcbankserveractiondispensers.h"

RpcBankServerActionDispensers::RpcBankServerActionDispensers(QObject *destination)
    : m_Destination(destination)
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
