#include "bitcoinhelper.h"

BitcoinHelper::BitcoinHelper()
{
}
void BitcoinHelper::takeOwnershipOfApplicableBroadcastDispensers(RpcBankServerBroadcastDispensers *broadcastDispensers)
{
    m_PendingBalanceDetectedMessageDispenser = broadcastDispensers->takeOwnershipOfPendingBalanceDetectedMessageDispenserRiggedForDelivery(this);
    m_ConfirmedBalanceDetectedMessageDispenser = broadcastDispensers->takeOwnershipOfConfirmedBalanceDetectedMessageDispenserRiggedForDelivery(this);
}
void BitcoinHelper::init()
{
    emit d("BitcoinHelper received init message");
    emit initialized();
}
void BitcoinHelper::start()
{
    emit d("BitcoinHelper received start message");
    emit started();
}
void BitcoinHelper::stop()
{
    emit d("BitcoinHelper received stop message");
    emit stopped();
}
