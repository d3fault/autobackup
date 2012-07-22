#include "bankserverclienttest.h"

BankServerClientTest::BankServerClientTest()
{
    m_BankClient = new BankServerClient();
    m_RpcBankServerHelper = new RpcBankServerHelper(m_BankClient);

    connect(m_RpcBankServerHelper, SIGNAL(initialized()), this, SIGNAL(initialized()));
    connect(m_RpcBankServerHelper, SIGNAL(started()), this, SIGNAL(started()));
    connect(m_RpcBankServerHelper, SIGNAL(stopped()), this, SIGNAL(stopped()));
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
}
void BankServerClientTest::simulateGetAddFundsKeyAction()
{
}
