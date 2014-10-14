#include "anonymousbitcoincomputingcouchbasedb.h"

#include "getandsubscribecacheitem.h"
#include "../frontend2backendRequests/storecouchbasedocumentbykeyrequest.h"
#include "../frontend2backendRequests/getcouchbasedocumentbykeyrequest.h"
#include "d3faultscouchbaseshared.h"

using namespace std;

const struct timeval AnonymousBitcoinComputingCouchbaseDB::m_OneHundredMilliseconds = {0,100000};

//TODOoptimization: not only will lockfree::queue be faster just as a queue implementation, but it provides two additional benefits: 1) dynamic sizing at run-time, so I don't need Store, StoreLarge, and Get (just Get and Store). 2) I think I can just pass the pointer to the request object instead of serializing/copying/reading/deserializing it, but unsure about this. Another benefit that is mostly design related, is that I can use proper boost::bind callbacks etc since I won't be serializing anything. I can clean up the "what x was for" enum code (as in, delete it), and boost::bind also provides a solution to expanding GetAndSubscribe (currently, changing subscriptions may still receive a subscription or two for the old subcription, but since we use "what x was for", they OLD subscription would go to the NEW callback and blah segfault (but boost::bind fixes this :-P)). Also the WApplication::bind thing I've never used.
//TODOoptimization: I think maybe the 100ms polling should increase as there are more items in the get and subscribe cache. Like if there's only 1 item we are at 100ms, but maybe a couple thousand should be like 5 seconds (and everything linearly in between ofc). 100ms polling just sounds like too much if there are tons of items, but eh I'm not sure that's correct since there's sharding/distribution of load going on for those thousands already. For now I'm just going to do 100ms for all. Thought about doing a time per key (cache item) and then having only the ones with lots of subscribers at 100ms (few subscribers = 1-5 seconds), but I like the first strategy mentioned better
//TODOoptimization: it might be wise to use nagle's algorithm, at, say, 5ms. the couchbase "get/store" commands can be batched together with ease (but figuring out which responses go with what might not be as simple)

#define ABC_SCHEDULE_COUCHBASE_REQUEST_USING_NEW_OR_RECYCLED_AUTO_RETRYING_WITH_EXPONENTIAL_BACKOFF(GetOrStore) \
AutoRetryingWithExponentialBackoffCouchbase##GetOrStore##Request *newOrRecycledExponentialBackoffTimerAndCallback; \
if(m_AutoRetryingWithExponentialBackoffCouchbase##GetOrStore##RequestCache.empty()) \
{ \
    newOrRecycledExponentialBackoffTimerAndCallback = new AutoRetryingWithExponentialBackoffCouchbase##GetOrStore##Request(); \
} \
else \
{ \
    newOrRecycledExponentialBackoffTimerAndCallback = m_AutoRetryingWithExponentialBackoffCouchbase##GetOrStore##RequestCache.back(); \
    m_AutoRetryingWithExponentialBackoffCouchbase##GetOrStore##RequestCache.pop_back(); \
} \
newOrRecycledExponentialBackoffTimerAndCallback->setNew##GetOrStore##RequestToRetry(originalRequest); \
schedule##GetOrStore##Request(newOrRecycledExponentialBackoffTimerAndCallback);

AnonymousBitcoinComputingCouchbaseDB::AnonymousBitcoinComputingCouchbaseDB()
    : m_Thread(boost::bind(&AnonymousBitcoinComputingCouchbaseDB::threadEntryPoint, this)), m_IsDoneInitializing(false), m_IsConnected(false), m_ThreadExittedCleanly(false), m_IsInEventLoop(false), m_NoMoreAllowedMuahahaha(false), m_PendingStoreCount(0), m_PendingGetCount(0), m_IsFinishedWithAllPendingRequests(false)
    , m_BeginStoppingCouchbaseCleanlyEvent(NULL)
    , m_FinalCleanUpAndJoinEvent(NULL)
    //, m_StoreWtMessageQueue0(NULL)
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO, ABC_COUCHBASE_MESSAGE_QUEUE_NULL_INITIALIZATION_MACRO)
    //, m_StoreMessageQueuesCurrentMessageBuffer(NULL);
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_MACRO, ABC_COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_DEFINITION_MACRO)
    //, m_StoreEventCallbackForWt0(NULL)
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO, ABC_COUCHBASE_LIBEVENTS_MEMBER_DEFINITIONS_MACRO)
{
    //constructor body, in case you're confused...
}
boost::condition_variable *AnonymousBitcoinComputingCouchbaseDB::isConnectedWaitCondition()
{
    return &m_IsConnectedWaitCondition;
}
boost::condition_variable *AnonymousBitcoinComputingCouchbaseDB::isFinishedWithAllPendingRequestsWaitCondition()
{
    return &m_IsFinishedWithAllPendingRequestsWaitCondition;
}
boost::mutex *AnonymousBitcoinComputingCouchbaseDB::isConnectedMutex()
{
    return &m_IsConnectedMutex;
}
boost::mutex *AnonymousBitcoinComputingCouchbaseDB::isFinishedWithAllPendingRequestsMutex()
{
    return &m_IsFinishedWithAllPendingRequestsMutex;
}
event *AnonymousBitcoinComputingCouchbaseDB::beginStoppingCouchbaseCleanlyEvent()
{
    return m_BeginStoppingCouchbaseCleanlyEvent;
}
event *AnonymousBitcoinComputingCouchbaseDB::finalCleanUpAndJoinEvent()
{
    return m_FinalCleanUpAndJoinEvent;
}
bool AnonymousBitcoinComputingCouchbaseDB::isDoneInitializing()
{
    return m_IsDoneInitializing;
}
bool AnonymousBitcoinComputingCouchbaseDB::isConnected()
{
    return m_IsConnected;
}
bool AnonymousBitcoinComputingCouchbaseDB::isInEventLoop()
{
    return m_IsInEventLoop;
}
bool AnonymousBitcoinComputingCouchbaseDB::isFinishedWithAllPendingRequests()
{
    return m_IsFinishedWithAllPendingRequests;
}
void AnonymousBitcoinComputingCouchbaseDB::waitForJoin()
{
    m_Thread.join();
}
bool AnonymousBitcoinComputingCouchbaseDB::threadExittedCleanly()
{
    return m_ThreadExittedCleanly;
}
void AnonymousBitcoinComputingCouchbaseDB::threadEntryPoint()
{
    //libevent
    struct LibEventBaseScopedDeleter
    {
        bool LibEventBaseInitializeSuccess;
        struct event_base *LibEventBase;

        LibEventBaseScopedDeleter()
            : LibEventBaseInitializeSuccess(true), LibEventBase(event_base_new())
        {
            if(!LibEventBase)
            {
                LibEventBaseInitializeSuccess = false;
            }
        }
        ~LibEventBaseScopedDeleter()
        {
            if(LibEventBaseInitializeSuccess)
            {
                event_base_free(LibEventBase);
            }
        }
    } LibEventBaseScopedDeleterInstance;

    if(!LibEventBaseScopedDeleterInstance.LibEventBaseInitializeSuccess)
    {
        cerr << "Error: Libevent failed to initialize" << endl;
        ABC_TELL_MAIN_THREAD_THAT_COUCHBASE_FAILED_MACRO
        return;
    }

    if(evthread_use_pthreads() < 0) //or evthread_use_windows_threads
    {
        cerr << "Error: Failed to evthread_use_pthreads" << endl;
        ABC_TELL_MAIN_THREAD_THAT_COUCHBASE_FAILED_MACRO
        return;
    }
    if(evthread_make_base_notifiable(LibEventBaseScopedDeleterInstance.LibEventBase) < 0)
    {
        cerr << "Error: Failed to make event base notifiable" << endl;
        ABC_TELL_MAIN_THREAD_THAT_COUCHBASE_FAILED_MACRO
        return;
    }
    struct lcb_create_io_ops_st couchbaseCreateIoOps;
    struct CouchbaseIoOpsScopedDeleter
    {
        bool DontDestroyBecauseCreateFailed;
        lcb_io_opt_t CouchbaseIoOps;
        CouchbaseIoOpsScopedDeleter()
            : DontDestroyBecauseCreateFailed(false)
        { }
        ~CouchbaseIoOpsScopedDeleter()
        {
            if(!DontDestroyBecauseCreateFailed)
            {
                lcb_destroy_io_ops(CouchbaseIoOps);
            }
        }
    } CouchbaseIoOpsScopedDeleterInstance;
    lcb_error_t error;
    memset(&couchbaseCreateIoOps, 0, sizeof(couchbaseCreateIoOps));
    couchbaseCreateIoOps.v.v0.type = LCB_IO_OPS_LIBEVENT;
    couchbaseCreateIoOps.v.v0.cookie = LibEventBaseScopedDeleterInstance.LibEventBase;
    error = lcb_create_io_ops(&CouchbaseIoOpsScopedDeleterInstance.CouchbaseIoOps, &couchbaseCreateIoOps);
    if(error != LCB_SUCCESS)
    {
        CouchbaseIoOpsScopedDeleterInstance.DontDestroyBecauseCreateFailed = true;
        cerr << "Error: Failed to create an IOOPS structure for libevent: " << lcb_strerror(NULL, error) << endl;
        ABC_TELL_MAIN_THREAD_THAT_COUCHBASE_FAILED_MACRO
        return;
    }

    //couchbase
    struct lcb_create_st copts;
    memset(&copts, 0, sizeof(copts));
    copts.v.v0.host = "192.168.56.10:8091"; //TODOreq: supply lots of hosts, either separated by semicolon or comma, I forget..
    copts.v.v0.io = CouchbaseIoOpsScopedDeleterInstance.CouchbaseIoOps;
    if((error = lcb_create(&m_Couchbase, &copts)) != LCB_SUCCESS)
    {
        cerr << "Failed to create a libcouchbase instance: " << lcb_strerror(NULL, error) << endl;
        ABC_TELL_MAIN_THREAD_THAT_COUCHBASE_FAILED_MACRO
        return;
    }
#if 0
    struct CouchbaseInstanceScopedDeleterOnly
    {
        ~CouchbaseInstanceScopedDeleterOnly()
        {
            lcb_destroy(m_Couchbase);
        }
    }CouchbaseInstanceScopedDeleterOnlyInstance;
#endif

    //lol configuration callback was using the cookie and yet i wasn't setting it until after lcb_connect (which is async (is why it worked)). plugging dat race condition [that never once happened]
    lcb_set_cookie(m_Couchbase, this);

    //couchbase callbacks
    lcb_set_error_callback(m_Couchbase, AnonymousBitcoinComputingCouchbaseDB::errorCallbackStatic);
    lcb_set_configuration_callback(m_Couchbase, AnonymousBitcoinComputingCouchbaseDB::configurationCallbackStatic);
    lcb_set_get_callback(m_Couchbase, AnonymousBitcoinComputingCouchbaseDB::getCallbackStatic);
    lcb_set_store_callback(m_Couchbase, AnonymousBitcoinComputingCouchbaseDB::storeCallbackStatic);
#ifdef ABC_DO_COUCHBASE_DURABILITY_POLL_BEFORE_CONSIDERING_STORE_COMPLETE
    lcb_set_durability_callback(m_Couchbase, AnonymousBitcoinComputingCouchbaseDB::durabilityCallbackStatic);
#endif
    if((error = lcb_connect(m_Couchbase)) != LCB_SUCCESS)
    {
        cerr << "Failed to start connecting libcouchbase instance: " << lcb_strerror(m_Couchbase, error) << endl;
        ABC_TELL_MAIN_THREAD_THAT_COUCHBASE_FAILED_MACRO
        lcb_destroy(m_Couchbase);
        return;
    }

    //libevent events

    //m_StoreEventCallbackForWt0 = event_new(LibEventBaseScopedDeleterInstance.LibEventBase, -1, EV_READ, eventSlotForWtStore0Static, this);
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO, ABC_SETUP_WT_TO_COUCHBASE_CALLBACKS_VIA_EVENT_NEW_MACRO)

#ifdef ABC_MULTI_CAMPAIGN_OWNER_MODE
    GetAndSubscribeCacheItem::setAnonymousBitcoinComputingCouchbaseDB(this);
    GetAndSubscribeCacheItem::setEventBase(LibEventBaseScopedDeleterInstance.LibEventBase);
#else // not #def ABC_MULTI_CAMPAIGN_OWNER_MODE
    m_GetAndSubscribePollingTimeout = evtimer_new(LibEventBaseScopedDeleterInstance.LibEventBase, &AnonymousBitcoinComputingCouchbaseDB::getAndSubscribePollingTimeoutEventSlotStatic, this);
#endif // ABC_MULTI_CAMPAIGN_OWNER_MODE
    AutoRetryingWithExponentialBackoffCouchbaseGetRequest::setAnonymousBitcoinComputingCouchbaseDB(this);
    AutoRetryingWithExponentialBackoffCouchbaseGetRequest::setEventBase(LibEventBaseScopedDeleterInstance.LibEventBase); //need this as a member to add exponential backoff timers at runtime
    m_BeginStoppingCouchbaseCleanlyEvent = event_new(LibEventBaseScopedDeleterInstance.LibEventBase, -1, EV_READ, &AnonymousBitcoinComputingCouchbaseDB::beginStoppingCouchbaseCleanlyEventSlotStatic, this);
    m_FinalCleanUpAndJoinEvent = event_new(LibEventBaseScopedDeleterInstance.LibEventBase, -1, EV_READ, &AnonymousBitcoinComputingCouchbaseDB::finalCleanupAndJoin, LibEventBaseScopedDeleterInstance.LibEventBase);

    m_IsInEventLoop = true; //race conditions? cached results? fuck the police

    event_base_loop(LibEventBaseScopedDeleterInstance.LibEventBase, 0); //wait until event_base_loopbreak is called, processing all events of course

    //cleanup
    //delete our 'held onto' gets which are effectively used as subscriptions
    BOOST_FOREACH(GetAndSubscribeCacheHashType::value_type &v, m_GetAndSubscribeCacheHash)
    {
        //First delete all subscribers of cache item
        //Wt itself deletes s.first (WApplication pointer)
        BOOST_FOREACH(SubscribersType::value_type &s, v.second->NewSubscribers)
        {
            GetCouchbaseDocumentByKeyRequest *subscription = static_cast<GetCouchbaseDocumentByKeyRequest*>(s.second);
            delete subscription;
        }
        BOOST_FOREACH(SubscribersType::value_type &s, v.second->Subscribers)
        {
            GetCouchbaseDocumentByKeyRequest *subscription = static_cast<GetCouchbaseDocumentByKeyRequest*>(s.second);
            delete subscription;
        }

        //Then delete cache item itself
        GetAndSubscribeCacheItem *cacheItem = v.second;
        delete cacheItem;
    }
    //TODOoptional: we could try to receive any left over messages in queue here (in order to tell them that they were too late). Currentlywe just disregard them and quit...
    if(m_IsConnected) //if m_IsConnected isn't true, then we never got our couchbase configuration so we never allocated certain buffers and so we don't need to release them either
    {
        //free(m_StoreMessageQueuesCurrentMessageBuffer); m_StoreMessageQueuesCurrentMessageBuffer = NULL;
        BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_MACRO, ABC_FREE_COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_MACRO)

        //delete m_StoreWtMessageQueue0; m_StoreWtMessageQueue0 = NULL;
        BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO, ABC_CLOSE_AND_DELETE_ALL_NEWD_COUCHBASE_MESSAGE_QUEUES_MACRO)
    }

    //message_queue::remove(ABC_WT_COUCHBASE_MESSAGE_QUEUES_BASE_NAME + "Store0");
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO, ABC_REMOVE_ALL_MESSAGE_QUEUES_MACRO)

    //wtf? valgrind complains if i don't have these event_free's here... but why come (why cum? because it is an option and with infinite time all possible options are eventually.. err... done (performed? executed? chosen? entered?)) it don't complain about all the cross thread ones not being freed (and i thought "all" (including below two) were being free'd at lcb_destroy_io_ops wtf wtf?). Maybe valgrind just isn't catching them because it's all happening at scope exit in struct destructor (lol wut) (in which case it's a valgrind bug i'd say)
#ifndef ABC_MULTI_CAMPAIGN_OWNER_MODE
    event_free(m_GetAndSubscribePollingTimeout);
#endif // ABC_MULTI_CAMPAIGN_OWNER_MODE
    event_free(m_FinalCleanUpAndJoinEvent);
    event_free(m_BeginStoppingCouchbaseCleanlyEvent);
    lcb_destroy(m_Couchbase);
#if 0 //TODOoptional: done by scoped structs, however we lose the eror message from lcb_destroy_io_ops. hackily capture in struct destructor and just put the struct inside an anon scope "{ }" if you want to capture that error. fuck it for now
    if(lcb_destroy_io_ops(couchbaseIoOps) != LCB_SUCCESS)
    {
        fprintf(stderr, "Error: Failed to lcb_destroy_io_ops\n");
        return;
    }
#endif
    //TODOoptional: lcb_destroy_io_ops "frees" all the events, but doesn't set them back to NULL! actually it might what the fuck do i know...

    m_ThreadExittedCleanly = true;
}
void AnonymousBitcoinComputingCouchbaseDB::errorCallbackStatic(lcb_t instance, lcb_error_t error, const char *errinfo)
{
    const_cast<AnonymousBitcoinComputingCouchbaseDB*>(static_cast<const AnonymousBitcoinComputingCouchbaseDB*>(lcb_get_cookie(instance)))->errorCallback(error, errinfo);
}
void AnonymousBitcoinComputingCouchbaseDB::errorCallback(lcb_error_t error, const char *errinfo)
{
    cerr << "COUCHBASE ERROR CALLBACK: " << lcb_strerror(m_Couchbase, error) << " - " << errinfo << endl;

    if(!m_IsConnected)
    {
        ABC_TELL_MAIN_THREAD_THAT_COUCHBASE_FAILED_MACRO
    }
}
void AnonymousBitcoinComputingCouchbaseDB::configurationCallbackStatic(lcb_t instance, lcb_configuration_t config)
{
    const_cast<AnonymousBitcoinComputingCouchbaseDB*>(static_cast<const AnonymousBitcoinComputingCouchbaseDB*>(lcb_get_cookie(instance)))->configurationCallback(config);
}
void AnonymousBitcoinComputingCouchbaseDB::configurationCallback(lcb_configuration_t config)
{
    if(config == LCB_CONFIGURATION_NEW)
    {
        //tell main thread that we're now ready to receive events
        {
            boost::lock_guard<boost::mutex> couchbaseConnectingLock(m_IsConnectedMutex);
            m_IsDoneInitializing = true;
            m_IsConnected = true;

            //clean-up previous crashed exits via message_queue::remove(ABC_WT_COUCHBASE_MESSAGE_QUEUES_BASE_NAME + "Store0");
            BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO, ABC_REMOVE_ALL_MESSAGE_QUEUES_MACRO)


            //m_StoreWtMessageQueue0 = new message_queue(create_only, "BlahBaseNameStore0", 200x, 100kb);
            BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO, ABC_NEW_AND_CREATE_MY_MESSAGE_QUEUES_MACRO)

            //m_StoreMessageQueuesCurrentMessageBuffer = malloc(ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_Store);
            BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_MACRO, ABC_MALLOC_COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_MACRO)
        }
        m_IsConnectedWaitCondition.notify_one();
    }
}
void AnonymousBitcoinComputingCouchbaseDB::storeCallbackStatic(lcb_t instance, const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp)
{
    AnonymousBitcoinComputingCouchbaseDB *anonymousBitcoinComputingCouchbaseDB = const_cast<AnonymousBitcoinComputingCouchbaseDB*>(static_cast<const AnonymousBitcoinComputingCouchbaseDB*>(lcb_get_cookie(instance)));
    anonymousBitcoinComputingCouchbaseDB->storeCallback(cookie, operation, error, resp);
    anonymousBitcoinComputingCouchbaseDB->decrementPendingStoreCountAndHandle();
}
void AnonymousBitcoinComputingCouchbaseDB::storeCallback(const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp)
{
    (void)operation; //TODOoptional: i was thinking of using a flag in my "from wt reqeust object (cookie here)" and to NOT do durability when it's a simple "STORE" (because STOREs _TEND_ to be not as important as "ADD" (though that certainly isn't ALWAYS true~)). the purpose of this comment is to point out that we can see if it's a STORE/ADD right here in the 'operation' variable :-P

    AutoRetryingWithExponentialBackoffCouchbaseStoreRequest *autoRetryingWithExponentialBackoffCouchbaseStoreRequest = const_cast<AutoRetryingWithExponentialBackoffCouchbaseStoreRequest*>(static_cast<const AutoRetryingWithExponentialBackoffCouchbaseStoreRequest*>(cookie));
    StoreCouchbaseDocumentByKeyRequest *originalRequest = autoRetryingWithExponentialBackoffCouchbaseStoreRequest->storeRequestRetrying();

    if(error != LCB_SUCCESS)
    {
        if(error != LCB_KEY_EEXISTS)
        {
            if(D3faultsCouchbaseShared::lcbErrorTypeIsEligibleForExponentialBackoff(error))
            {
                autoRetryingWithExponentialBackoffCouchbaseStoreRequest->backoffAndRetryAgain();
                return;
            }

            //TODOreq: MAYBE lcb_store_replica (if it even exists xD, this comment is copy/paste from GET[_replica]) when error qualifies (play around with failover stuff + timeouts. i don't know how couchbase does failovers, I might just need a normal retry (assuming failover time is less than message timeout (so by the time the second one is dispatched, failover has occured)) instead of the store_replica thing)

            cerr << "Failed to store key: " << lcb_strerror(m_Couchbase, error) << endl;
            if(originalRequest->SaveCasOutput)
            {
                StoreCouchbaseDocumentByKeyRequest::respondWithCas(originalRequest, resp->v.v0.cas, false, true);
            }
            else
            {
                StoreCouchbaseDocumentByKeyRequest::respond(originalRequest, false, true);
            }
            m_AutoRetryingWithExponentialBackoffCouchbaseStoreRequestCache.push_back(autoRetryingWithExponentialBackoffCouchbaseStoreRequest);
            delete originalRequest;
            return;
        }

        //if LCB_ADD'ing, the key already exists. If LCB_SET, then your cas (which is implied otherwise we can't get this error) didn't match when setting (someone modified since you did the get)

        //lcb op fail handling. this is a VERY common situation (for both add and set)
        //example LCB_ADD fails: 1) buy ad slot [with filler] -- the core operation of abc. 2) username already exists
        //example LCB_SET fails: lock-account-before-buying-ad-slot[with-filler]-just-after-verifying-users-balance-is-high-enough

        //LCB_KEY_EEXISTS
        if(originalRequest->SaveCasOutput)
        {
            StoreCouchbaseDocumentByKeyRequest::respondWithCas(originalRequest, resp->v.v0.cas, false, false);
        }
        else
        {
            StoreCouchbaseDocumentByKeyRequest::respond(originalRequest, false, false);
        }
        m_AutoRetryingWithExponentialBackoffCouchbaseStoreRequestCache.push_back(autoRetryingWithExponentialBackoffCouchbaseStoreRequest);
        delete originalRequest;
        return;
    }

    //getting here means no more exponential retrying for this store request is necessary, so recycle it into the cache
    m_AutoRetryingWithExponentialBackoffCouchbaseStoreRequestCache.push_back(autoRetryingWithExponentialBackoffCouchbaseStoreRequest);

#ifdef ABC_DO_COUCHBASE_DURABILITY_POLL_BEFORE_CONSIDERING_STORE_COMPLETE
    //durability polling has it's own built in exponential backoff <3
    lcb_durability_opts_t lcbDurabilityOptions;
    memset(&lcbDurabilityOptions, 0, sizeof(lcb_durability_opts_t));
#ifdef ABC_COUCHBASE_DURABILITY_WAIT_FOR_PERSISTED_COUNT
    lcbDurabilityOptions.v.v0.persist_to = ABC_COUCHBASE_DURABILITY_WAIT_FOR_PERSISTED_COUNT;
#endif
    lcbDurabilityOptions.v.v0.replicate_to = ABC_COUCHBASE_DURABILITY_WAIT_FOR_REPLICACTION_COUNT;
    lcbDurabilityOptions.v.v0.timeout = 5000000; //5 second timeout is PLENTY, and/but we should probably change this in the place it gets its default if we leave it to zero [just as a TODOoptimization to lessen memory writing]
    lcb_durability_cmd_t lcbDurabilityCommand;
    memset(&lcbDurabilityCommand, 0, sizeof(lcb_durability_cmd_t));
    lcbDurabilityCommand.v.v0.key = resp->v.v0.key;
    lcbDurabilityCommand.v.v0.nkey = resp->v.v0.nkey;

    //in my testing with LCB_ADD, setting lcb->cas to resp->cas didn't work (had: "as expected", but wtf DID i expect?? i guess just a tiny bit of extra verification)... BUT i don't think it matters at all for "LCB_ADD" operations. TODOreq: verify that the cas shit IS working for LCB_SET.. by for example manually locking an account in the couchbase admin area, then running the app and... err wait no... that's the regular cas not durability cas. i guess just run the normal operation twice? does lcb durability not play nice with cas? that's the feeling i'm getting (it sees the key already there and then thinks "replication" is done, even though the cas is old? i haven't a clue and that would suck if true... :-/... I guess the TODOreq is saying that I need to devise some kind of test for this...
    //^do 2 cas-swaps back to back before the first one gets replicated both receive a successful durability poll???


    if(originalRequest->HasCasInput)
    {
        //LCB_SET implied
        lcbDurabilityCommand.v.v0.cas = resp->v.v0.cas;
    }
    if(originalRequest->SaveCasOutput)
    {
        originalRequest->CasInput = resp->v.v0.cas; //HACK (1st): storing this here might give me pain later, but eh basically i need to store it somewhere because i don't have access to it in the durability callback.... and eh i'm PRETTY sure i'm done with the casInput at this point... so i think it's safe to use this field for just a little
    }

    lcb_durability_cmd_t *lcbDurabilityCommandList[1];
    lcbDurabilityCommandList[0] = &lcbDurabilityCommand;
    error = lcb_durability_poll(m_Couchbase, cookie, &lcbDurabilityOptions, 1, lcbDurabilityCommandList);
    if(error != LCB_SUCCESS)
    {
        cerr << "Failed to schedule lcb_durability_poll: " << lcb_strerror(m_Couchbase, error) << endl;
        if(originalRequest->SaveCasOutput)
        {
            StoreCouchbaseDocumentByKeyRequest::respondWithCas(originalRequest, 0, false, true);
        }
        else
        {
            StoreCouchbaseDocumentByKeyRequest::respond(originalRequest, false, true);
        }
        delete originalRequest;
    }
#else //no durability polling, just respond
    if(originalRequest->SaveCasOutput)
    {
        StoreCouchbaseDocumentByKeyRequest::respondWithCas(originalRequest, resp->v.v0.cas, true, false);
    }
    else
    {
        StoreCouchbaseDocumentByKeyRequest::respond(originalRequest, true, false);
    }
    delete originalRequest;
#endif // ABC_DO_COUCHBASE_DURABILITY_POLL_BEFORE_CONSIDERING_STORE_COMPLETE

    //TODOoptional: maybe a m_PendingDurabilityCount; used to not decrement pendingStoreCount until the durability finished, but i think i accidentally took that out. but really am considering removing durability polling altogether right now so idfk. OLD: if durability isn't needed for this particular store, --m_PendingStoreCount just like getCallback/etc
}
//no it isn't ;-P
void AnonymousBitcoinComputingCouchbaseDB::decrementPendingStoreCountAndHandle()
{
    ABC_GOT_A_PENDING_RESPONSE_FROM_COUCHBASE(Store)
}
void AnonymousBitcoinComputingCouchbaseDB::getCallbackStatic(lcb_t instance, const void *cookie, lcb_error_t error, const lcb_get_resp_t *resp)
{
    //TODOoptional: boost::bind might do this recontextualizing for me (hell it might even just be a macro doing the same thing!), but idk and usually only use boost::bind when i'm told to. I find it hilarious that Qt allows you to pass a pointer across a thread, and boost allows you to point to a specific class instance's method... but both say that the other's is "wrong"... and yet they are both C++ frameworks/libraries that provide a lot of the same functionality. I'll tell you one thing: although I'm glad I can still swim in boost land, I much prefer Qt. Much neater and stuff (hard to explain unless you are coder too in which case you already know so what the fuck who am I typing this to?)
    AnonymousBitcoinComputingCouchbaseDB *anonymousBitcoinComputingCouchbaseDB = const_cast<AnonymousBitcoinComputingCouchbaseDB*>(static_cast<const AnonymousBitcoinComputingCouchbaseDB*>(lcb_get_cookie(instance)));
    anonymousBitcoinComputingCouchbaseDB->getCallback(cookie, error, resp);
    anonymousBitcoinComputingCouchbaseDB->decrementPendingGetCountAndHandle(); //because too many damn exit points in getCallback, BUT we do only want to call it _AFTER_ all of them (and in fact i think i was forgetting to call it in some places. putting it here ensures it always gets called :-P)
}
void AnonymousBitcoinComputingCouchbaseDB::getCallback(const void *cookie, lcb_error_t error, const lcb_get_resp_t *resp)
{
    AutoRetryingWithExponentialBackoffCouchbaseGetRequest *autoRetryingWithExponentialBackoffCouchbaseGetRequest = const_cast<AutoRetryingWithExponentialBackoffCouchbaseGetRequest*>(static_cast<const AutoRetryingWithExponentialBackoffCouchbaseGetRequest*>(cookie));
    GetCouchbaseDocumentByKeyRequest *originalRequest = autoRetryingWithExponentialBackoffCouchbaseGetRequest->getRequestRetrying();

    //TODOoptimization: __unlikely, though i think the innards of an if statement are that by default (since "jump". but maybe __unlikely optimizes the statement evaluation as well?)
    //TODOreq: error cases for the hacky get and subscribe polling need to be handled... err... differently (the exponential backoff thing is probably fine, BUT for generic db error ETC (in my single GetAndSubscribe-able key case, LCB_KEY_ENOENT will never be seen, but as I make it future proof then maybe it might (depending how I do shit)) we probably want to notify all subscribers or SOMETHING and not just respond/delete/return like a normal request, idk what specifically but eh something)
    if(error != LCB_SUCCESS)
    {
        if(error != LCB_KEY_ENOENT)
        {
            if(D3faultsCouchbaseShared::lcbErrorTypeIsEligibleForExponentialBackoff(error))
            {
                /*
                TO DOnereq: exponential backoff. I'm thinking a getNewOrRecycled queue of libevent timeouts (i'm dumb for doing macro hell to solve the 'n' methods/functions problem. class/object instantiation solves it betterer :-P)
                */
                autoRetryingWithExponentialBackoffCouchbaseGetRequest->backoffAndRetryAgain();
                return;
            }

            //TODOreq: MAYBE lcb_get_replica when error qualifies (play around with failover stuff + timeouts. i don't know how couchbase does failovers, I might just need a normal retry (assuming failover time is less than message timeout (so by the time the second one is dispatched, failover has occured)) instead of the get_replica thing)

            cerr << "Error couchbase getCallback:" << lcb_strerror(m_Couchbase, error) << endl;

            //hack: deleting the request hackily used for get and subscribe would lead to a segfault later, and/or at the very least make our get and subscribe polling mechanism timer stop...
            if(originalRequest->GetAndSubscribe != 0 && originalRequest->GetAndSubscribe != 6)
            {
                //i honestly don't konw what to do with a get and subscribe error thing, so don't "respond" it and definitely don't delete it... just ehh continue as if nothing had happened and maybe the error will be gone in 100ms xD...

#ifdef ABC_MULTI_CAMPAIGN_OWNER_MODE
                GetAndSubscribeCacheItem *cacheItem = m_GetAndSubscribeCacheHash.at(originalRequest->CouchbaseGetKeyInput);
                //exception thrown if not found, but if we get to this code path then it has to be there (hence no try/catch)
                cacheItem->startPollingTimeoutTimer();
#else // not #def ABC_MULTI_CAMPAIGN_OWNER_MODE
                event_add(m_GetAndSubscribePollingTimeout, &m_OneHundredMilliseconds);
#endif // ABC_MULTI_CAMPAIGN_OWNER_MODE
            }
            else
            {
                if(originalRequest->SaveCAS)
                {
                    GetCouchbaseDocumentByKeyRequest::respondWithCAS(originalRequest, "", 0, false, true);
                }
                else
                {
                    GetCouchbaseDocumentByKeyRequest::respond(originalRequest, "", false, true);
                }
                m_AutoRetryingWithExponentialBackoffCouchbaseGetRequestCache.push_back(autoRetryingWithExponentialBackoffCouchbaseGetRequest);
                delete originalRequest;
            }
            return;
        }

        //LCB_KEY_ENOENT (key does not exist)
        if(originalRequest->SaveCAS)
        {
            GetCouchbaseDocumentByKeyRequest::respondWithCAS(originalRequest, "", 0, false, false);
        }
        else
        {
            GetCouchbaseDocumentByKeyRequest::respond(originalRequest, "", false, false);
        }
        m_AutoRetryingWithExponentialBackoffCouchbaseGetRequestCache.push_back(autoRetryingWithExponentialBackoffCouchbaseGetRequest);
        delete originalRequest;
        return;
    }

    //getting here means no more exponential retrying for this get request is necessary, so recycle it into the cache
    m_AutoRetryingWithExponentialBackoffCouchbaseGetRequestCache.push_back(autoRetryingWithExponentialBackoffCouchbaseGetRequest);

    if(originalRequest->GetAndSubscribe != 0 && originalRequest->GetAndSubscribe != 6)
    {
        //TODOreq: handle case where key doesn't exist (but since we're manually setting up get and subscribe shit for now (including recompilations), that is ridiculously unlikely)

        //TODOoptimization: still iffy on if this get and subscribe stuff needs to be on it's own thread, so as not to congest traditional gets/stores when there is an update

        //TO DOnereq (not sure about the end of this comment, but deleting of the cache happens when timeout occurs and that slot sees it as being empty): the last person to unsubscribe himself should also remove the cache item from m_GetAndSubscribeCacheHash and delete the cache item (see the catch code path's comments (this is an other time we need to "???" <-, but is it the only?)

        GetAndSubscribeCacheItem *cacheItem;
#if 0
        try //TO DOnereq(a get and subscribe poll means that the cache item definitely exists, since we process unsubscribe requests in the poll callback just below here): this try might not be necessary, depending what we end up doing for unsubscribe code
        {
#endif
            cacheItem = m_GetAndSubscribeCacheHash.at(originalRequest->CouchbaseGetKeyInput);

            //exception thrown if not found, but if we get to this code path then it has to be there (hence no try/catch)

            //the following two while loops are our processing of unsubscribe/change-session id requests. we could have done it right when we got the requests in eventSlotForWtGet, but we do it here instead to prevent a segfault when request hackily being used requests unsubscribe/change-session-id during the time it's being hackily used. now that we've finished using the request to pull out our cacheItem, it's ok if it gets deleted
            while(!cacheItem->UnsubscribeRequestsToProcessMomentarily.empty())
            {
                //process unsubscribe requests
                GetCouchbaseDocumentByKeyRequest *originalUnsubscribeRequestFromWt = static_cast<GetCouchbaseDocumentByKeyRequest*>(cacheItem->UnsubscribeRequestsToProcessMomentarily.back());
                cacheItem->UnsubscribeRequestsToProcessMomentarily.pop_back();

                GetCouchbaseDocumentByKeyRequest *existingSubscription;
                try
                {
                    existingSubscription = static_cast<GetCouchbaseDocumentByKeyRequest*>(cacheItem->NewSubscribers.at(originalUnsubscribeRequestFromWt->AnonymousBitcoinComputingWtGUIPointerForCallback));

                    //is in new subscribers
                    cacheItem->NewSubscribers.erase(originalUnsubscribeRequestFromWt->AnonymousBitcoinComputingWtGUIPointerForCallback); //see TODOoptimization in change session id about using a find iterator instead
                    delete existingSubscription;
                }
                catch(std::out_of_range &notInNewSubscribersException)
                {
                    try
                    {
                        existingSubscription = static_cast<GetCouchbaseDocumentByKeyRequest*>(cacheItem->Subscribers.at(originalUnsubscribeRequestFromWt->AnonymousBitcoinComputingWtGUIPointerForCallback));

                        //is in old subscribers
                        cacheItem->Subscribers.erase(originalUnsubscribeRequestFromWt->AnonymousBitcoinComputingWtGUIPointerForCallback); //see TODOoptimization in change session id about using a find iterator instead
                        delete existingSubscription;
                    }
                    catch(std::out_of_range &notInOldSubscribersException)
                    {
                        //double  unsubscribe is quite likely so we don't care when it happens
                    }
                }

                //always delete the unsubscribe request
                delete originalUnsubscribeRequestFromWt;
            }
            while(!cacheItem->ChangeSessionIdRequestsToProcessMomentarily.empty())
            {
                //process change session id requests
                GetCouchbaseDocumentByKeyRequest *originalChangeSessionIdRequestFromWt = static_cast<GetCouchbaseDocumentByKeyRequest*>(cacheItem->ChangeSessionIdRequestsToProcessMomentarily.back());
                cacheItem->ChangeSessionIdRequestsToProcessMomentarily.pop_back();

                GetCouchbaseDocumentByKeyRequest *existingSubscription;
                try
                {
                    existingSubscription = static_cast<GetCouchbaseDocumentByKeyRequest*>(cacheItem->NewSubscribers.at(originalChangeSessionIdRequestFromWt->AnonymousBitcoinComputingWtGUIPointerForCallback));

                    //is in new subscribers
                    //delete the old session id's request, but we're about to 'insert' so no need to erase from hash
                    delete existingSubscription;
                }
                catch(std::out_of_range &notInNewSubscribersException)
                {
                    try
                    {
                        existingSubscription = static_cast<GetCouchbaseDocumentByKeyRequest*>(cacheItem->Subscribers.at(originalChangeSessionIdRequestFromWt->AnonymousBitcoinComputingWtGUIPointerForCallback));

                        //is in old subscribers
                        cacheItem->Subscribers.erase(originalChangeSessionIdRequestFromWt->AnonymousBitcoinComputingWtGUIPointerForCallback); //TODOoptimization: we could use 'find' instead of 'at' and already have the iterator, which makes for faster erasing here (fuckit)
                        delete existingSubscription;
                    }
                    catch(std::out_of_range &notInOldSubscribersException)
                    {
                        //TODOoptional:
                        cerr << "i'm not sure what it means if we get a change session id request for a wapplication that isn't subscribed... so for now give an error and just continue onto other change session id requests doing nothing with this one" << endl;
                        delete originalChangeSessionIdRequestFromWt;
                        continue; //had return, but that is error
                    }
                }

                //if we get here then the old request was 'delete'd (and if it was in old subscribers hash, it was removed from it), so now we just add to new subscribers (which would overwrite the old one if it was even there)
                cacheItem->NewSubscribers[originalChangeSessionIdRequestFromWt->AnonymousBitcoinComputingWtGUIPointerForCallback] = originalChangeSessionIdRequestFromWt;
                //we put ourself in NewSubscribers because we definitely want the next poll. we also don't even know if the old session id is in new subscribers or old subscribers list, but putting it into new will get it put into old soon anyways
            }


            //traditional subscription update checks (now that we've handled unsubscribes + change session ids)
            cacheItem->CurrentlyFetchingPossiblyNew = false;
            std::string possiblyNewValueDependingOnCas = std::string(static_cast<const char*>(resp->v.v0.bytes), resp->v.v0.nbytes);
            lcb_cas_t casToTellUsIfTheDocChangedOrNot = resp->v.v0.cas;

            //new subscribers want what we just got no matter what
            BOOST_FOREACH(SubscribersType::value_type &v, cacheItem->NewSubscribers)
            {
                if(static_cast<GetCouchbaseDocumentByKeyRequest*>(v.second)->GetAndSubscribe != 4)
                {
                    GetCouchbaseDocumentByKeyRequest::respondWithCAS(static_cast<GetCouchbaseDocumentByKeyRequest*>(v.second), possiblyNewValueDependingOnCas, casToTellUsIfTheDocChangedOrNot, true, false);
                }
            }

            //old subscribers only notified if the cas has changed
            if(cacheItem->DocumentCAS != casToTellUsIfTheDocChangedOrNot) //did it change?
            {
                //it changed (or is new (cas is 0 initially), so store it and notify old subscribers (if any)
                cacheItem->Document = possiblyNewValueDependingOnCas;
                cacheItem->DocumentCAS = casToTellUsIfTheDocChangedOrNot;

                BOOST_FOREACH(SubscribersType::value_type &v, cacheItem->Subscribers)
                {
                    GetCouchbaseDocumentByKeyRequest::respondWithCAS(static_cast<GetCouchbaseDocumentByKeyRequest*>(v.second), possiblyNewValueDependingOnCas, casToTellUsIfTheDocChangedOrNot, true, false);
                }
            }

            SubscribersType noJsSingleGetHackRequestsThatWantThe_NEXT_RequestBecauseThisOneMightBeOffBecauseOfARaceConditionLoL;

            //new subscribers become old subscribers no matter what (unless they are no-js single get hack, in which case we delete them :-P)
            BOOST_FOREACH(SubscribersType::value_type &v, cacheItem->NewSubscribers)
            {
                GetCouchbaseDocumentByKeyRequest *getCouchbaseDocumentByKeyRequest = static_cast<GetCouchbaseDocumentByKeyRequest*>(v.second);
                if(getCouchbaseDocumentByKeyRequest->GetAndSubscribe == 4)
                {
                    //'4' wasn't responded to yet, and we hackily make it in new subscribers for the NEXT one
                    noJsSingleGetHackRequestsThatWantThe_NEXT_RequestBecauseThisOneMightBeOffBecauseOfARaceConditionLoL[v.first] = v.second;
                    getCouchbaseDocumentByKeyRequest->GetAndSubscribe = 5;
                }
                else if(getCouchbaseDocumentByKeyRequest->GetAndSubscribe == 5)
                {
                    //special no-js single get hack (is already responded to, so now just delete)
                    delete getCouchbaseDocumentByKeyRequest;
                }
                else
                {
                    //normal making of new subscriber old subscriber
                    cacheItem->Subscribers[v.first] = v.second;
                }
            }
            cacheItem->NewSubscribers.clear();

            //no-js single get 'add back to new subscribers' [for NEXT poll result] hack
            BOOST_FOREACH(SubscribersType::value_type &v, noJsSingleGetHackRequestsThatWantThe_NEXT_RequestBecauseThisOneMightBeOffBecauseOfARaceConditionLoL)
            {
                cacheItem->NewSubscribers[v.first] = v.second;
            }

#ifdef ABC_MULTI_CAMPAIGN_OWNER_MODE
            cacheItem->startPollingTimeoutTimer();
#else // not #def ABC_MULTI_CAMPAIGN_OWNER_MODE
            event_add(m_GetAndSubscribePollingTimeout, &m_OneHundredMilliseconds);
#endif
#if 0
        }
        catch(std::out_of_range &keyNotInCacheException)
        {
            //TO DOnereq
            //this is a race conditon where the last subscriber has unsubscribed but we still had a get dispatched (HOLY SHIT WE COULD SEGFAULT IF THE POINTER TO GetCouchbaseDocumentByKeyRequest WAS DELETED DURING UNSUBSCRIBE TO DOnereq). i think all it means is that we don't want to do the 100ms timer again...
            //we should probably also remove the cacheItem from the list and delete the pointer, SO THAT the cache 'misses' when someone new subscribes and then they do a full get and the timer shits starts back up
            //^^that's probably not the only time we need to [??? to what ???]
        }
#endif
        //ABC_GOT_A_PENDING_RESPONSE_FROM_COUCHBASE(Get)
        return; //don't delete the request hackily used for polling for two reasons: 1) might still be a subscriber, 2) if it unsubscribed it has already been deleted
    }

    if(originalRequest->SaveCAS) //TODOoptimization: __unlikely or whatever i can find in boost
    {
        GetCouchbaseDocumentByKeyRequest::respondWithCAS(originalRequest, std::string(static_cast<const char*>(resp->v.v0.bytes), resp->v.v0.nbytes), resp->v.v0.cas, true, false);
    }
    else
    {
        GetCouchbaseDocumentByKeyRequest::respond(originalRequest, std::string(static_cast<const char*>(resp->v.v0.bytes), resp->v.v0.nbytes), true, false);
    }

    delete originalRequest;
}
//context is everything
void AnonymousBitcoinComputingCouchbaseDB::decrementPendingGetCountAndHandle()
{
    ABC_GOT_A_PENDING_RESPONSE_FROM_COUCHBASE(Get)
}
#ifdef ABC_DO_COUCHBASE_DURABILITY_POLL_BEFORE_CONSIDERING_STORE_COMPLETE
void AnonymousBitcoinComputingCouchbaseDB::durabilityCallbackStatic(lcb_t instance, const void *cookie, lcb_error_t error, const lcb_durability_resp_t *resp)
{
    const_cast<AnonymousBitcoinComputingCouchbaseDB*>(static_cast<const AnonymousBitcoinComputingCouchbaseDB*>(lcb_get_cookie(instance)))->durabilityCallback(cookie, error, resp);
}
void AnonymousBitcoinComputingCouchbaseDB::durabilityCallback(const void *cookie, lcb_error_t error, const lcb_durability_resp_t *resp)
{
    AutoRetryingWithExponentialBackoffCouchbaseStoreRequest *autoRetryingWithExponentialBackoffCouchbaseStoreRequest = const_cast<AutoRetryingWithExponentialBackoffCouchbaseStoreRequest*>(static_cast<const AutoRetryingWithExponentialBackoffCouchbaseStoreRequest*>(cookie));
    StoreCouchbaseDocumentByKeyRequest *originalRequest = autoRetryingWithExponentialBackoffCouchbaseStoreRequest->storeRequestRetrying();

    if(error != LCB_SUCCESS)
    {
        cerr << "Error: Generic lcb_durability_poll callback failure: " << lcb_strerror(m_Couchbase, error) << endl;
        if(originalRequest->SaveCasOutput)
        {
            StoreCouchbaseDocumentByKeyRequest::respondWithCas(originalRequest, originalRequest->CasInput, true, true); //another use of that cas hack (3rd)
        }
        else
        {
            StoreCouchbaseDocumentByKeyRequest::respond(originalRequest, true, true);
        }
        //TODOoptional: if we make it to durability, the lcb op has succeeded?
        //^Yes, but what the fuck are the implications of that??? My business logic for now assumes that db error lcb op failed (it doesn't even check it). It's especially worth noting that a 'recovery possy' member might see an LCB_ADD whose replication failed (here) and then do a bunch of recovery steps.... and/but our business will be out of sync in some obscure way I have never thought about (maybe it doesn't mess anything up, maybe the world ends)

        delete originalRequest;
        return;
    }
    if(resp->v.v0.err != LCB_SUCCESS)
    {
        cerr << "Error: Specific lcb_durability_poll callback failure: " << lcb_strerror(m_Couchbase, resp->v.v0.err) << endl;
        if(originalRequest->SaveCasOutput)
        {
            StoreCouchbaseDocumentByKeyRequest::respondWithCas(originalRequest, originalRequest->CasInput, true, true); //another use of that cas hack (4th)
        }
        else
        {
            StoreCouchbaseDocumentByKeyRequest::respond(originalRequest, true, true);
        }
        //TODOoptional: if we make it to durability, the lcb op has succeeded?
        //^Yes, see same req above for concerns

        delete originalRequest;
        return;
    }

    if(originalRequest->SaveCasOutput)
    {
        StoreCouchbaseDocumentByKeyRequest::respondWithCas(originalRequest, originalRequest->CasInput, true, false); //it's a HACK (2nd) that you can see in storeCallback, my using of the casInput field to hold onto (until we get to here, the durability callback) what is actually the cas OUTPUT. since you see this request is about to be deleted in a few lines (and we don't use the serializer to get back to Wt, we use a .Post, this appears safe
    }
    else
    {
        StoreCouchbaseDocumentByKeyRequest::respond(originalRequest, true, false);
    }

    delete originalRequest;
}
#endif // ABC_DO_COUCHBASE_DURABILITY_POLL_BEFORE_CONSIDERING_STORE_COMPLETE
//had some lulz just now thinking that it'd be smart to periodically vacuum the get new or recycled exponential backoff vectors... except that makes absolutely no sense at all because they are there to help with high server load, and vacuuming them would only help, YOU GUESSED IT, at high server load...
#ifdef ABC_MULTI_CAMPAIGN_OWNER_MODE
void AnonymousBitcoinComputingCouchbaseDB::getAndSubscribePollingTimeoutEventSlot(GetAndSubscribeCacheItem *cacheItem)
{
    //get the first subscriber found. if there aren't any subscribers found, we don't do the get and then remove it from m_GetAndSubscribeCacheHash and delete the cache item
    GetCouchbaseDocumentByKeyRequest *originalRequest = NULL; //had it called 'aRequestToHackilyUseLoL' but eh MACRO
    if(!cacheItem->Subscribers.empty())
    {
        originalRequest = static_cast<GetCouchbaseDocumentByKeyRequest*>(cacheItem->Subscribers.begin()->second);
    }
    else if(!cacheItem->NewSubscribers.empty())
    {
        originalRequest = static_cast<GetCouchbaseDocumentByKeyRequest*>(cacheItem->NewSubscribers.begin()->second);
    }

    if(originalRequest == NULL)
    {
        //delete cache item since no subscribers. the unsubscribe code could have done it, but it's better to do it here because then subscribers can be add-removed-add-removed and so on changing between "0 and 1" rapidly without us having to delete/new over and over with each one. here we give 100ms leeway in which it can be empty for (at a maximum (could only be 1ms, we dunno/care (as in, don't care)). TODOreq: when a subscriber is added, the cacheItem needs to either be deleted or the timer needs to be running. if the cache item is deleted, we re-start the timer. this isn't future proof, my design here with the timer is only for one cacheItem (/lazy)

        m_GetAndSubscribeCacheHash.erase(cacheItem->Key);
        delete cacheItem;
        return;
    }

    cacheItem->CurrentlyFetchingPossiblyNew = true;
    ABC_SCHEDULE_COUCHBASE_REQUEST_USING_NEW_OR_RECYCLED_AUTO_RETRYING_WITH_EXPONENTIAL_BACKOFF(Get)
    //TODOreq: as i have it coded now, if i did two subscribables, they would each event_add the same timer using their own "100ms" thingy. not sure if they'd overwrite each other or what, but yea i need to fix that before ever doing multiple subscribables
}
#else // not #def ABC_MULTI_CAMPAIGN_OWNER_MODE
void AnonymousBitcoinComputingCouchbaseDB::getAndSubscribePollingTimeoutEventSlotStatic(int unusedSocket, short events, void *userData)
{
    (void)unusedSocket;
    (void)events;
    static_cast<AnonymousBitcoinComputingCouchbaseDB*>(userData)->getAndSubscribePollingTimeoutEventSlot();
}
void AnonymousBitcoinComputingCouchbaseDB::getAndSubscribePollingTimeoutEventSlot()
{
    //TO DOnereq(they don't, except the one hackily used for polling ofc): subscribers shouldn't take up an AutoRetry thingy, but if they reeaaally need to because it would warrant a huge refactor otherwise, i guess it's ok :-/...
    std::list<std::string> listOfKeysWithNoSubscribers;
    BOOST_FOREACH(GetAndSubscribeCacheHashType::value_type &v, m_GetAndSubscribeCacheHash)
    {
        GetAndSubscribeCacheItem *cacheItem = v.second;

        //get the first subscriber found. if there aren't any subscribers found, we don't do the get and then remove it from m_GetAndSubscribeCacheHash and delete the cache item

        GetCouchbaseDocumentByKeyRequest *originalRequest = NULL; //had it called 'aRequestToHackilyUseLoL' but eh MACRO
        if(!cacheItem->Subscribers.empty())
        {
            originalRequest = static_cast<GetCouchbaseDocumentByKeyRequest*>(cacheItem->Subscribers.begin()->second);
        }
        else if(!cacheItem->NewSubscribers.empty())
        {
            originalRequest = static_cast<GetCouchbaseDocumentByKeyRequest*>(cacheItem->NewSubscribers.begin()->second);
        }

        if(originalRequest != NULL)
        {
            cacheItem->CurrentlyFetchingPossiblyNew = true;
            ABC_SCHEDULE_COUCHBASE_REQUEST_USING_NEW_OR_RECYCLED_AUTO_RETRYING_WITH_EXPONENTIAL_BACKOFF(Get)
            //TODOreq: as i have it coded now, if i did two subscribables, they would each event_add the same timer using their own "100ms" thingy. not sure if they'd overwrite each other or what, but yea i need to fix that before ever doing multiple subscribables
        }
        else
        {
            //add it to a list to be deleted once we stop iterating them (mutable?) should we delete cache item since no subscribers. the unsubscribe code could have done it, but it's better to do it here because then subscribers can be add-removed-add-removed and so on changing between "0 and 1" rapidly without us having to delete/new over and over with each one. here we give 100ms leeway in which it can be empty for (at a maximum (could only be 1ms, we dunno/care (as in, don't care)). TODOreq: when a subscriber is added, the cacheItem needs to either be deleted or the timer needs to be running. if the cache item is deleted, we re-start the timer. this isn't future proof, my design here with the timer is only for one cacheItem (/lazy)
            listOfKeysWithNoSubscribers.push_back(v.first);
        }
    }
    BOOST_FOREACH(std::string &keyWithNoSubscribersToDelete, listOfKeysWithNoSubscribers)
    {
        GetAndSubscribeCacheItem *cacheItemWithNoSubscribersToDelete = m_GetAndSubscribeCacheHash.at(keyWithNoSubscribersToDelete);
        m_GetAndSubscribeCacheHash.erase(keyWithNoSubscribersToDelete); //TODOoptimziation: find(), use iterator to erase() -- same as unsubscribe/change-sessionId code
        delete cacheItemWithNoSubscribersToDelete;
    }
}
#endif // ABC_MULTI_CAMPAIGN_OWNER_MODE
void AnonymousBitcoinComputingCouchbaseDB::beginStoppingCouchbaseCleanlyEventSlotStatic(evutil_socket_t unusedSocket, short events, void *userData)
{
    (void)unusedSocket;
    (void)events;
    static_cast<AnonymousBitcoinComputingCouchbaseDB*>(userData)->beginStoppingCouchbaseCleanlyEventSlot();
}
void AnonymousBitcoinComputingCouchbaseDB::beginStoppingCouchbaseCleanlyEventSlot()
{
    m_NoMoreAllowedMuahahaha = true;
    //TO DOnereq (see below optimization): set up a bool flag in the ADD/GET event slots to refuse to start more. Finish the ones we're already working on (where get list? or maybe i just keep a count of each (cheaper)?), then perhaps do a notify_one() back to main thread or some such...
    //TODOoptimization: don't implement this yet, but it would be good to have a timeout for the above (if the count doesn't become zero on time etc)

    //maybe we got lucky, or maybe there is no activity on the server
    if(m_PendingStoreCount == 0 && m_PendingGetCount == 0)
    {
        notifyMainThreadWeAreFinishedWithAllPendingRequests();
    }

    //EVENTUALLY, BUT NOT IN THIS METHOD:
    //event_base_loopbreak((struct event_base *)userData);
}
void AnonymousBitcoinComputingCouchbaseDB::finalCleanupAndJoin(int unusedSocket, short events, void *userData)
{
    (void)unusedSocket;
    (void)events;
    event_base_loopbreak((event_base*)userData);
    //^^lcb_breakout instead?
}
void AnonymousBitcoinComputingCouchbaseDB::notifyMainThreadWeAreFinishedWithAllPendingRequests()
{
    {
        boost::lock_guard<boost::mutex> isFinishedWithAllPendingRequestsLock(m_IsFinishedWithAllPendingRequestsMutex);
        m_IsFinishedWithAllPendingRequests = true;
    }
    m_IsFinishedWithAllPendingRequestsWaitCondition.notify_one();
}
void AnonymousBitcoinComputingCouchbaseDB::scheduleGetRequest(AutoRetryingWithExponentialBackoffCouchbaseGetRequest *autoRetryingWithExponentialBackoffCouchbaseGetRequest)
{
    lcb_get_cmd_t cmd;
    const lcb_get_cmd_t *cmds[1];
    cmds[0] = &cmd;
    GetCouchbaseDocumentByKeyRequest *originalRequest = autoRetryingWithExponentialBackoffCouchbaseGetRequest->getRequestRetrying();
    const char *keyInput = originalRequest->CouchbaseGetKeyInput.c_str();
    memset(&cmd, 0, sizeof(cmd));
    cmd.v.v0.key = keyInput;
    cmd.v.v0.nkey = strlen(keyInput);
    lcb_error_t error = lcb_get(m_Couchbase, autoRetryingWithExponentialBackoffCouchbaseGetRequest, 1, cmds);
    if(error  != LCB_SUCCESS)
    {
        cerr << "Failed to setup get request: " << lcb_strerror(m_Couchbase, error) << endl;
        if(originalRequest->SaveCAS)
        {
            GetCouchbaseDocumentByKeyRequest::respondWithCAS(originalRequest, "", 0, false, true);
        }
        else
        {
            GetCouchbaseDocumentByKeyRequest::respond(originalRequest, "", false, true);
        }
        delete originalRequest;
        return;
    }
    ++m_PendingGetCount; //TODOoptional: overflow? meh not worried about it for now xD
}
void AnonymousBitcoinComputingCouchbaseDB::scheduleStoreRequest(AutoRetryingWithExponentialBackoffCouchbaseStoreRequest *autoRetryingWithExponentialBackoffCouchbaseStoreRequest)
{

    lcb_store_cmd_t cmd;
    const lcb_store_cmd_t *cmds[1];
    cmds[0] = &cmd;
    memset(&cmd, 0, sizeof(cmd));
    StoreCouchbaseDocumentByKeyRequest *originalRequest = autoRetryingWithExponentialBackoffCouchbaseStoreRequest->storeRequestRetrying();
    const char *keyInput = originalRequest->CouchbaseStoreKeyInput.c_str();
    const char *documentInput = originalRequest->CouchbaseStoreDocumentInput.c_str();
    cmd.v.v0.key = keyInput;
    cmd.v.v0.nkey = originalRequest->CouchbaseStoreKeyInput.length();
    cmd.v.v0.bytes = documentInput;
    cmd.v.v0.nbytes = originalRequest->CouchbaseStoreDocumentInput.length();
    if(originalRequest->LcbStoreModeIsAdd)
    {
        cmd.v.v0.operation = LCB_ADD;
    }
    else
    {
        cmd.v.v0.operation = LCB_SET;
        if(originalRequest->HasCasInput)
        {
            cmd.v.v0.cas = originalRequest->CasInput;
        }
    }
    lcb_error_t err = lcb_store(m_Couchbase, autoRetryingWithExponentialBackoffCouchbaseStoreRequest, 1, cmds);
    if(err != LCB_SUCCESS)
    {
        cerr << "Failed to set up store request: " << lcb_strerror(m_Couchbase, err) << endl;
        if(originalRequest->SaveCasOutput)
        {
            StoreCouchbaseDocumentByKeyRequest::respondWithCas(originalRequest, 0, false, true);
        }
        else
        {
            StoreCouchbaseDocumentByKeyRequest::respond(originalRequest, false, true);
        }
        delete originalRequest; //TODOreq: consider the implications of deleting that if we're hackily using it for get-and-subscribe? for now just crossing my fingers :). oh btw TODOreq: is it lcb_store that returns LCB_CLIENT_ENOMEM (warranting exponential backoff/retry here too)?
        return;
    }
    ++m_PendingStoreCount;
}
void AnonymousBitcoinComputingCouchbaseDB::eventSlotForWtStore()
{
    ABC_READ_STORE_REQUEST_FROM_APPROPRIATE_MESSAGE_BUFFER(Store)
    ABC_SCHEDULE_COUCHBASE_REQUEST_USING_NEW_OR_RECYCLED_AUTO_RETRYING_WITH_EXPONENTIAL_BACKOFF(Store)
}
#ifndef ABC_USE_BOOST_LOCKFREE_QUEUE //hack. couldn't boost pp repeat these since Get is way different (get and subscribe hackery (which, if moved to a separate object entirely, would probably solve the problem))...
void AnonymousBitcoinComputingCouchbaseDB::eventSlotForWtStoreLarge()
{
    ABC_READ_STORE_REQUEST_FROM_APPROPRIATE_MESSAGE_BUFFER(StoreLarge)
    ABC_SCHEDULE_COUCHBASE_REQUEST_USING_NEW_OR_RECYCLED_AUTO_RETRYING_WITH_EXPONENTIAL_BACKOFF(Store) //not StoreLarge!
}
#endif // ABC_USE_BOOST_LOCKFREE_QUEUE
void AnonymousBitcoinComputingCouchbaseDB::eventSlotForWtGet()
{
    if(m_NoMoreAllowedMuahahaha) //TODOoptional: we should still allow unsubscribe requests to be processed? probably doesn't matter but idk tbh...
    {
        return;
        //TODOoptional: post() a response, BUT really that doesn't matter too much because i mean the server's about to go down regardless... (unless i'm taking db down ONLY and wt is staying up (lol wut?))
    }

#ifdef ABC_USE_BOOST_LOCKFREE_QUEUE
    GetCouchbaseDocumentByKeyRequest *originalRequest = m_GetMessageQueuesCurrentMessageBuffer;
#else
    GetCouchbaseDocumentByKeyRequest *originalRequest = new GetCouchbaseDocumentByKeyRequest(); //TODOoptimization: get new or recycled maybe? idfk. i think since i'm always so tempted to do get new or recycled all around my code, i should finally benchmark it... xD. still i've heard that "allocations are ridiculously slow", and this makes sense especially considering there's a mutex lock involved. i think i benchmarked something similar a while back, but it had some other qt shit involved so idk

    {
        std::istringstream getCouchbaseDocumentByKeyRequestFromWtSerialized(static_cast<const char*>(m_GetMessageQueuesCurrentMessageBuffer));
        boost::archive::text_iarchive deSerializer(getCouchbaseDocumentByKeyRequestFromWtSerialized);
        deSerializer >> *originalRequest;
    }
#endif // ABC_USE_BOOST_LOCKFREE_QUEUE

    if(originalRequest->GetAndSubscribe != 0)
    {
        //thought about just querying the key [against a list] to see if it's 'get and subscribe' mode, but using a bool like this makes it scale betterer in the futuerer

        GetAndSubscribeCacheItem *keyValueCacheItem;
        try
        {
            //TODOreq: if we've just scheduled our 100ms poll to couchbase and are expecting a fresh value soon, we should just add ourself to the subscribers list. we could respond with the probably-about-to-be-made-stale one, but we shouldn't
            //TO DOnereq(NewSubscribers): if the 100ms poll doesn't return a 'new' doc (based on cas), there's still going to be a list (sometimes of size zero) of subscribers who are interested in being given _A_ result. The directly above T0d0req mentions a kind that would be interested, and there is also the "many successive get-and-subscribes" in a row really fast, where all but the first simply added themselves to said list (that actually implies that there was no cache hit, but eh just like above T0d0req above says: if one is scheduled, we wait for it!)
            //^^So I'm thinking something like m_ListOfSubscribersWhoOnlyWant_CHANGED_Updates and m_ListOfSubscribersWhoDontHaveShitYet (old and new subscribers, respectively). Upon the 100ms poll getting a value (any value), it gives it's result to m_ListOfSubscribersWhoDontHaveShitYet, then checks to see if the cas has changed and then if it has gives it's result to m_ListOfSubscribersWhoOnlyWant_NEW_Updates. Then it adds-while-clearing m_ListOfSubscribersWhoDontHaveShitYet _into_ m_ListOfSubscribersWhoOnlyWant_CHANGED_Updates
            //^^^The point is, if some bool 'm_PollIsActive' (for that particular key), then we don't want to do the 'respondWithCAS' a few lines down

            //first thought of unsubscribe segfault, before completely comprehending:
            //TO DOnereq(problem-described-backwards-but-yet): for the actual poll / cache miss, I need to use ONE OF the GetCouchbaseDocumentByKeyRequests (subscribers) to do the get itself using existing infrastructure. It will be easy to "pull back out" (as being GetAndSubscribe 'mode') because of the same bool I used to get to here. BUT doing so is a bit hacky and does lead to a tiny race condition that when happened would make the 100ms now 200ms (or possibly longer, but it gets rarer and rarer). If the end-user for the corresponding GetCouchbaseDocumentByKeyRequest should UNSUBSCRIBE (navigate away, disconnect) while the 'get poll' is taking place, we might not see him in the list anymore and might... idk... disregard the result? Hard to wrap my head around but now that I've typed this shit, I'll double check that I accounted for it later. Might not matter at all depending how I code it (lookup by KEY = doesn't matter)

            //I have written previously that the polling will be lazy. I said it should do a get if 'lastGet' happened >= 100ms ago. I have changed my mind BECAUSE: you could have shit tons of 'subscribers' sitting waiting for their "Post" updates only... and very little of them doing "first page load 'get'ing" (and only the first page load get'ing would go through the code path to see if >= 100ms has elapsed). Since the primary way of pushing updates is to them (masses) via Post without them requiring to do a 'get' (hence the name 'SUBSCRIBE'), I'm going to use an actual timer for polling and not lazy/event-driven polling like I said earlier.

            //try to get
            keyValueCacheItem = m_GetAndSubscribeCacheHash.at(originalRequest->CouchbaseGetKeyInput);

            //CACHE HIT if we get this far and the exception isn't thrown

            //first handle our two special cases (change session id and unsubscribe) by queuing their requests, to be processed when the next poll completes. if we processed them here, we'd run the risk of deleting the message being hackily used to poll the value every 100ms
            if(originalRequest->GetAndSubscribe == 2) //change session id
            {
                keyValueCacheItem->ChangeSessionIdRequestsToProcessMomentarily.push_back(originalRequest);

                //don't continue doing get [and/or subscribe]
                return;
            }
            else if(originalRequest->GetAndSubscribe == 3) //unsubscribe
            {
                keyValueCacheItem->UnsubscribeRequestsToProcessMomentarily.push_back(originalRequest);

                //don't continue doing get [and/or subscribe]
                return;
            }
            else if(originalRequest->GetAndSubscribe == 4) //no-js hack that just wants single update
            {
                //TO DOnereq: if subscription cache doesn't fulfill our [sexual] needs, set GetAndSubscribe to 6 and send it on through as a hard get. setting it to 6 will make sure it doesn't go through regular get and subscibe stuff in the getCallback stage, BUT also uses the "UPDATE" callback when sending the result back to Wt

                //since this chunk of code can happen RIGHT AFTER a buy event (if the no-js user was the one that bought it), what the subscription cache has is PROBABLY stale so we don't want it. but it gets even more complicated because we don't even know if the subscription cache has anything. TO DOnereq(hard get): what if keyValueCacheItem doesn't exist also?

                //give us the next value, no matter what it is, which must/should be the value that we just 'stored' as the buyer (however TO DOnereq(LOLOL solved it via incredibly dirty hacks. '5'. basically making it get the NEXT NEXT polling update): i'm not sure it is always true given race conditions of events being raised etc :-/. will probably be true MOST OF THE TIME though, which makes this ehh fuck (omg so fucking hard to even think about my brain is going to explode and for now i'm just going to 'not give a shit' (tm) and just put it in new subscribers and pull it out hackily, because it should work MOST of the time and eh i am not sure how to make it work all of the time))
                keyValueCacheItem->NewSubscribers[originalRequest->AnonymousBitcoinComputingWtGUIPointerForCallback] = originalRequest;
                return; //don't continue as normal get and subscribe
            }

            //traditional get and subscribe

            //TO DOnereq: the 2nd user to subscribe might get to this code path before the very first 'get' has populated document/cas... so... wtf?
            if(!keyValueCacheItem->CurrentlyFetchingPossiblyNew)
            {
                //the cache item has a value, so we return the new subscriber the value now as an optimization and add him to the old subscribers
                GetCouchbaseDocumentByKeyRequest::respondWithCAS(originalRequest, keyValueCacheItem->Document, keyValueCacheItem->DocumentCAS, true, false);
                keyValueCacheItem->Subscribers[originalRequest->AnonymousBitcoinComputingWtGUIPointerForCallback] = originalRequest;
            }
            else
            {
                //we'll wait for the one that's about to come (especially since there might not even be ONE yet)
                keyValueCacheItem->NewSubscribers[originalRequest->AnonymousBitcoinComputingWtGUIPointerForCallback] = originalRequest;
            }

            //whether or not we're already subscribed, looking up takes just as long as inserting [again possibly], so we just insert. In fact inserting is faster if the key already exists, because then there's just one lookup


            return; //we return because the key was in the cache (even if the doc isn't). we added ourself to the subscribers list, and the timer is already active so we'll get all future updates :)
        }
        catch(std::out_of_range &keyNotInCacheException)
        {
            //CACHE MISS

            if(originalRequest->GetAndSubscribe == 2) //for these two specials: if there is no cache hit, we don't do jack shit (hey it rhymes). ACTUALLY for the changeSessionId one, shouldn't I create the cache item, since clearly they're expressing interest in it (let's ignore the fact that we SHOULD NEVER EVEN GET HERE [because in order to change session-id we have to have been subscribed [TO SOME EXISTING KEY] in the first place])
            {
                //change sessionId
                delete originalRequest;
                return;
            }
            else if(originalRequest->GetAndSubscribe == 3)
            {
                //unsubscribe
                delete originalRequest;
                return;
            }

            //add ourself and if get isn't already scheduled (we're the first), schedule it. If we get here then we might be the first, but we might also be very close after the first so that the very first get (which is already schedule by the first) hasn't finished yet. Once the first POLL is completed, we should never get here
            //^^NOPE, that is subscriber cache, this miss is for just the key itself (first miss creates it)

            if(originalRequest->GetAndSubscribe != 4)
            {
                //create cache item, even though we have nothing to put in it yet. TODOreq: we need to make sure that we wait for the first get to finish before we ever give someone the "Document" from the cache item (BECAUSE IT IS EMPTY)

#ifdef ABC_MULTI_CAMPAIGN_OWNER_MODE
                GetAndSubscribeCacheItem *newKeyValueCacheItem = new GetAndSubscribeCacheItem(originalRequest->CouchbaseGetKeyInput);
#else // not #def ABC_MULTI_CAMPAIGN_OWNER_MODE
                GetAndSubscribeCacheItem *newKeyValueCacheItem = new GetAndSubscribeCacheItem();
#endif

                //if we get here, we definitely need to do the first 'get' (when it returns, it sets up the 100ms poller (TO DOnereq(we still do the timeout if all unsubscribed by then, but the timeout callback doesn't do the db get (and hence the timeout doesn't get rescheduled a 2nd time either)): unless all users have unsubscribed by then xD))

                m_GetAndSubscribeCacheHash[originalRequest->CouchbaseGetKeyInput] = newKeyValueCacheItem;
                keyValueCacheItem = newKeyValueCacheItem;
                keyValueCacheItem->NewSubscribers[originalRequest->AnonymousBitcoinComputingWtGUIPointerForCallback] = originalRequest;
            }
            else //GetAndSubscribe == 4 (no-js single get hack)
            {
                //get and subscribe cache doesn't have anything for the 'no-js' single get hack, so set to 6 for special hard db get, and then doeeeet
                originalRequest->GetAndSubscribe = 6;
            }
            //we don't return, because we want to actually do the get!
        }
    }

    //get new or recycled AutoRetryingWithExponentialBackoffCouchbaseGetRequest
    ABC_SCHEDULE_COUCHBASE_REQUEST_USING_NEW_OR_RECYCLED_AUTO_RETRYING_WITH_EXPONENTIAL_BACKOFF(Get)
}
//queue<StoreCouchbaseDocumentByKeyRequest*> *AnonymousBitcoinComputingCouchbaseDB::getStoreLockFreeQueueForWt0()
//{
//    return m_StoreWtMessageQueue0;
//}
BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO, ABC_COUCHBASE_LOCKFREE_QUEUE_GETTER_MEMBER_DEFINITIONS_MACRO)

//struct event *AnonymousBitcoinComputingCouchbaseDB::getStoreEventCallbackForWt0()
//{
//    return m_StoreEventCallbackForWt0;
//}
BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO, ABC_COUCHBASE_LIBEVENTS_GETTER_MEMBER_DEFINITIONS_MACRO)

//void AnonymousBitcoinComputingCouchbaseDB::eventSlotForWtStore0Static(evutil_socket_t unusedSocket, short events, void *userData)
//{
//    (void)unusedSocket;
//    (void)events;
//    static_cast<AnonymousBitcoinComputingCouchbaseDB*>(userData)->eventSlotStore0();
//}
BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO, ABC_COUCHBASE_LIBEVENTS_SLOT_STATIC_METHOD_DEFINITIONS_MACRO)

//void AnonymousBitcoinComputingCouchbaseDB::eventSlotForWtStore0()
//{
//    unsigned int priority;
//    message_queue::size_type actualMessageSize;
//    m_StoreWtMessageQueue0->receive(m_StoreMessageQueuesCurrentMessageBuffer,(message_queue::size_type)HACK_WT_TO_COUCHBASE_MAX_MESSAGE_SIZE_Store, actualMessageSize, priority);
//    eventSlotForWtStore();
//}
BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO, ABC_COUCHBASE_LIBEVENTS_SLOT_METHOD_DEFINITIONS_MACRO)
