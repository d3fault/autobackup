#include "bitcoinhelperandbroadcastmessagedispenseruser.h"

#include "messageDispensers/broadcastdispensers.h"
#include "messageDispensers/broadcasts/pendingbalanceaddeddetectedmessagedispenser.h"
#include "messages/broadcasts/pendingbalanceaddeddetectedmessage.h"

BitcoinHelperAndBroadcastMessageDispenserUser::BitcoinHelperAndBroadcastMessageDispenserUser()
    : m_DebugTimer(0)
{ }
void BitcoinHelperAndBroadcastMessageDispenserUser::grabBroadcastDispensersAndMoveEachToOurThread(BroadcastDispensers *broadcastDispensers, QThread *thread)
{
    m_PendingBalanceAddedDetectedMessageDispenser = broadcastDispensers->pendingBalanceAddedDetectedMessageDispenser();
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

    emit d(QString("starting timer on what should be the bitcoin thread: ") + QString::number(QThread::currentThreadId()));
    emit d("starting debug timer");
}
void BitcoinHelperAndBroadcastMessageDispenserUser::handleDebugTimerTimeout()
{
    if(qrand() % 10 == 3)
    {
        emit d(QString("timer timed out and we got 3 on what should be the bitcoin thread: ") + QString::number(QThread::currentThreadId()));
        PendingBalanceAddedDetectedMessage *pendingBalanceAddedDetectedMessage = m_PendingBalanceAddedDetectedMessageDispenser->getNewOrRecycled();
        pendingBalanceAddedDetectedMessage->myDeliver();
    }
}
