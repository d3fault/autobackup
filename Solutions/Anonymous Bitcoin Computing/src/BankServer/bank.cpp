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
    if(!m_PerAppBankAccountsHash.value(appId).hasBankAccountFor(username))
    {
        if(m_PerAppBankAccountsHash.value(appId).addBankAccount(username))
        {
            emit bankAccountCreated(username);
            return;
        }
    }

    //todo: race condition somehow not detected by appdb (i think it's good that we double check. just because appdb is a cache does not mean we should rely on it. it answers most of the race condition requests for us but *might* miss some. it certainly should not be the finaly say of who gets to do what. we need a final check on the bank server. here it is (above: hasBankAccountFor))
    //shared race condition detection code {appdb && bank}: ---- would also imply shared "doAction" (shift variables in database for most examples). KISS... but also design well. a delicate balance
    //if the appdb were to use the same code, we could basically have duplicate code completely for the bank code and the bankhelper code
    //they both are essentially the same thing
    //they both receive bankActionRequests
    //and they both reply with bankActionResponse's
    //the bankHelper in the appdb is what holds the db of cached bank values
    //this is NOT the same database as in AppLogic (who owns what, variables of slots, the index, etc)
    //for an ad campaign, there is lastBuyPrice and lastBuyTime. from this + the user's minimumSlotPrice for the given campaign, we can calculate (and re-calculate in javascript) the current price and when it will hit the minimum. we want it to hit the minimum at exactly when the last ad slot that was purchased expires. it doubles at each slot purchase, and the rate at which it declines back to minimumSlotPrice is calculated and populated/supplied to js code on the webserver so the webserver can keep the user up to date without ever coming back to the [web]server or receiving a push/update from the [web]server

    //todo: error where addBankAccount fails (db errors?)


    emit bankAccountCreationFailed();
}
void Bank::handleBalanceTransferRequested(const QString &username, double amount)
{
}