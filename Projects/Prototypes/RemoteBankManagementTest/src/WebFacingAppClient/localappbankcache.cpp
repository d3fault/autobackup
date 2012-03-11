#include "localappbankcache.h"

LocalAppBankCache::LocalAppBankCache(QObject *parent) :
    QObject(parent), m_BankServer(0)
{
}
void LocalAppBankCache::addUser(QString userNameToAdd)
{
    m_BankServer->addUser(userNameToAdd); //it makes sure it's connected (or waits until it is) for us
}
void LocalAppBankCache::handleUserAdded(QString newUser)
{
    //remote has added the user and signal'd us, so now we add it to our local cache and then signal the gui
    m_Db.addUser(newUser);

    emit userAdded(newUser);
}
void LocalAppBankCache::init()
{
    if(!m_BankServer)
    {
        m_BankServer = new RemoteBankServerHelper(); //starts async connecting
        //todo: this is where i should invokeMethod the async connection starting (so i can listen to the emit d()'s in the connecting phase. oh actually i should move up the connect to SIGNAL d() below to above this invokeMethod. not high priority
        connect(m_BankServer, SIGNAL(userAdded(QString)), this, SLOT(handleUserAdded(QString)));
        connect(m_BankServer, SIGNAL(addFundsKeyReceived(QString,QString)), this, SLOT(handleAddFundsKeyReceived(QString,QString)));
        connect(m_BankServer, SIGNAL(pendingPaymentReceived(QString,QString,double)), this, SLOT(handlePendingPaymentReceived(QString,QString,double)));
        connect(m_BankServer, SIGNAL(confirmedPaymentReceived(QString,QString,double)), this, SLOT(handleConfirmedPaymentReceived(QString,QString,double)));

        connect(m_BankServer, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
        connect(&m_Db, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
    }
}
void LocalAppBankCache::addFundsRequest(QString user)
{
    //'pending' (bitcoin) = say wait 10 mins for confirmed bro
    //neither pending or confirmed = give em same key again
    //empty OR confirmed = give em new

    //only when getting a new one do we need to touch the server

    //normally, in the impl, i'm going to want to propagate the result up to the gui properly/as an object or something (to be displayed in a custom part of the gui)... but since this is a prototype, i'm just going to use emit d() to a) say '10 mins bro', b) give em the same key (a message saying they need to use this one first would be nice), and c) give them their new key

    if(m_Db.hasAddFundRequestBitcoinKey(user))
    {
        if(m_Db.addFundsRequestBitcoinKeyHasNoUpdates(user))
        {
            //they haven't used their key, so we give them back the same one
            emit d("you need to use your existing key first: '" + m_Db.getExistingAddFundsKey(user) + "'");
        }
        else if(m_Db.addFundsRequestBitcoinKeyIsPendingConfirm(user))
        {
            //they've made a payment, but it hasn't been ~10 minutes for the confirmation yet. just tell them to wait.
            emit d("we've received your payment, but we need to wait for it to be confirmed before you can start another transaction. on average, this takes about 10 minutes");
        }
        else if(m_Db.addFundsRequestBitcoinKeyIsConfirmed(user))
        {
            //their previous payment has been confirmed, so we can issue them a new one
            userNeedsNewBitcoinAddFundsKey(user);
        }
    }
    else
    {
        //this is their first time adding funds, so just give them a key
        userNeedsNewBitcoinAddFundsKey(user);
    }
}
void LocalAppBankCache::userNeedsNewBitcoinAddFundsKey(QString user)
{
    //send a request to the server for a new key. when we get it back, update the db and present it to the user

    m_BankServer->getNewAddFundsKey(user);
}
void LocalAppBankCache::handleAddFundsKeyReceived(QString user, QString newKey)
{
    //slot called from the server when a key is sent back to us
    m_Db.setFundsRequestBitcoinKey(user, newKey);

    //TODO: in impl, we'll use the signal to notify them in a special place in teh gui.. but for here/now, emit d()'ing the new key up is good enough. i still want to define the signals that i will be using (and leave them commented out)
    emit d("here's the new add funds key for user '" + user + "': '" + newKey + "'");
    //emit addFundsKeyReceived(user, newKey);

    //semi-todo: we might use the addFundsKeyReceived signal where we shouldn't, for when we return an existing/awaiting/unused key to a user. but then we won't be able to say 'hey use this key first bitch' so maybe not...
}
void LocalAppBankCache::handlePendingPaymentReceived(QString user, QString key, double pendingAmount)
{
    //update our local db/cache
    m_Db.pendindAmountReceived(user, pendingAmount);

    //TODO: just like handleAddFundsKeyReceived above, we're just goint to use emit d() to push the notification to the mainwebsitebankview.cpp... but in impl it'd be a special signal (probably)
    emit d(user + " received " + QString::number(pendingAmount,'f',8) + " -PENDING- @ " + key);
}
void LocalAppBankCache::handleConfirmedPaymentReceived(QString user, QString key, double confirmedAmount)
{
    //todo: see pending sister function's TODOs
    m_Db.confirmedAmountReceived(user, confirmedAmount);
    emit d(user + " received " + QString::number(confirmedAmount,'f',8) + " -CONFIRMED- @ " + key);
}
