#include "bankserverclient.h"

BankServerClient::BankServerClient()
{
    //m_AnotherRpcServerSayWtSideOfThings = new QObject(); //rpc client's backend business object?
    //TODO^ having a single project be both a client and server is a future project. don't lose track of your objective
    //simplify it just like the RemoteBankManagementTest did so there's just 2 portions

    //yes, there will be actual objects instantiated here... but i can't think of any for the simplified RemoteBankManagementTest example
}
void BankServerClient::instructBackendObjectsToClaimRelevantDispensers()
{
    //
}
void BankServerClient::moveBackendBusinessObjectsToTheirOwnThreadsAndStartTheThreads()
{
    //
}
void BankServerClient::init()
{
    //TODOmb: if we ever add a backend object, we'd send a queued init to it right here.. and also need a daisyChain() method to get it's initialized() signal

    emit initialized();
}
void BankServerClient::start()
{
    emit started();
}
void BankServerClient::stop()
{
    emit stopped();
}
