#include "bitcoinhelper.h"

BitcoinHelper::BitcoinHelper()
{
}
void BitcoinHelper::takeOwnershipOfApplicableBroadcastDispensers(BroadcastDispensers *broadcastDispensers)
{
    m_PendingBalanceAddedMessageDispenser = broadcastDispensers->takeOwnershipOfPendingBalanceAddedMessageDispenserRiggedForDelivery(this);
}
