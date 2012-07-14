#include "bitcoinhelper.h"

BitcoinHelper::BitcoinHelper()
{
    /*
    m_PendingBalanceAddedMessageDispenser = broadcastDispensers->pendingBalanceAddedMessageDispenser(); //i think all the dispensers on broadcastDispensers are null right now because i need to set 'this' as the owner when new'ing them... and i am still in my own constructor... so it's a reasonable conclusion. fml.
    */
    //leaving comment in here to explain the refactor
}
void BitcoinHelper::init()
{
    emit initialized();
}
void BitcoinHelper::start()
{
    emit started();
}
void BitcoinHelper::stop()
{
    emit stopped();
}
void BitcoinHelper::takeOwnershipOfApplicableBroadcastDispensers(RpcBankServerClientsHelper *rpcBankServerClientsHelper)
{
   m_PendingBalanceAddedMessageDispenser = rpcBankServerClientsHelper->takeOwnershipOfPendingBalanceAddedMessageDispenserRiggedForDelivery(this);
}
