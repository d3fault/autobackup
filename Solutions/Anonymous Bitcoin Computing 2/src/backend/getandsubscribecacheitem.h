#ifndef GETANDSUBSCRIBECACHEITEM_H
#define GETANDSUBSCRIBECACHEITEM_H

#include <string>
#include <sys/types.h>
#include <boost/unordered_map.hpp>

class AnonymousBitcoinComputingCouchbaseDB;

typedef boost::unordered_map<void * /* WApplication pointer */, void * /* GetCouchbaseDocumentByKeyRequest pointer */> SubscribersType;

class GetAndSubscribeCacheItem
{
public:
#ifdef ABC_MULTI_CAMPAIGN_OWNER_MODE
    static void setAnonymousBitcoinComputingCouchbaseDB(AnonymousBitcoinComputingCouchbaseDB *anonymousBitcoinComputingCouchbaseDB);
    static void setEventBase(struct event_base *eventBase);

    void startPollingTimeoutTimer();
#endif // ABC_MULTI_CAMPAIGN_OWNER_MODE

#ifdef ABC_MULTI_CAMPAIGN_OWNER_MODE
    GetAndSubscribeCacheItem(const std::string &key);
    ~GetAndSubscribeCacheItem();
#else // not #def ABC_MULTI_CAMPAIGN_OWNER_MODE
    GetAndSubscribeCacheItem();
#endif
    bool CurrentlyFetchingPossiblyNew; //it's currently fetching, but that doesn't mean it will be-new/have-changed
    std::string Document;
    u_int64_t DocumentCAS;

#ifdef ABC_MULTI_CAMPAIGN_OWNER_MODE
    std::string Key;
#endif //ABC_MULTI_CAMPAIGN_OWNER_MODE

    SubscribersType NewSubscribers; //want whatever next fetch gives
    SubscribersType Subscribers; //only want changes

    std::vector<void*> UnsubscribeRequestsToProcessMomentarily;
    std::vector<void*> ChangeSessionIdRequestsToProcessMomentarily;
private:
#ifdef ABC_MULTI_CAMPAIGN_OWNER_MODE
    static const struct timeval m_OneHundredMilliseconds;
    static AnonymousBitcoinComputingCouchbaseDB *m_AnonymousBitcoinComputingCouchbaseDB;
    static struct event_base *m_EventBase;

    struct event *m_MyLibEventTimer;

    static void timeoutCallbackStatic(int unusedSocket, short events, void *userData);
#endif // ABC_MULTI_CAMPAIGN_OWNER_MODE
};

#endif // GETANDSUBSCRIBECACHEITEM_H
