#include "bankservertest.h"

BankServerTest::BankServerTest()
{
    m_BankServer = new BankServer();
    m_RpcBankServerClientsHelper = new RpcBankServerClientsHelper(m_BankServer);


    connect(m_RpcBankServerClientsHelper, SIGNAL(initialized()), this, SIGNAL(initialized()));
    connect(m_RpcBankServerClientsHelper, SIGNAL(started()), this, SIGNAL(started()));
    connect(m_RpcBankServerClientsHelper, SIGNAL(stopped()), this, SIGNAL(stopped()));

    connect(m_BankServer, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
    connect(m_RpcBankServerClientsHelper, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
}
void BankServerTest::init()
{
    m_RpcBankServerClientsHelper->init();
}
void BankServerTest::start()
{
    m_RpcBankServerClientsHelper->start();
}
void BankServerTest::stop()
{
    m_RpcBankServerClientsHelper->stop();
}
void BankServerTest::simulatePendingBalanceDetectedBroadcast()
{
    QMetaObject::invokeMethod(m_BankServer, "simulatePendingBalanceDetectedBroadcast", Qt::QueuedConnection);
}
void BankServerTest::simulateConfirmedBalanceDetectedBroadcast()
{
    QMetaObject::invokeMethod(m_BankServer, "simulateConfirmedBalanceDetectedBroadcast", Qt::QueuedConnection);
}
