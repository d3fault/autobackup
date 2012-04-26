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
}
