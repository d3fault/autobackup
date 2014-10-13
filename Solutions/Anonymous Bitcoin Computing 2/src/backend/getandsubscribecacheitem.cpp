#include "getandsubscribecacheitem.h"

#ifdef ABC_MULTI_CAMPAIGN_OWNER_MODE
#include <event2/event.h>

#include "anonymousbitcoincomputingcouchbasedb.h"

const struct timeval GetAndSubscribeCacheItem::m_OneHundredMilliseconds = {0,100000};
AnonymousBitcoinComputingCouchbaseDB* GetAndSubscribeCacheItem::m_AnonymousBitcoinComputingCouchbaseDB = NULL;
struct event_base* GetAndSubscribeCacheItem::m_EventBase = NULL;

void GetAndSubscribeCacheItem::setAnonymousBitcoinComputingCouchbaseDB(AnonymousBitcoinComputingCouchbaseDB *anonymousBitcoinComputingCouchbaseDB)
{
    m_AnonymousBitcoinComputingCouchbaseDB = anonymousBitcoinComputingCouchbaseDB;
}
void GetAndSubscribeCacheItem::setEventBase(event_base *eventBase)
{
    m_EventBase = eventBase;
}
void GetAndSubscribeCacheItem::startPollingTimeoutTimer()
{
    event_add(m_MyLibEventTimer, &m_OneHundredMilliseconds);
}
GetAndSubscribeCacheItem::GetAndSubscribeCacheItem(const std::string &key)
#else // not #def ABC_MULTI_CAMPAIGN_OWNER_MODE
GetAndSubscribeCacheItem::GetAndSubscribeCacheItem()
#endif // ABC_MULTI_CAMPAIGN_OWNER_MODE
    : CurrentlyFetchingPossiblyNew(true /* true because we always fetch one right after being created */)
    , DocumentCAS(0)
#ifdef ABC_MULTI_CAMPAIGN_OWNER_MODE
    , Key(key)
    , m_MyLibEventTimer(evtimer_new(m_EventBase, &GetAndSubscribeCacheItem::timeoutCallbackStatic, this))
#endif // ABC_MULTI_CAMPAIGN_OWNER_MODE
{ }
#ifdef ABC_MULTI_CAMPAIGN_OWNER_MODE
GetAndSubscribeCacheItem::~GetAndSubscribeCacheItem()
{
    event_free(m_MyLibEventTimer);
}
#endif
#ifdef ABC_MULTI_CAMPAIGN_OWNER_MODE
void GetAndSubscribeCacheItem::timeoutCallbackStatic(int unusedSocket, short events, void *userData)
{
    (void)unusedSocket;
    (void)events;
    m_AnonymousBitcoinComputingCouchbaseDB->getAndSubscribePollingTimeoutEventSlot(static_cast<GetAndSubscribeCacheItem*>(userData));
}
#endif // ABC_MULTI_CAMPAIGN_OWNER_MODE
