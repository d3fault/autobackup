#include "bitcoinhelperandbroadcastmessagedispenseruser.h"

#include "messageDispensers/broadcastdispensers.h"
#include "messageDispensers/broadcasts/pendingbalanceaddeddetectedmessagedispenser.h"
#include "messages/broadcasts/pendingbalanceaddeddetectedmessage.h"

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

    emit d("starting debug timer");
}
void BitcoinHelperAndBroadcastMessageDispenserUser::handleDebugTimerTimeout()
{
    emit d("timer timed out");
    if(qrand() % 10 == 3)
    {
        emit d("wewt got 3. dispatching broadcast message now");
        PendingBalanceAddedDetectedMessage *pendingBalanceAddedDetectedMessage = m_PendingBalanceAddedDetectedMessageDispenser->getNewOrRecycled();
        pendingBalanceAddedDetectedMessage->myDeliver();
    }
}
