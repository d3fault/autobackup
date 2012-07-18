#include "bitcoinhelper.h"

BitcoinHelper::BitcoinHelper()
{
}
void BitcoinHelper::takeOwnershipOfApplicableBroadcastDispensers(RpcBankServerBroadcastDispensers *broadcastDispensers)
{
    m_PendingBalanceDetectedMessageDispenser = broadcastDispensers->takeOwnershipOfPendingBalanceDetectedMessageDispenserRiggedForDelivery(this);
    m_ConfirmedBalanceDetectedMessageDispenser = broadcastDispensers->takeOwnershipOfConfirmedBalanceDetectedMessageDispenserRiggedForDelivery(this);
}
