#include "bitcoinhelper.h"

BitcoinHelper::BitcoinHelper()
{
}
void BitcoinHelper::takeOwnershipOfApplicableBroadcastDispensers(BroadcastDispensers *broadcastDispensers)
{
    m_PendingBalanceDetectedMessageDispenser = broadcastDispensers->takeOwnershipOfPendingBalanceDetectedMessageDispenserRiggedForDelivery(this);
    m_ConfirmedBalanceDetectedMessageDispenser = broadcastDispensers->takeOwnershipOfConfirmedBalanceDetectedMessageDispenserRiggedForDelivery(this);
}
