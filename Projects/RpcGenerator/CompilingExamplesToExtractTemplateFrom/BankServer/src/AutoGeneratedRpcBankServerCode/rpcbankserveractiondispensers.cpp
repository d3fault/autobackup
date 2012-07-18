#include "rpcbankserveractiondispensers.h"

RpcBankServerActionDispensers::RpcBankServerActionDispensers(IBankServerProtocolKnower *destination)
    : m_Destination(destination)
{
}
CreateBankAccountMessageDispenser *RpcBankServerActionDispensers::takeOwnershipOfCreateBankAccountMessageDispenserRiggedForDelivery(QObject *owner)
{
    m_CreateBankAccountMessageDispenser = new CreateBankAccountMessageDispenser(m_Destination, owner);
}
GetAddFundsKeyMessageDispenser * RpcBankServerActionDispensers::takeOwnershipOfGetAddFundsKeyMessageDispenserRiggedForDelivery(QObject *owner)
{
}
