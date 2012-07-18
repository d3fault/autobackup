#include "irpcbankserver.h"

void IRpcBankServer::setActionDispensers(ActionDispensers *actionDispensers)
{
    m_ActionDispensers = actionDispensers;
}
ActionDispensers *IRpcBankServer::actionDispensers()
{
    return m_ActionDispensers;
}
void IRpcBankServer::setBroadcastDispensers(BroadcastDispensers *broadcastDispensers)
{
    m_BroadcastDispensers = broadcastDispensers;
}
BroadcastDispensers *IRpcBankServer::broadcastDispensers()
{
    return m_BroadcastDispensers;
}
