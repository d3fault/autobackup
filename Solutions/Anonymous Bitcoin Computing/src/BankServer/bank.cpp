#include "bank.h"

Bank::Bank(QObject *parent) :
    QObject(parent)
{
}
void Bank::init()
{
    //todo: init sql db i guess

    emit initialized();
}
void Bank::handleBankAccountCreationRequested(const QString &username)
{
}
void Bank::handleBalanceTransferRequested(const QString &username, double amount)
{
}
