#include "irpcbankserverbusiness.h"

void IRpcBankServerBusiness::setActionDispensers(RpcBankServerActionDispensers *actionDispensers)
{
    m_ActionDispensers = actionDispensers;
}
RpcBankServerActionDispensers *IRpcBankServerBusiness::actionDispensers()
{
    return m_ActionDispensers;
}
void IRpcBankServerBusiness::setBroadcastDispensers(RpcBankServerBroadcastDispensers *broadcastDispensers)
{
    m_BroadcastDispensers = broadcastDispensers;
}
RpcBankServerBroadcastDispensers *IRpcBankServerBusiness::broadcastDispensers()
{
    return m_BroadcastDispensers;
}
