#include "bankdb.h"

BankDb::BankDb(QObject *parent) :
    QObject(parent)
{
}
void BankDb::addUser(const QString &appId, const QString &userName)
{
    if(m_Db.contains(appId))
    {
        QHash<QString, UserBankAccount*> listOfAccountsForThisAppId = m_Db.value(appId);
        //todo: check that this doesn't extract a hardcopy that is then modified... that we're in fact modifying the list that lives in m_Db
        if(listOfAccountsForThisAppId.contains(userName))
        {
            //todo: propagate error properly (back to client)
            emit d("user " + userName + " is already in db");
            return;
        }
        UserBankAccount *newAccount = new UserBankAccount();
        newAccount->AddFundsBitcoinKey = DEFAULT_EMPTY_ADD_FUNDS_BITCOIN_KEY; //TODO: our bitcoin polling loop has to check that the bitcoin key is NOT this 'default'. it can't just check every Awaiting because when the user is first created, before they try to add funds, we are in a state where they don't have a key but they ARE at AwaitingPayment. i could add another enum but it doesn't matter how you do it
        newAccount->Balance = 0.0;
        newAccount->AddFundsStatus = UserBankAccount::AwaitingPayment;

        listOfAccountsForThisAppId.insert(userName, newAccount);
    }
    else
    {
        //since appId doesn't exist, we know username doesn't already exist
        QHash<QString, UserBankAccount*> newListOfAccountsForThisAppId; //new because the list doesn't yet exist since the appId doesn't yet exist
        UserBankAccount *newAccount = new UserBankAccount();
        newAccount->AddFundsBitcoinKey = DEFAULT_EMPTY_ADD_FUNDS_BITCOIN_KEY;
        newAccount->Balance = 0.0;
        newAccount->AddFundsStatus = UserBankAccount::AwaitingPayment;

        //re: TODOreq below.. this is also duplicate code that is growing... so moving it to constructor is definitely a good idea

        newListOfAccountsForThisAppId.insert(userName, newAccount);
        m_Db.insert(appId, newListOfAccountsForThisAppId);

        //TODOreq: the appId's should not be inserted here... they should be inserted in the constructor. if we make it to this else, we should respond with the error saying INVALID_APP_ID. for now/simple-testing, we'll just add it. i'm not sure if we can have an appId with a zero size VALUE (another hash of user accounts)... and i don't care to test that right now..
        //additionally, this will NOT be in the constructor in the impl... it will be in/near like the CREATE statement for the db or some shit... (we MIGHT add the ability to add an appId without recompiling later.... but that's low priority atm)
    }
}
void BankDb::setAddFundsKey(const QString &appId, const QString &userName, const QString &newKey)
{
    if(m_Db.contains(appId))
    {
        QHash<QString, UserBankAccount*> listOfAccountsForThisAppId = m_Db.value(appId);
        if(listOfAccountsForThisAppId.contains(userName))
        {
            UserBankAccount *userAccount = listOfAccountsForThisAppId.value(userName);
            userAccount->AddFundsBitcoinKey = newKey;
            userAccount->AddFundsStatus = UserBankAccount::AwaitingPayment;
        }
        else
        {
            emit d("error in setAddFundsKey, username not found in db");
        }
    }
    else
    {
        emit d("error in setAddFundsKey, appId not found in db");
    }
}
