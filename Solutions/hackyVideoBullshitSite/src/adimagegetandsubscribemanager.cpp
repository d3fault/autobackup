#include "adimagegetandsubscribemanager.h"

#include "boostthreadinitializationsynchronizationkit.h"

AdImageGetAndSubscribeManager::AdImageGetAndSubscribeManager()
{
}
AdImageGetAndSubscribeManager::startManagingAdImageSubscriptionsAndWaitUntilToldToStop(BoostThreadInitializationSynchronizationKit *threadInitializationSynchronizationKit)
{
    //was going to use libevent, but then realized the only reason i DIDN'T use qt in Abc2 was because i couldn't figure out how to make couchbase play nicely with qt's event loop (it probably could, but fuck if i know how). no couchbase = qt it is :).. still gonna use boost's json libs though lawl. shit now i had no reason to upgrade my boost libs for lockfree::queue but that's ok. and hey i'm more comfortable with qt's http client than boost/asio (the asio http client tutorial made me cringe at how low level it is)
    //OLD: libevent initialization copy/paste job from Abc2 db backend. Despite this app having nothing to do with couchbase, I've grown fond of libevent :-P. Thank you for forcing me to use it, couchbase. If I could wrap my head around boost's signals2 library, I'd use that instead (documentation sucks, gives no threading examples)


    threadInitializationSynchronizationKit->notifyThatInitializationIsComplete();
}
