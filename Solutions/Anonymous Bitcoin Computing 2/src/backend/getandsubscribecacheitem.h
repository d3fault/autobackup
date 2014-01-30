#ifndef GETANDSUBSCRIBECACHEITEM_H
#define GETANDSUBSCRIBECACHEITEM_H

#include <string>
#include <sys/types.h>
#include <boost/unordered_map.hpp>

typedef boost::unordered_map<void * /* WApplication pointer */, void * /* GetCouchbaseDocumentByKeyRequest pointer */> SubscribersType;
//TODOreq: when sessionId changes (login), we need to change it here too

struct GetAndSubscribeCacheItem
{
    GetAndSubscribeCacheItem() : CurrentlyFetchingPossiblyNew(true /* true because we always fetch one right after being created */), DocumentCAS(0) { }
    bool CurrentlyFetchingPossiblyNew; //it's currently fetching, but that doesn't mean it will be-new/have-changed
    std::string Document;
    u_int64_t DocumentCAS;
    SubscribersType NewSubscribers; //want whatever next fetch gives
    SubscribersType Subscribers; //only want changes

    //TODOreq: unsubscribe at appropriate times too!
};

#endif // GETANDSUBSCRIBECACHEITEM_H
