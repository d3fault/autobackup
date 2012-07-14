#include "bitcoinhelper.h"

BitcoinHelper::BitcoinHelper()
    : m_Timer(0), m_I(0.0)
{
    /*
    m_PendingBalanceAddedMessageDispenser = broadcastDispensers->pendingBalanceAddedMessageDispenser(); //i think all the dispensers on broadcastDispensers are null right now because i need to set 'this' as the owner when new'ing them... and i am still in my own constructor... so it's a reasonable conclusion. fml.
    */
    //leaving comment in here to explain the refactor
}
void BitcoinHelper::init()
{
    if(!m_Timer)
    {
        m_Timer = new QTimer(this); //i think parent might matter as for what thread the timer operates on
        connect(m_Timer, SIGNAL(timeout()), this, SLOT(handleTimeout()));

        emit d(QString("receiving init on what should be the bitcoin thread: ") + QString::number(QThread::currentThreadId()));
        emit initialized();
    }
}
void BitcoinHelper::start()
{
    if(!m_Timer->isActive())
    {
        m_Timer->start(500);

        emit d(QString("receiving start on what should be the bitcoin thread: ") + QString::number(QThread::currentThreadId()));
        emit started();
    }
}
void BitcoinHelper::stop()
{
    if(m_Timer->isActive())
    {
        m_Timer->stop();

        emit d(QString("receiving stop on what should be the bitcoin thread: ") + QString::number(QThread::currentThreadId()));
        emit stopped();
    }
}
void BitcoinHelper::takeOwnershipOfApplicableBroadcastDispensers(RpcBankServerClientsHelper *rpcBankServerClientsHelper)
{
    m_PendingBalanceAddedMessageDispenser = rpcBankServerClientsHelper->takeOwnershipOfPendingBalanceAddedMessageDispenserRiggedForDelivery(this);
    connect(m_PendingBalanceAddedMessageDispenser, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
}
void BitcoinHelper::handleTimeout()
{
    if(qrand() % 10 == 3)
    {
        PendingBalanceAddedMessage *pbam = m_PendingBalanceAddedMessageDispenser->getNewOrRecycled();
        pbam->Username = QString("pbamUser@") + QDateTime::currentDateTime().toString();
        pbam->PendingBalance = ++m_I;

        emit d(QString("got a 3 in pending balance lottery. should be on bitcoin thread: ") + QString::number(QThread::currentThreadId()));

        pbam->deliver();
    }
}
