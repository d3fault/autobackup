#include "bitcoinpoller.h"

//it feels dirty having BitcoinPoller on the same thread as Bank, but it's necessary because they both are going to be accessing BitcoinHelper and we don't want to do that at the same time (though it probably wouldn't matter, i'm not going to risk it). as for whether the connection to pollThisKeyAndNotifyUsOfUpdates is Queued or Direct (it will be direct if we leave it automatic), decide later after we've written more code. prolly won't matter either way.
BitcoinPoller::BitcoinPoller(int pollingIntervalInSeconds, QObject *parent) :
    QObject(parent), m_KeyIterator(0), m_CurrentlyPolling(false)
{
    m_PollingTimer = new QTimer();
    m_PollingTimer->setInterval(pollingIntervalInSeconds * 1000);
    connect(m_PollingTimer, SIGNAL(timeout()), this, SLOT(handlePollingTimerTimedOut()));
}
void BitcoinPoller::pollThisKeyAndNotifyUsOfUpdates(QString appId, QString userName, QString key)
{
    emit d("adding key: " + key + " to list of keys to be polled");
    //using a QMutableListIterator, i should add the new key. it will not get poll'd until the enxt timeout because ListIterator.insert() inserts and then puts the iterator after the inserted item
    //i should have another QHash that associates my keys (in the list) with an AppId and Username. i don't think list iterator works with hash's.. but could be wrong. oh just found there's mutable hash iterator lawl. fucking bullshit, QMutableHashIterator doesn't have a .insert ... you can modify existing but can't grow it like with QMutableListIterator. oh well, i don't NEED to have only 1 hash containing all my shit (these two comments i kinda changed my mind halfway.. but go back to original idea)

    DetailsAssociatedWithKey *deets = new DetailsAssociatedWithKey(); //pointer so we can modify it in-hash
    deets->AppId = appId;
    deets->UserName = userName;
    deets->IsPending = false; //awaiting

    m_NewKeysHash.insert(key, deets);

    if(!m_CurrentlyPolling && !m_PollingTimer->isActive())
    {
        emit d("starting the timer because we aren't currently polling (probably empty) and the timer isn't already started");
        m_PollingTimer->start();
    }
}
void BitcoinPoller::handlePollingTimerTimedOut()
{
    emit d("polling timer timed out");
    if(m_NewKeysHash.size() > 0)
    {
        emit d(QString::number(m_NewKeysHash.size()) + " new keys to poll");
        emit d("before appending the new keys, our hash size is: " + QString::number(m_CurrentlyPollingKeysHash.size()));
        m_CurrentlyPollingKeysHash.unite(m_NewKeysHash); //append/combine them (order doesn't matter)
        m_NewKeysHash.clear();
        emit d("after appending the new keys, our hash size is: " + QString::number(m_CurrentlyPollingKeysHash.size()));
    }
    if(m_CurrentlyPollingKeysHash.size() > 0)
    {
        emit d("we are about to beginning polling because our hash has: " + QString::number(m_CurrentlyPollingKeysHash.size()) + " keys");
        m_CurrentlyPolling = true; //so we don't enable the timer each time pollThisKeyAndNotifyUsOfUpdates is called. hmm come to think of it maybe this is why my bank.ccp implementation didn't work with multiple keys... idfk.
        m_PollingTimer->stop();
        if(!m_KeyIterator) //first time
        {
            m_KeyIterator = new QMutableHashIterator<QString, DetailsAssociatedWithKey*>(m_CurrentlyPollingKeysHash); //todo: should i set it to null first or delete it first if it isn't null? i could see this as being a memory leak. it most likely is. or maybe i could just initialize it in the constructor and then do .toFront here??? it is mutable so it's ok if shit gets added... but actually no it's not unless the iterator is the one doing the adding... which doesn't exist for hashes. hmmf.
        }
        else
        {
            *m_KeyIterator = m_CurrentlyPollingKeysHash; //the assign operator is overridden to make the iterator operate on the underlying hash. it also calls .toFront() for us. this should work but might not... because it checks to see that the new one isn't the same as the one it's already operating on (i just examined the source and it APPEARS to be the case)
            //m_KeyIterator->toFront(); this might be all i need, but i don't know if it will work even after i've inserted/united the new keys...
            //the safest solution is to delete m_KeyIterator and then new it again
        }
        QMetaObject::invokeMethod(this, "pollOneKey", Qt::QueuedConnection);
    }
    else
    {
        emit d("stopping the timer because the list of hash's is empty. it should re-enable itself when one is added");
        m_CurrentlyPolling = false;
        m_PollingTimer->stop();
    }
}
void BitcoinPoller::pollOneKey()
{
    emit d("entering pollOneKey");
    if(m_KeyIterator->hasNext())
    {
        emit d("hasNext returned true");
        m_KeyIterator->next();

        DetailsAssociatedWithKey *deets = m_KeyIterator->value();

        double amount = BitcoinHelper::Instance()->parseAmountAtAddressForConfirmations((deets->IsPending ? 1 : 0), m_KeyIterator->key()); //the key of the hash contains the bitcoin key. lol maybe i should call it bitcoinKey to avoid confusion
        emit d("polling key: " + m_KeyIterator->key());
        if(amount > 0.0)
        {
            if(deets->IsPending) //we're polling for the confirmation
            {
                emit confirmedAmountDetected(deets->AppId, deets->UserName, m_KeyIterator->key(), amount);
                m_KeyIterator->remove(); //don't poll this one anymore. TODO: maybe delete the pointer in the value? psbl memory leak
            }
            else //we're polling for the initial payment (it will be pending once received)
            {
                emit pendingAmountDetected(deets->AppId, deets->UserName, m_KeyIterator->key(), amount);
                deets->IsPending = true; //since deets is a pointer, we don't need to do iterator.setValue to put it back in. we could though and make deets a non-pointer, but fuck it.
            }
        }

        //schedule the invoke. slightly a waste of efficiency for when there is no hasNext, but dupe code if i do hasNext check here... HMMM, which is worse? the dupe code will grow even more because it will also have to re-enable the timer
        QMetaObject::invokeMethod(this, "pollOneKey", Qt::QueuedConnection);
    }
    else
    {
        emit d("hasNext returned false, starting the timer back up");
        //end of list, re-enable timer
        m_CurrentlyPolling = false;
        m_PollingTimer->start();
    }
}

//man this code is so much cleaner/simpler than my Bank.cpp implementation. rofl. only slight inefficiency i can see is the QMutableHashIterator being re-constructed over and over...
