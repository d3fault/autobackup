#include "bankserver.h"

BankServer::BankServer()
{
    m_Bitcoin = new BitcoinHelper();
    //new impl backend objects, rig up connections etc to init/start/stop to/from backend objects
    //do not start them in any way or move their thread yet. do that in provided pure virtual
}
void BankServer::init()
{
}
void BankServer::start()
{
}
void BankServer::stop()
{
}
void BankServer::createBankAccount(CreateBankAccountMessage *createBankAccountMessage)
{
}
void BankServer::getAddFundsKey(GetAddFundsKeyMessage *getAddFundsKeyMessage)
{
}
void BankServer::instructBackendObjectsToClaimRelevantDispensers()
{
    m_Bitcoin->takeOwnershipOfApplicableBroadcastDispensers(m_BroadcastDispensers);
}
void BankServer::moveBackendBusinessObjectsToTheirOwnThreadsAndStartTheThreads()
{
}
