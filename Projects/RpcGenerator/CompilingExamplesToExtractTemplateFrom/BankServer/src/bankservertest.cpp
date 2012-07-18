#include "bankservertest.h"

BankServerTest::BankServerTest()
{
    m_BankServer = new BankServer();
    m_RpcBankServerClientsHelper = new RpcBankServerClientsHelper(m_BankServer);
}
void BankServerTest::init()
{
}
void BankServerTest::start()
{
}
void BankServerTest::stop()
{
}
