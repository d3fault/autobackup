#include "bankservertest.h"

BankServerTest::BankServerTest()
{
    m_BankServer = new BankServer();
    m_RpcBankServerClientsHelper = new RpcBankServerClientsHelper(m_BankServer);

    connect(m_RpcBankServerClientsHelper, SIGNAL(initialized()), this, SIGNAL(initialized()));
    connect(m_RpcBankServerClientsHelper, SIGNAL(started()), this, SIGNAL(started()));
    connect(m_RpcBankServerClientsHelper, SIGNAL(stopped()), this, SIGNAL(stopped()));
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
