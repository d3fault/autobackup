#include "broadcastdispensers.h"

#include "irpcclientshelper.h"
#include "broadcasts/pendingbalanceaddeddetectedmessagedispenser.h"

BroadcastDispensers::BroadcastDispensers(IRpcClientsHelper *destinationObject)
{
    m_PendingBalanceAddedDetectedMessageDispenser = new PendingBalanceAddedDetectedMessageDispenser(destinationObject);
}
PendingBalanceAddedDetectedMessageDispenser * BroadcastDispensers::pendingBalanceAddedDetectedMessageDispenser()
{
    return m_PendingBalanceAddedDetectedMessageDispenser;
}
