#include "rpcbitcoinhelper.h"

#include <QDateTime> //debug

RpcBitcoinHelper::RpcBitcoinHelper(QObject *parent)
    : QObject(parent)
{ }
void RpcBitcoinHelper::initialize(RpcBankServerClientsHelper *rpcBankServerClientsHelper)
{
    m_PendingBalanceDetectedMessageDispenser = rpcBankServerClientsHelper->broadcastDispensers()->takeOwnershipOfPendingBalanceDetectedMessageDispenserRiggedForDelivery(this);
    m_ConfirmedBalanceDetectedMessageDispenser = rpcBankServerClientsHelper->broadcastDispensers()->takeOwnershipOfConfirmedBalanceDetectedMessageDispenserRiggedForDelivery(this);

    emit initialized();
}
void RpcBitcoinHelper::start()
{
    emit d("BitcoinHelper received start message");
    emit started();
}
void RpcBitcoinHelper::stop()
{
    emit d("BitcoinHelper received stop message");
    emit stopped();
}
void RpcBitcoinHelper::simulatePendingBalanceDetectedBroadcast()
{
    ServerPendingBalanceDetectedMessage *pendingBalanceDetectedMessage = m_PendingBalanceDetectedMessageDispenser->getNewOrRecycled();
    pendingBalanceDetectedMessage->Username = (QString("randomUsername@") + QDateTime::currentDateTime().toString());
    pendingBalanceDetectedMessage->PendingBalance = 69.420374;
    emit d(QString("SIMULATING pending balance detected broadcast for user: ") + pendingBalanceDetectedMessage->Username);
    pendingBalanceDetectedMessage->deliver();
}
void RpcBitcoinHelper::simulateConfirmedBalanceDetectedBroadcast()
{
    ServerConfirmedBalanceDetectedMessage *confirmedBalanceDetectedMessage = m_ConfirmedBalanceDetectedMessageDispenser->getNewOrRecycled();
    confirmedBalanceDetectedMessage->Username = (QString("randomUsername@") + QDateTime::currentDateTime().toString());
    confirmedBalanceDetectedMessage->ConfirmedBalance = 111.23456;
    emit d(QString("SIMULATING confirmed balance detected broadcast for user: ") + confirmedBalanceDetectedMessage->Username);
    confirmedBalanceDetectedMessage->deliver();
}
