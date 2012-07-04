#include "bitcoinhelperandbroadcastmessagedispenseruser.h"

BitcoinHelperAndBroadcastMessageDispenserUser::BitcoinHelperAndBroadcastMessageDispenserUser(BroadcastDispensers *broadcastDispensers)
    : m_DebugTimer(0)
{
    m_PendingBalanceAddedDetectedMessageDispenser = broadcastDispensers->pendingBalanceAddedDetectedMessageDispenser();

    //TODOreq: move the dispenser object to the thread that we are on
}
void BitcoinHelperAndBroadcastMessageDispenserUser::moveBroadcastDispensersToThread(QThread *thread)
{
    m_PendingBalanceAddedDetectedMessageDispenser->moveToThread(thread);
    //etc
}
void BitcoinHelperAndBroadcastMessageDispenserUser::startDebugTimer()
{
    if(!m_DebugTimer)
    {
        m_DebugTimer = new QTimer();
        connect(m_DebugTimer, SIGNAL(timeout()), this, SLOT(handleDebugTimerTimeout()));
    }
    m_DebugTimer->start(500);
}
void BitcoinHelperAndBroadcastMessageDispenserUser::handleDebugTimerTimeout()
{
    if(qrand() % 10 == 3)
    {
        PendingBalanceAddedDetectedMessage *pendingBalanceAddedDetectedMessage = m_PendingBalanceAddedDetectedMessageDispenser->getNewOrRecycled();
        pendingBalanceAddedDetectedMessage->setDetectedBalanceAmountEtc(5);
        pendingBalanceAddedDetectedMessage->deliver();
    }
}
