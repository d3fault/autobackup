#include "localbankdb.h"

LocalBankDb::LocalBankDb(QObject *parent) :
    QObject(parent)
{ }
void LocalBankDb::addUser(QString newUser)
{
    UserBankAccount *newUserAccount = new UserBankAccount();
    newUserAccount->AddFundsBitcoinKey = DEFAULT_EMPTY_ADD_FUNDS_BITCOIN_KEY;
    newUserAccount->Balance = 0.0;
    m_Db.insert(newUser, newUserAccount);
}
bool LocalBankDb::hasAddFundRequestBitcoinKey(QString user)
{
    bool hasKey = false;
    if(m_Db.contains(user))
    {
        UserBankAccount *userAccount = m_Db.value(user);
        if(userAccount->AddFundsBitcoinKey != DEFAULT_EMPTY_ADD_FUNDS_BITCOIN_KEY)
        {
            hasKey = true;
        }
    }
    return hasKey;
}
bool LocalBankDb::addFundsRequestBitcoinKeyHasNoUpdates(QString user)
{
    if(m_Db.contains(user))
    {
        UserBankAccount *userAccount = m_Db.value(user);
        if(userAccount->AddFundsStatus == UserBankAccount::AwaitingPayment)
        {
            return true;
        }
    }
    else
    {
        emit d("invalid user in hasNoUpdates check");
        return true; //because we definitely don't want to give them a new key... still, this isn't exactly perfect. better error propagation/handling needed. this would be a fatal error obviously
    }

    //false means they either have a pending or confirmed payment
    return false;
}
QString LocalBankDb::getExistingAddFundsKey(QString user)
{
    if(m_Db.contains(user))
    {
        UserBankAccount *userAccount = m_Db.value(user);
        QString key = userAccount->AddFundsBitcoinKey.trimmed();
        if(key.isEmpty())
        {
            return QString("error, existing key was empty");
        }
        else
        {
            return key;
        }
    }
    else
    {
        return QString("error getting existing key");
    }
}
bool LocalBankDb::addFundsRequestBitcoinKeyIsPendingConfirm(QString user)
{
    //sure we could deduce that if this is false that the payment IS confirmed... but i like having the extra step of checking that it is in fact confirmed. one case of that is if m_Db.contains returns false, we too are going to return false. but that doesn't mean it's confirmed!!!

    if(m_Db.contains(user))
    {
        UserBankAccount *userAccount = m_Db.value(user);
        if(userAccount->AddFundsStatus == UserBankAccount::PaymentPending)
        {
            return true;
        }
    }
    else
    {
        emit d("error in isPendingConfirms, user not found");
    }
    return false;
}
bool LocalBankDb::addFundsRequestBitcoinKeyIsConfirmed(QString user)
{
    if(m_Db.contains(user))
    {
        UserBankAccount *userAccount = m_Db.value(user);
        if(userAccount->AddFundsStatus == UserBankAccount::PaymentConfirmed)
        {
            return true;
        }
    }
    else
    {
        emit d("error in keyIsConfirmed, user not found");
    }
    return false;
}
void LocalBankDb::setFundsRequestBitcoinKey(QString user, QString newKey)
{
    if(m_Db.contains(user))
    {
        UserBankAccount *userAccount = m_Db.value(user);
        userAccount->AddFundsBitcoinKey = newKey;
        userAccount->AddFundsStatus = UserBankAccount::AwaitingPayment;
    }
    else
    {
        emit d("error in setFundsRequestBitcoinKey, user not found");
    }
}
void LocalBankDb::pendindAmountReceived(QString user, QString key, double amount)
{
    //i think we need to modify my db... maybe add a pending balance field? maybe keep a list of all transactions (and their pending/confirmed status)?
    //it doesn't make sense to just set the user's balance to the new amount and set their AddFundStatus to pending... because WHAT ABOUT ALL THEIR PREVIOUS SHIT?
}
void LocalBankDb::confirmedAmountReceived(QString user, QString key, double amount)
{
    //for this one, we'll probably just append to the user's balance. easier than pending. but wtf is the point of having the AddFundStatus::Confirmed then?? maybe i should change them to Awaiting and Pending only. but Awaiting really only applies to a key (which is only used once per transaction)... HMMMM... starting to think i should have a list of transactions (the key is the primary key)... might be good for book-keeping too.. but i don't really even want to do book-keeping. i'd rather delete the data :)... but also, if i DO book-keeping, it might allow me to verify account balances before doing payouts (might be hard, but possible)
}
