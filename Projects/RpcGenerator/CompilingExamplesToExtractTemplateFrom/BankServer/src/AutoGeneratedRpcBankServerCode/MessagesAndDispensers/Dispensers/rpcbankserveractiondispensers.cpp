#include "rpcbankserveractiondispensers.h"

RpcBankServerActionDispensers::RpcBankServerActionDispensers(IAcceptRpcBankServerActionAndActionErrorAndBroadcastMessageDeliveries *destination)
    : m_Destination(destination)
{ }
ServerCreateBankAccountMessageDispenser *RpcBankServerActionDispensers::takeOwnershipOfCreateBankAccountMessageDispenserRiggedForDelivery(QObject *owner)
{
    m_CreateBankAccountMessageDispenser = new ServerCreateBankAccountMessageDispenser(m_Destination, owner);
    return m_CreateBankAccountMessageDispenser;
}
ServerGetAddFundsKeyMessageDispenser * RpcBankServerActionDispensers::takeOwnershipOfGetAddFundsKeyMessageDispenserRiggedForDelivery(QObject *owner)
{
    m_GetAddFundsKeyMessageDispenser = new ServerGetAddFundsKeyMessageDispenser(m_Destination, owner);
    return m_GetAddFundsKeyMessageDispenser;
}
