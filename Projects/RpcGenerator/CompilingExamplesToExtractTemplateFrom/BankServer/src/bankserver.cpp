#include "bankserver.h"

BankServer::BankServer()
{
    m_Bitcoin = new BitcoinHelper();

    connect(m_Bitcoin, SIGNAL(initialized()), this, SIGNAL(initialized()));
    connect(m_Bitcoin, SIGNAL(started()), this, SIGNAL(started()));
    connect(m_Bitcoin, SIGNAL(stopped()), this, SIGNAL(stopped()));
}
void BankServer::instructBackendObjectsToClaimRelevantDispensers()
{
    m_Bitcoin->takeOwnershipOfApplicableBroadcastDispensers(m_BroadcastDispensers);
}
void BankServer::moveBackendBusinessObjectsToTheirOwnThreadsAndStartTheThreads()
{
    m_BitcoinThread = new QThread();
    m_Bitcoin->moveToThread(m_BitcoinThread);
    m_BitcoinThread->start();
}
void BankServer::init()
{
    QMetaObject::invokeMethod(m_Bitcoin, "init", Qt::QueuedConnection);
}
void BankServer::start()
{
    QMetaObject::invokeMethod(m_Bitcoin, "start", Qt::QueuedConnection);
}
void BankServer::stop()
{
    QMetaObject::invokeMethod(m_Bitcoin, "stop", Qt::QueuedConnection);
}
void BankServer::createBankAccount(CreateBankAccountMessage *createBankAccountMessage)
{
}
void BankServer::getAddFundsKey(GetAddFundsKeyMessage *getAddFundsKeyMessage)
{
}

