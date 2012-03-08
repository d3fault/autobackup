#include "bank.h"

Bank::Bank(QObject *parent) :
    QObject(parent), m_Clients(0), m_CurrentlyProcessingPollingLists(false), m_CurrentIndexInListOfAwaitingKeysToPoll(0), m_CurrentIndexInListOfPendingKeysToPoll(0)
{
}
void Bank::start()
{
    if(!m_Clients)
    {
        m_PollingTimer = new QTimer();
        m_PollingTimer->setInterval(TIME_IN_BETWEEN_EACH_POLL);
        connect(m_PollingTimer, SIGNAL(timeout()), this, SLOT(handlePollingTimerTimedOut()));

        m_ServerThread = new QThread();
        m_Clients = new AppClientHelper();
        m_Clients->moveToThread(m_ServerThread);
        m_ServerThread->start();

        connect(m_Clients, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
        connect(&m_Db, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
        connect(&m_Bitcoin, SIGNAL(d(QString)), this, SIGNAL(d(QString)));

        //requests from client
        connect(m_Clients, SIGNAL(addUserRequested(QString,QString)), this, SLOT(handleAddUserRequested(QString,QString)));
        connect(m_Clients, SIGNAL(addFundsKeyRequested(QString,QString)), this, SLOT(handleAddFundsKeyRequested(QString,QString)));

        //responding to requests from client
        connect(this, SIGNAL(userAdded(QString,QString)), m_Clients, SLOT(handleUserAdded(QString,QString)));
        connect(this, SIGNAL(addFundsKeyGenerated(QString,QString,QString)), m_Clients, SLOT(handleAddFundsKeyGenerated(QString,QString,QString)));
        connect(this, SIGNAL(pendingAmountDetected(QString,double)), m_Clients, SLOT(handlePendingAmountDetected(QString,double)));
        connect(this, SIGNAL(confirmedAmountDetected(QString,double)), m_Clients, SLOT(handleConfirmedAmountDetected(QString,double)));
        //todo: a BitcoinPoller class might be better.. it makes more sense for it to emit the signal and contain all the polling logic. it can, but might not have to, be on it's own thread. but it's not toooo inaccurate for the bank to do all this logic. and the code's already here so fuck it

        QMetaObject::invokeMethod(m_Clients, "startListening", Qt::QueuedConnection);
    }
}
void Bank::handleAddUserRequested(const QString &appId, const QString &userName)
{
    //TODO: we should emit an AddUserResult or something, which deals with success, fails, and any other scenarios... and also stores the appId + userName in it (struct?)
    //i seem to be wanting this AddUserResult to be propogating from the db all the way through this server app and even back to the remote connection!! it would be nice if i can figure out way to do such a thing using just 1 type. it just might be possible. i want to avoid the mess of BankDbAddUserResult -> BankAddUserResult -> BankServerAddUserResult --[network]--> parse/process BankServerAddUserResult
    //perhaps the answer is to use an AddUserResult type struct thing (enum maybe?) in my sharedProtocol.h file

    m_Db.addUser(appId, userName);
    emit userAdded(appId, userName);
}
void Bank::handleAddFundsKeyRequested(const QString &appId, const QString &userName)
{
    //todo: should we also check here that we're not awaiting/pending... or is checking on the client/cache enough? we shouldn't even get here unless it was already checked (by the client)... but for future proofing for apps maybe it'd be good to check here (only? too?)

    QString newKey = m_Bitcoin.getNewKeyToReceivePaymentsAt();
    m_Db.setAddFundsKey(appId, userName, newKey);
    //todo: set up the code that polls this new key every so often to see if it has changed. i guess for now it's ok to iterate over m_Db and check/poll the keys for anything that's Awaiting or Pending... but in the future we'll want to have the list be separate (probably? could be wrong) and also NOT check awaitings that are > 24 hours old (or something). the user can still manually request the poll... but we definitely need to STOP polling after a certain period of time... else we may end up with hundreds of thousands of old as fuck keys that we are polling

    //we should set a timer that begins polling the list of addresses every 10 seconds. but the timer should stop while processing the list. and each item in the list should be sent as an event to ourselves so we're still able to receive addFundsKeyRequests at the same time that we're polling for older ones
    //so basically there should be a check for isLastKeyToPoll at the end of pollOneAddFundsKey (which we invokeObject queue'd).. if it evaluates to false, we launch another pollOneAddFundsKey (as an event). and lastly, if isLastKeyToPoll evalutates to true, we re-enable the 10-second timer. we don't want race conditions... which is why we disable the timer right when we start processing the list

    emit addFundsKeyGenerated(appId, userName, newKey);

    m_ListOfNewKeysToAddToAwaitingOnNextTimeout.append(newKey);

    if(!m_PollingTimer->isActive() && !m_CurrentlyProcessingPollingLists)
    {
        //if the timer isn't already started (it won't be started in 2 cases: our FIRST time at this code AND when processing the lists), and it isn't stopped because we're in the middle of processing the list... then start the timer. the boolean is safe to access because we only enter any of this class's methods via events.. which means we'll always be on the same thread and no other thread accesses the boolean
        m_PollingTimer->start();
    }
}
void Bank::handlePollingTimerTimedOut()
{
    emit d("poll timer just timed out");
    if(m_ListOfNewKeysToAddToAwaitingOnNextTimeout.size() > 0 /*we can't add to our list of keys to poll while we're in the middle of polling... that'd be a nightmare*/)
    {
        m_ListOfAwaitingKeysToPoll.append(m_ListOfNewKeysToAddToAwaitingOnNextTimeout);
        m_ListOfNewKeysToAddToAwaitingOnNextTimeout.clear();
    }
    if(m_ListOfNewKeysToAddToPendingOnNextTimeout.size() > 0)
    {
        m_ListOfPendingKeysToPoll.append(m_ListOfNewKeysToAddToPendingOnNextTimeout);
        m_ListOfNewKeysToAddToPendingOnNextTimeout.clear();
    }
    m_CurrentIndexInListOfAwaitingKeysToPoll = 0;
    m_CurrentIndexInListOfPendingKeysToPoll = 0;
    if(m_ListOfAwaitingKeysToPoll.size() > 0)
    {
        m_CurrentlyProcessingPollingLists = true;
        m_PollingTimer->stop();
        QMetaObject::invokeMethod(this, "pollOneAwaitingKey", Qt::QueuedConnection);
    }
    if(m_ListOfPendingKeysToPoll.size() > 0)
    {
        m_CurrentlyProcessingPollingLists = true;
        if(m_PollingTimer->isActive()) { m_PollingTimer->stop(); } //i hope calling this twice has no effect. actually, hoping is for suckers. *adds code*
        QMetaObject::invokeMethod(this, "pollOnePendingKey", Qt::QueuedConnection);
    }
    emit d("at the end of the timeout function, the timer is now: " + QString((m_PollingTimer->isActive() ? "active" : "disabled")));
}
void Bank::pollOneAwaitingKey()
{
    if(m_CurrentIndexInListOfAwaitingKeysToPoll < m_ListOfAwaitingKeysToPoll.size())
    {
        QString keyToPoll = m_ListOfAwaitingKeysToPoll.at(m_CurrentIndexInListOfAwaitingKeysToPoll);
        emit d("about to poll awaiting payment key: " + keyToPoll);
        double pendingAmount = m_Bitcoin.parseAmountAtAddressForConfirmations(0, keyToPoll);

        if(pendingAmount > 0.0)
        {
            emit d("key: " + keyToPoll + " now has a pending amount of: " + QString::number(pendingAmount, 'f', 8)); //todo: static doubleToString

            //i guess this is where i'd emit a signal?? TODO: we need the appId, username, key, and amount... but for now/debugging/coding/simplicity, we'll just emit it the key + amount
            emit pendingAmountDetected(keyToPoll, pendingAmount);

            //not only do we emit the update, but we add it to the next step so it gets poll'd for it's confirmation next time
            m_ListOfNewKeysToAddToPendingOnNextTimeout.append(m_ListOfAwaitingKeysToPoll.takeAt(m_CurrentIndexInListOfAwaitingKeysToPoll)); //TODO: oh fuck huge error. we're modifying our list as we're iterating through it
            //hack: decrement the index to account for what we took/removed. the next time we're here, we'll have the same exact .at() ... but it SHOULD work(???)
            --m_CurrentIndexInListOfAwaitingKeysToPoll;

            //TODO/thoughts: the polling lists should just be in memory (in impl). when we emit our signal or whatever, bank responds to it by updating the db (which is in-memory for now, but won't be in the impl) and also by notifying the client over the network. oh whoops thought this code was in bitcoinhelper.cpp (should it be?)... but well yea, we're still going to emit the signal... but we just modify the m_Db right here
        }

        //onto the next! if we didn't remove one, .at() will actually be +1.. but if we did remove one (see above), the next .at() will be the same. it's why we have the -- and then right here a ++ again. we ALWAYS want to ++ to get to the next, but we don't always need to use the -- hack unless we remove one
        ++m_CurrentIndexInListOfAwaitingKeysToPoll;
    }

    reEnableTimerIfBothListsAreDone();
}
void Bank::pollOnePendingKey()
{
    //all/most of the same comments from the Awaiting function above apply here... including the decrement/takeAt/increment hack

    emit d("polling one pending key");

    if(m_CurrentIndexInListOfPendingKeysToPoll < m_ListOfPendingKeysToPoll.size())
    {
        QString keyToPoll = m_ListOfPendingKeysToPoll.at(m_CurrentIndexInListOfPendingKeysToPoll);
        emit d("about to poll pending payment key: " + keyToPoll);
        double confirmedAmount = m_Bitcoin.parseAmountAtAddressForConfirmations(1, keyToPoll);
        if(confirmedAmount > 0.0)
        {
            emit d("key: " + keyToPoll + " now has a confirmed amount of: " + QString::number(confirmedAmount, 'f', 8));
            emit confirmedAmountDetected(keyToPoll, confirmedAmount);

            m_ListOfPendingKeysToPoll.removeAt(m_CurrentIndexInListOfPendingKeysToPoll); //we don't take because there's no step after this

            --m_CurrentIndexInListOfPendingKeysToPoll;

            //TODO: the m_Db modification should be within these braces... but not necessarily after this decrement. i think we need appId and username anyways to do the modification, so hold off for now. and also: do it for the 'Awaiting' cousin method above. we don't really benefit from updating the db.. but if the app crashes and restarts, we retain our state.
            //should be something like m_Db.promoteAddFundsKeyState(UserAccount::Confirmed)... Pending for the Awaiting method above. and again: this line should not necessarily go here... but definitely within these braces
        }
        ++m_CurrentIndexInListOfPendingKeysToPoll;
    }

    reEnableTimerIfBothListsAreDone();
}
void Bank::reEnableTimerIfBothListsAreDone()
{
    if(m_CurrentIndexInListOfAwaitingKeysToPoll >= m_ListOfAwaitingKeysToPoll.size() && m_CurrentIndexInListOfPendingKeysToPoll >= m_ListOfPendingKeysToPoll.size())
    {
        //if they're both at the end of their list of keys to poll, we re-enable the timer so that in X seconds, we poll the list again. i think this might be an off by one error
        m_CurrentlyProcessingPollingLists = false;
        m_PollingTimer->start();
        emit d("re-enabling the timer because we reached the end of the lists");
    }
}

//TODO: when converting a double to a QString, the format parameter should be 'f', with precision of 8. MAYBE a precision of 9 and we manually round it??? the default format uses e and E... but we don't want that so we have to force 'f'
//when converting from QString to double, it should just work.. but we can check the &ok to make sure it's true afterwards
