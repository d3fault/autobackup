#include "bankserverclienttest.h"

BankServerClientTest::BankServerClientTest()
{
    m_BankServerClient = new BankServerClient();
    m_RpcBankServerHelper = new RpcBankServerHelper(m_BankServerClient);

    connect(m_RpcBankServerHelper, SIGNAL(initialized()), this, SIGNAL(initialized()));
    connect(m_RpcBankServerHelper, SIGNAL(started()), this, SIGNAL(started()));
    connect(m_RpcBankServerHelper, SIGNAL(stopped()), this, SIGNAL(stopped()));

    connect(m_BankServerClient, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
    connect(m_RpcBankServerHelper, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
}
void BankServerClientTest::init()
{
    m_RpcBankServerHelper->init();
}
void BankServerClientTest::start()
{
    m_RpcBankServerHelper->start();
}
void BankServerClientTest::stop()
{
    m_RpcBankServerHelper->stop();
}
void BankServerClientTest::simulateCreateBankAccountAction()
{
    QMetaObject::invokeMethod(m_BankServerClient, "simulateCreateBankAccountAction", Qt::QueuedConnection);
}
void BankServerClientTest::simulateGetAddFundsKeyAction()
{
    QMetaObject::invokeMethod(m_BankServerClient, "simulateGetAddFundsKeyAction", Qt::QueuedConnection);
}
