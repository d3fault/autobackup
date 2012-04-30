#include "bankserverhelper.h"

BankServerHelper::BankServerHelper(QObject *parent) :
    QObject(parent)
{
}
void BankServerHelper::connectToBankServer()
{
    //TODO
    emit connectedToBankServer();
}
void BankServerHelper::handleBankServerActionRequest(BankServerActionRequest *request)
{
    //TODO: synchronously send the request over our ssltcpclient (to be coded)
    //i'm thinking in _THIS_ class is where the 'pending requests' list should go...
    //oh fuck... wait a minute... doesn't our 2nd race condition need to check against that list??? oh fuck. maybe uhmm store it in a list in the AppLogic side JUST BEFORE emitting it over to us??
}
