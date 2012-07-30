#include "bitcoinhelper.h"

#include <QDateTime> //debug

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
void BitcoinHelper::simulatePendingBalanceDetectedBroadcast()
{
    ServerPendingBalanceDetectedMessage *pendingBalanceDetectedMessage = m_PendingBalanceDetectedMessageDispenser->getNewOrRecycled();
    pendingBalanceDetectedMessage->Username = (QString("randomUsername@") + QDateTime::currentDateTime().toString());
    pendingBalanceDetectedMessage->PendingBalance = 69.420374;
    emit d(QString("SIMULATING pending balance detected broadcast for user: ") + pendingBalanceDetectedMessage->Username);
    pendingBalanceDetectedMessage->deliver();
}
void BitcoinHelper::simulateConfirmedBalanceDetectedBroadcast()
{
    ServerConfirmedBalanceDetectedMessage *confirmedBalanceDetectedMessage = m_ConfirmedBalanceDetectedMessageDispenser->getNewOrRecycled();
    confirmedBalanceDetectedMessage->Username = (QString("randomUsername@") + QDateTime::currentDateTime().toString());
    confirmedBalanceDetectedMessage->ConfirmedBalance = 111.23456;
    emit d(QString("SIMULATING confirmed balance detected broadcast for user: ") + confirmedBalanceDetectedMessage->Username);
    confirmedBalanceDetectedMessage->deliver();
}
