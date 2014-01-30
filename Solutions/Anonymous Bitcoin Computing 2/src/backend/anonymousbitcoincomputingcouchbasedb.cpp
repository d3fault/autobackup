#include "anonymousbitcoincomputingcouchbasedb.h"

#include "../frontend2backendRequests/storecouchbasedocumentbykeyrequest.h"
#include "../frontend2backendRequests/getcouchbasedocumentbykeyrequest.h"

using namespace std;

const struct timeval AnonymousBitcoinComputingCouchbaseDB::m_OneHundredMilliseconds = {0,100000};

AnonymousBitcoinComputingCouchbaseDB::AnonymousBitcoinComputingCouchbaseDB()
    : m_Thread(boost::bind(&AnonymousBitcoinComputingCouchbaseDB::threadEntryPoint, this)), m_IsDoneInitializing(false), m_IsConnected(false), m_ThreadExittedCleanly(false), m_IsInEventLoop(false), m_NoMoreAllowedMuahahaha(false), m_PendingAddCount(0), m_PendingGetCount(0), m_IsFinishedWithAllPendingRequests(false)
    //, m_AddWtMessageQueue0(NULL)
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES, COUCHBASE_MESSAGE_QUEUE_DEFINITIONS_MACRO, Add)
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES, COUCHBASE_MESSAGE_QUEUE_DEFINITIONS_MACRO, Get)
    //, m_AddMessageQueuesCurrentMessageBuffer(NULL);
    COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_DEFINITION_MACRO(Add)
    COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_DEFINITION_MACRO(Get)
    //, m_AddEventCallbackForWt0(NULL)
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES, COUCHBASE_LIBEVENTS_MEMBER_DEFINITIONS_MACRO, Add)
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES, COUCHBASE_LIBEVENTS_MEMBER_DEFINITIONS_MACRO, Get)
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
        TELL_MAIN_THREAD_THAT_COUCHBASE_FAILED_MACRO
        return;
    }

    if(evthread_use_pthreads() < 0) //or evthread_use_windows_threads
    {
        cerr << "Error: Failed to evthread_use_pthreads" << endl;
        TELL_MAIN_THREAD_THAT_COUCHBASE_FAILED_MACRO
        return;
    }
    if(evthread_make_base_notifiable(LibEventBaseScopedDeleterInstance.LibEventBase) < 0)
    {
        cerr << "Error: Failed to make event base notifiable" << endl;
        TELL_MAIN_THREAD_THAT_COUCHBASE_FAILED_MACRO
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
            lcb_destroy_io_ops(CouchbaseIoOps);
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
        TELL_MAIN_THREAD_THAT_COUCHBASE_FAILED_MACRO
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
        TELL_MAIN_THREAD_THAT_COUCHBASE_FAILED_MACRO
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
    lcb_set_durability_callback(m_Couchbase, AnonymousBitcoinComputingCouchbaseDB::durabilityCallbackStatic);
    if((error = lcb_connect(m_Couchbase)) != LCB_SUCCESS)
    {
        cerr << "Failed to start connecting libcouchbase instance: " << lcb_strerror(m_Couchbase, error) << endl;
        TELL_MAIN_THREAD_THAT_COUCHBASE_FAILED_MACRO
        lcb_destroy(m_Couchbase);
        return;
    }

    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES, SETUP_WT_TO_COUCHBASE_CALLBACKS_VIA_EVENT_NEW_MACRO, Add)
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES, SETUP_WT_TO_COUCHBASE_CALLBACKS_VIA_EVENT_NEW_MACRO, Get)

    m_GetAndSubscribePollingTimeout = evtimer_new(LibEventBaseScopedDeleterInstance.LibEventBase, &AnonymousBitcoinComputingCouchbaseDB::getAndSubscribePollingTimeoutEventSlotStatic, this);
    m_BeginStoppingCouchbaseCleanlyEvent = event_new(LibEventBaseScopedDeleterInstance.LibEventBase, -1, EV_READ, &AnonymousBitcoinComputingCouchbaseDB::beginStoppingCouchbaseCleanlyEventSlotStatic, this);
    m_FinalCleanUpAndJoinEvent = event_new(LibEventBaseScopedDeleterInstance.LibEventBase, -1, EV_READ, &AnonymousBitcoinComputingCouchbaseDB::finalCleanupAndJoin, LibEventBaseScopedDeleterInstance.LibEventBase);

    m_IsInEventLoop = true; //race conditions? cached results? fuck the police

    event_base_loop(LibEventBaseScopedDeleterInstance.LibEventBase, 0); //wait until event_base_loopbreak is called, processing all events of course

    //cleanup
    //TODOreq: we could try to receive any left over messages in queue here (in order to tell them that they were too late), or just disregard them and quit...
    if(m_IsConnected) //if m_IsConnected isn't true, then we never got our couchbase configuration so we never allocated certain buffers and so we don't need to release them either
    {
#if 0 //only need one buffer, not a buffer per queue
        //free(m_AddMessageQueue0CurrentMessageBuffer); m_AddMessageQueue0CurrentMessageBuffer = NULL;
        BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES, FREE_COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFERS_MACRO, Add)
        BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES, FREE_COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFERS_MACRO, Get)
#endif
        FREE_COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_MACRO(Add)
        FREE_COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_MACRO(Get)

        //delete m_AddWtMessageQueue0; m_AddWtMessageQueue0 = NULL;
        BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES, CLOSE_AND_DELETE_ALL_NEWD_COUCHBASE_MESSAGE_QUEUES_MACRO, Add)
        BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES, CLOSE_AND_DELETE_ALL_NEWD_COUCHBASE_MESSAGE_QUEUES_MACRO, Get)
    }
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES, REMOVE_ALL_MESSAGE_QUEUES_MACRO, Add)
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES, REMOVE_ALL_MESSAGE_QUEUES_MACRO, Get)

    //wtf? valgrind complains if i don't have these event_free's here... but why come it don't complain about all the cross thread ones not being freed (and i thought "all" (including below two) were being free'd at lcb_destroy_io_ops wtf wtf?). Maybe valgrind just isn't catching them because it's all happening at scope exit in struct destructor (lol wut) (in which case it's a valgrind bug i'd say)
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
    cerr << "ERROR: " << lcb_strerror(m_Couchbase, error) << errinfo << endl;

    if(!m_IsConnected)
    {
        TELL_MAIN_THREAD_THAT_COUCHBASE_FAILED_MACRO
    }

    //TODOreq: mucho handlo por que donde ti amo grande gordo gato buneo
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

            //clean-up previous crashed exits via message_queue::remove
            BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES, REMOVE_ALL_MESSAGE_QUEUES_MACRO, Add)
            BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES, REMOVE_ALL_MESSAGE_QUEUES_MACRO, Get)

            //TODOreq: message queues and current message buffers need to be delete'd/free'd at various error cases

            //m_AddWtMessageQueue0 = new message_queue(create_only, "BlahBaseNameAdd0", 200x, 100kb);
            BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES, NEW_AND_CREATE_MY_ADD_MESSAGE_QUEUES_MACRO, ~)
            BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES, NEW_AND_CREATE_MY_GET_MESSAGE_QUEUES_MACRO, ~)

#if 0 //only need one buffer, not a buffer per queue
            //m_AddMessageQueue0CurrentMessageBuffer = malloc(100kb);
            BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES, MALLOC_COUCHBASE_ADD_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFERS_MACRO, ~)
            BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES, MALLOC_COUCHBASE_GET_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFERS_MACRO, ~)
#endif
            MALLOC_COUCHBASE_ADD_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_MACRO()
            MALLOC_COUCHBASE_GET_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_MACRO()

        }
        m_IsConnectedWaitCondition.notify_one();
    }
}
void AnonymousBitcoinComputingCouchbaseDB::storeCallbackStatic(lcb_t instance, const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp)
{
    const_cast<AnonymousBitcoinComputingCouchbaseDB*>(static_cast<const AnonymousBitcoinComputingCouchbaseDB*>(lcb_get_cookie(instance)))->storeCallback(cookie, operation, error, resp);
}
void AnonymousBitcoinComputingCouchbaseDB::storeCallback(const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp)
{
    (void)operation; //TODOoptional: i was thinking of using a flag in my "from wt reqeust object (cookie here)" and to NOT do durability when it's a simple "STORE" (because STOREs _TEND_ to be not as important as "ADD" (though that certainly isn't ALWAYS true~)). the purpose of this comment is to point out that we can see if it's a STORE/ADD right here in the 'operation' variable :-P

    StoreCouchbaseDocumentByKeyRequest *originalRequest = const_cast<StoreCouchbaseDocumentByKeyRequest*>(static_cast<const StoreCouchbaseDocumentByKeyRequest*>(cookie));

    if(error != LCB_SUCCESS)
    {
        if(error != LCB_KEY_EEXISTS)
        {
            cerr << "Failed to store key: " << lcb_strerror(m_Couchbase, error) << endl;
            if(originalRequest->SaveCasOutput)
            {
                StoreCouchbaseDocumentByKeyRequest::respondWithCas(originalRequest, resp->v.v0.cas, false, true);
            }
            else
            {
                StoreCouchbaseDocumentByKeyRequest::respond(originalRequest, false, true);
            }
            END_OF_WT_REQUEST_LIFETIME_IN_DB_BACKEND_MACRO(Add)
            //TODOreq: handle failed store (including decrementing 'pendingAddCount' if appropriate). we should err ehh uhm...
            return;
        }

        //if LCB_ADD'ing, the key already exists. If LCB_SET, then your cas (which is implied otherwise we can't get this error) didn't match when setting (someone modified since you did the get)

        //TODOreq: error handling, except this is a VERY common error (for both add and set) so high as fuck priority. still KISS and doing all "success" cases first...
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
        END_OF_WT_REQUEST_LIFETIME_IN_DB_BACKEND_MACRO(Add)
        return;
    }

    lcb_durability_opts_t lcbDurabilityOptions;
    memset(&lcbDurabilityOptions, 0, sizeof(lcb_durability_opts_t));
#ifdef COUCHBASE_DURABILITY_WAIT_FOR_PERSISTED_COUNT
    lcbDurabilityOptions.v.v0.persist_to = COUCHBASE_DURABILITY_WAIT_FOR_PERSISTED_COUNT;
#endif
    lcbDurabilityOptions.v.v0.replicate_to = COUCHBASE_DURABILITY_WAIT_FOR_REPLICACTION_COUNT;
    lcbDurabilityOptions.v.v0.timeout = 5000000; //5 second timeout is PLENTY, and/but we should probably change this in the place it gets its default if we leave it to zero [just as a TODOoptimization to lessen memory writing]
    lcb_durability_cmd_t lcbDurabilityCommand;
    memset(&lcbDurabilityCommand, 0, sizeof(lcb_durability_cmd_t));
    lcbDurabilityCommand.v.v0.key = resp->v.v0.key;
    lcbDurabilityCommand.v.v0.nkey = resp->v.v0.nkey;

    //in my testing with LCB_ADD, setting lcb->cas to resp->cas didn't work (had: "as expected", but wtf DID i expect?? i guess just a tiny bit of extra verification)... BUT i don't think it matters at all for "LCB_ADD" operations. TODOreq: verify that the cas shit IS working for LCB_SET.. by for example manually locking an account in the couchbase admin area, then running the app and... err wait no... that's the regular cas not durability cas. i guess just run the normal operation twice? does lcb durability not play nice with cas? that's the feeling i'm getting (it sees the key already there and then thinks "replication" is done, even though the cas is old? i haven't a clue and that would suck if true... :-/... I guess the TODOreq is saying that I need to devise some kind of test for this...


    if(originalRequest->HasCasInput) //TODOoptimization i am dumb i think the reason my static_casting didn't work earlier was because i didn't use const keyword. change the ugly casts to these kinds in other occurances
    {
        //LCB_SET implied
        lcbDurabilityCommand.v.v0.cas = resp->v.v0.cas; //TODOreq this is the cas of the new doc? relevant? i think yes...
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
        fprintf(stderr, "Failed to schedule lcb_durability_poll: %s\n", lcb_strerror(m_Couchbase, error));
        //TODOreq: proper error handling bah
    }

    //TODOreq: if durability isn't needed for this particular store, --m_PendingAddCount just like getCallback/etc
}
void AnonymousBitcoinComputingCouchbaseDB::getCallbackStatic(lcb_t instance, const void *cookie, lcb_error_t error, const lcb_get_resp_t *resp)
{
    //TODOoptional: boost::bind might do this recontextualizing for me (hell it might even just be a macro doing the same thing!), but idk and usually only use boost::bind when i'm told to. I find it hilarious that Qt allows you to pass a pointer across a thread, and boost allows you to point to a specific class instance's method... but both say that the other's is "wrong"... and yet they are both C++ frameworks/libraries that provide a lot of the same functionality. I'll tell you one thing: although I'm glad I can still swim in boost land, I much prefer Qt. Much neater and stuff (hard to explain unless you are coder too in which case you already know so what the fuck who am I typing this to?)
    const_cast<AnonymousBitcoinComputingCouchbaseDB*>(static_cast<const AnonymousBitcoinComputingCouchbaseDB*>(lcb_get_cookie(instance)))->getCallback(cookie, error, resp);
}
void AnonymousBitcoinComputingCouchbaseDB::getCallback(const void *cookie, lcb_error_t error, const lcb_get_resp_t *resp)
{
    GetCouchbaseDocumentByKeyRequest *originalRequest = const_cast<GetCouchbaseDocumentByKeyRequest*>(static_cast<const GetCouchbaseDocumentByKeyRequest*>(cookie));

    if(error != LCB_SUCCESS)
    {
        if(error != LCB_KEY_ENOENT)
        {
            //TODOreq: exponential backoff when error qualifies
            //TODOreq: lcb_get_replica when error qualifies

            cerr << "Error couchbase getCallback:" << lcb_strerror(m_Couchbase, error) << endl;
            if(originalRequest->SaveCAS)
            {
                GetCouchbaseDocumentByKeyRequest::respondWithCAS(originalRequest, "", 0, false, true);
            }
            else
            {
                GetCouchbaseDocumentByKeyRequest::respond(originalRequest, 0, 0, false, true);
            }
            //TODOreq: you know the drill~
            END_OF_WT_REQUEST_LIFETIME_IN_DB_BACKEND_MACRO(Get)
            return;
        }

        //LCB_KEY_ENOENT (key does not exist)
        if(originalRequest->SaveCAS)
        {
            GetCouchbaseDocumentByKeyRequest::respondWithCAS(originalRequest, "", 0, false, false);
        }
        else
        {
            GetCouchbaseDocumentByKeyRequest::respond(originalRequest, 0, 0, false, false);
        }
        END_OF_WT_REQUEST_LIFETIME_IN_DB_BACKEND_MACRO(Get)
        return;
    }

    if(originalRequest->GetAndSubscribe)
    {
        //TODOreq: handle case where key doesn't exist (but since we're manually setting up get and subscribe shit for now (including recompilations), that is ridiculously unlikely)

        //TODOoptimization: still iffy on if this get and subscribe stuff needs to be on it's own thread, so as not to congest traditional gets/adds when there is an update

        //TODOreq: when unsubscribing, we check oldsubscribers first but we should also check newsubscribers if it isn't found

        //TODOreq: the last person to unsubscribe himself should also remove the cache item from m_GetAndSubscribeCacheHash and delete the cache item (see the catch code path's comments (this is an other time we need to "???" <-, but is it the only?)

        GetAndSubscribeCacheItem *cacheItem;
        try //TODOreq: this try might not be necessary, depending what we end up doing for unsubscribe code
        {
            cacheItem = m_GetAndSubscribeCacheHash.at(originalRequest->CouchbaseGetKeyInput);

            //exception thrown if not found

            cacheItem->CurrentlyFetchingPossiblyNew = false; //TODOreq: set this back to true after the timer times out (just before get dispatch)
            std::string possiblyNewValueDependingOnCas = std::string(static_cast<const char*>(resp->v.v0.bytes), resp->v.v0.nbytes);
            lcb_cas_t casToTellUsIfTheDocChangedOrNot = resp->v.v0.cas;

            //new subscribers want what we just got no matter what
            BOOST_FOREACH(SubscribersType::value_type &v, cacheItem->NewSubscribers)
            {
                GetCouchbaseDocumentByKeyRequest::respondWithCAS(static_cast<GetCouchbaseDocumentByKeyRequest*>(v.second), possiblyNewValueDependingOnCas, casToTellUsIfTheDocChangedOrNot, true, false);
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

            //new subscribers become old subscribers no matter what
            BOOST_FOREACH(SubscribersType::value_type &v, cacheItem->NewSubscribers)
            {
                cacheItem->Subscribers[v.first] = v.second;
            }
            cacheItem->NewSubscribers.clear();


            //TODOreq: now set up timer? i don't think we want to set up the timer when the key isn't in cache (all unsubscribed)
            event_add(m_GetAndSubscribePollingTimeout, &m_OneHundredMilliseconds);
        }
        catch(std::out_of_range &keyNotInCacheException)
        {
            //TODOreq
            //this is a race conditon where the last subscriber has unsubscribed but we still had a get dispatched (HOLY SHIT WE COULD SEGFAULT IF THE POINTER TO GetCouchbaseDocumentByKeyRequest WAS DELETED DURING UNSUBSCRIBE TODOreq). i think all it means is that we don't want to do the 100ms timer again...
            //we should probably also remove the cacheItem from the list and delete the pointer, SO THAT the cache 'misses' when someone new subscribes and then they do a full get and the timer shits starts back up
            //^^that's probably not the only time we need to [??? to what ???]
        }

        //TODOreq: ?? this return belong here? maybe this is where we'd decrement pending get count? or maybe only on unsubscribe? idfk yet
        --m_PendingGetCount; //TODOreq: rare ish case where this is last pending and we're supposed to be stopping, but don't because we didn't call the macro like the normal gets
        return;
    }

    //TODOreq: who takes ownership of resp and is responsible for deleting it? me or couchbase? when does that occur (since I'm now looking into 'hold onto it because Wt might CAS-swap it')? This memory consideration also applies to all the rest of my couchbase callbacks I'd imagine (but valgrind hasn't complained about them... so idfk. I think since I'm not deleting them, it's safe to assume(xD) that they're only valid during the duration of this callback)
    if(originalRequest->SaveCAS) //TODOoptimization: __unlikely or whatever i can find it boost
    {
        GetCouchbaseDocumentByKeyRequest::respondWithCAS(originalRequest, std::string(static_cast<const char*>(resp->v.v0.bytes), resp->v.v0.nbytes), resp->v.v0.cas, true, false);
    }
    else
    {
        GetCouchbaseDocumentByKeyRequest::respond(originalRequest, resp->v.v0.bytes, resp->v.v0.nbytes, true, false);
    }

    END_OF_WT_REQUEST_LIFETIME_IN_DB_BACKEND_MACRO(Get)
}
void AnonymousBitcoinComputingCouchbaseDB::durabilityCallbackStatic(lcb_t instance, const void *cookie, lcb_error_t error, const lcb_durability_resp_t *resp)
{
    ((AnonymousBitcoinComputingCouchbaseDB*)(lcb_get_cookie(instance)))->durabilityCallback(cookie, error, resp);
}
void AnonymousBitcoinComputingCouchbaseDB::durabilityCallback(const void *cookie, lcb_error_t error, const lcb_durability_resp_t *resp)
{
    StoreCouchbaseDocumentByKeyRequest *originalRequest = const_cast<StoreCouchbaseDocumentByKeyRequest*>(static_cast<const StoreCouchbaseDocumentByKeyRequest*>(cookie));
    if(error != LCB_SUCCESS)
    {
        cerr << "Error: Generic lcb_durability_poll callback failure: " << lcb_strerror(m_Couchbase, error) << endl;
        //TODOreq: handle errors
        if(originalRequest->SaveCasOutput)
        {
            StoreCouchbaseDocumentByKeyRequest::respondWithCas(originalRequest, originalRequest->CasInput, true, true); //another use of that cas hack (3rd)
        }
        else
        {
            StoreCouchbaseDocumentByKeyRequest::respond(originalRequest, true, true);
        }
        //TODOreq: if we make it to durability, the lcb op has succeeded?

        END_OF_WT_REQUEST_LIFETIME_IN_DB_BACKEND_MACRO(Add)
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
        //TODOreq: if we make it to durability, the lcb op has succeeded?

        //TODOreq: handle errors
        END_OF_WT_REQUEST_LIFETIME_IN_DB_BACKEND_MACRO(Add)
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

    END_OF_WT_REQUEST_LIFETIME_IN_DB_BACKEND_MACRO(Add)
}
void AnonymousBitcoinComputingCouchbaseDB::getAndSubscribePollingTimeoutEventSlotStatic(int unusedSocket, short events, void *userData)
{
    (void)unusedSocket;
    (void)events;
    static_cast<AnonymousBitcoinComputingCouchbaseDB*>(userData)->getAndSubscribePollingTimeoutEventSlot();
}
void AnonymousBitcoinComputingCouchbaseDB::getAndSubscribePollingTimeoutEventSlot()
{
    BOOST_FOREACH(GetAndSubscribeCacheHashType::value_type &v, m_GetAndSubscribeCacheHash)
    {
        GetAndSubscribeCacheItem *cacheItem = v.second;

        //get the first subscriber found. proper design would make me less paranoid about this, but i'm not even sure we can guarantee that there are any subscribers. so we should play it safe and if there aren't any subscribers found, we don't do the timeout or even. hell, we could (should?) then delete the cache item and remove it from m_GetAndSubscribeCacheHash (is BOOST_FOREACH mutable??)

        GetCouchbaseDocumentByKeyRequest *getCouchbaseDocumentByKeyRequestFromWt = NULL; //had it called 'aRequestToHackilyUseLoL' but eh MACRO
        if(!cacheItem->Subscribers.empty())
        {
            getCouchbaseDocumentByKeyRequestFromWt = static_cast<GetCouchbaseDocumentByKeyRequest*>(cacheItem->Subscribers.begin()->second);
        }
        else if(!cacheItem->NewSubscribers.empty())
        {
            getCouchbaseDocumentByKeyRequestFromWt = static_cast<GetCouchbaseDocumentByKeyRequest*>(cacheItem->NewSubscribers.begin()->second);
        }

        if(getCouchbaseDocumentByKeyRequestFromWt != NULL)
        {
            cacheItem->CurrentlyFetchingPossiblyNew = true;
            DO_SCHEDULE_COUCHBASE_GET()
        }
        //else, TODOreq should we delete cache item since no subscribers or should someone else (the unsubscribe code?)
    }
}
void AnonymousBitcoinComputingCouchbaseDB::beginStoppingCouchbaseCleanlyEventSlotStatic(evutil_socket_t unusedSocket, short events, void *userData)
{
    (void)unusedSocket;
    (void)events;
    static_cast<AnonymousBitcoinComputingCouchbaseDB*>(userData)->beginStoppingCouchbaseCleanlyEventSlot();
}
void AnonymousBitcoinComputingCouchbaseDB::beginStoppingCouchbaseCleanlyEventSlot()
{
    m_NoMoreAllowedMuahahaha = true;
    //TODOreq: set up a bool flag in the ADD/GET event slots to refuse to start more. Finish the ones we're already working on (where get list? or maybe i just keep a count of each (cheaper)?), then perhaps do a notify_one() back to main thread or some such...
    //TODOoptimization: don't implement this yet, but it would be good to have a timeout for the above (if the count doesn't become zero on time etc)

    //maybe we got lucky, or maybe there is no activity on the server
    if(m_PendingAddCount == 0 && m_PendingGetCount == 0)
    {
        notifyMainThreadWeAreFinishedWithAllPendingRequests();
    }

    //EVENTUALLY, BUT PROBABLY NOT IN THIS METHOD:
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
void AnonymousBitcoinComputingCouchbaseDB::eventSlotForWtAdd() //TODOoptional: change all these "Add" occurances to "Store", now that we LCB_SET too
{
    if(m_NoMoreAllowedMuahahaha)
    {
        return;
    }
    ++m_PendingAddCount; //TODOreq: overflow? meh not worried about it for now

    //TODOreq: delete at appropriate places (error, success)
    StoreCouchbaseDocumentByKeyRequest *storeCouchbaseDocumentByKeyRequestFromWt = new StoreCouchbaseDocumentByKeyRequest();

    {
        std::istringstream addCouchbaseDocumentByKeyRequestFromWtSerialized((const char*)m_AddMessageQueuesCurrentMessageBuffer);
        boost::archive::text_iarchive deSerializer(addCouchbaseDocumentByKeyRequestFromWtSerialized);
        deSerializer >> *storeCouchbaseDocumentByKeyRequestFromWt;
    }

    lcb_store_cmd_t cmd;
    const lcb_store_cmd_t *cmds[1];
    cmds[0] = &cmd;
    memset(&cmd, 0, sizeof(cmd));

    const char *keyInput = storeCouchbaseDocumentByKeyRequestFromWt->CouchbaseAddKeyInput.c_str();
    const char *documentInput = storeCouchbaseDocumentByKeyRequestFromWt->CouchbaseAddDocumentInput.c_str();

    cmd.v.v0.key = keyInput;
    cmd.v.v0.nkey = storeCouchbaseDocumentByKeyRequestFromWt->CouchbaseAddKeyInput.length();
    cmd.v.v0.bytes = documentInput;
    cmd.v.v0.nbytes = storeCouchbaseDocumentByKeyRequestFromWt->CouchbaseAddDocumentInput.length();
    if(storeCouchbaseDocumentByKeyRequestFromWt->LcbStoreModeIsAdd)
    {
        cmd.v.v0.operation = LCB_ADD;
    }
    else
    {
        cmd.v.v0.operation = LCB_SET;
        if(storeCouchbaseDocumentByKeyRequestFromWt->HasCasInput)
        {
            cmd.v.v0.cas = storeCouchbaseDocumentByKeyRequestFromWt->CasInput;
        }
    }
    lcb_error_t err = lcb_store(m_Couchbase, storeCouchbaseDocumentByKeyRequestFromWt, 1, cmds);
    if(err != LCB_SUCCESS)
    {
        cerr << "Failed to set up store request: " << lcb_strerror(m_Couchbase, err) << endl;
        //TODOreq: handle failed store request
        return;
    }
}
void AnonymousBitcoinComputingCouchbaseDB::eventSlotForWtGet()
{
    if(m_NoMoreAllowedMuahahaha) //damn double negatives xD
    {
        return;
        //TODOreq: post() a response at least, BUT really that doesn't matter too much because i mean the server's about to go down regardless... (unless i'm taking db down ONLY and wt is staying up (lol wut?))
    }

    //TODOreq: 'delete' for various error exits (we're using it as cookie :-P), including proper/normal place
    GetCouchbaseDocumentByKeyRequest *getCouchbaseDocumentByKeyRequestFromWt = new GetCouchbaseDocumentByKeyRequest();

    {
        std::istringstream getCouchbaseDocumentByKeyRequestFromWtSerialized((const char*)m_GetMessageQueuesCurrentMessageBuffer);
        boost::archive::text_iarchive deSerializer(getCouchbaseDocumentByKeyRequestFromWtSerialized);
        deSerializer >> *getCouchbaseDocumentByKeyRequestFromWt;
    }

    if(getCouchbaseDocumentByKeyRequestFromWt->GetAndSubscribe)
    {
        //thought about just querying the key [against a list] to see if it's 'get and subscribe' mode, but using a bool like this makes it scale betterer in the futuerer

        GetAndSubscribeCacheItem *keyValueCacheItem;
        try
        {
            //TODOreq: if we've just scheduled our 100ms poll to couchbase and are expecting a fresh value soon, we should just add ourself to the subscribers list. we could respond with the probably-about-to-be-made-stale one, but we shouldn't
            //TODOreq: if the 100ms poll doesn't return a 'new' doc (based on cas), there's still going to be a list (sometimes of size zero) of subscribers who are interested in being given _A_ result. The directly above TODOreq mentions a kind that would be interested, and there is also the "many successive get-and-subscribes" in a row really fast, where all but the first simply added themselves to said list (that actually implies that there was no cache hit, but eh just like above TODOreq above says: if one is scheduled, we wait for it!)
            //^^So I'm thinking something like m_ListOfSubscribersWhoOnlyWant_NEW_Updates and m_ListOfSubscribersWhoDontHaveShitYet. Upon the 100ms poll getting a value (any value), it gives it's result to m_ListOfSubscribersWhoDontHaveShitYet, then checks to see if the cas has changed and then if it has gives it's result to m_ListOfSubscribersWhoOnlyWant_NEW_Updates. Then it adds-while-clearing m_ListOfSubscribersWhoDontHaveShitYet _into_ m_ListOfSubscribersWhoOnlyWant_NEW_Updates
            //^^^The point is, if some bool 'm_PollIsActive' (for that particular key), then we don't want to do the 'respondWithCAS' a few lines down

            //TODOreq: for the actual poll / cache miss, I need to use ONE OF the GetCouchbaseDocumentByKeyRequests (subscribers) to do the get itself using existing infrastructure. It will be easy to "pull back out" (as being GetAndSubscribe 'mode') because of the same bool I used to get to here. BUT doing so is a bit hacky and does lead to a tiny race condition that when happened would make the 100ms now 200ms (or possibly longer, but it gets rarer and rarer). If the end-user for the corresponding GetCouchbaseDocumentByKeyRequest should UNSUBSCRIBE (navigate away, disconnect) while the 'get poll' is taking place, we might not see him in the list anymore and might... idk... disregard the result? Hard to wrap my head around but now that I've typed this shit, I'll double check that I accounted for it later. Might not matter at all depending how I code it (lookup by KEY = doesn't matter)

            //I have written previously that the polling will be lazy. I said it should do a get if 'lastGet' happened >= 100ms ago. I have changed my mind BECAUSE: you could have shit tons of 'subscribers' sitting waiting for their "Post" updates only... and very little of them doing "first page load 'get'ing" (and only the first page load get'ing would go through the code path to see if >= 100ms has elapsed). Since the primary way of pushing updates is to them (masses) via Post without them requiring to do a 'get' (hence the name 'SUBSCRIBE'), I'm going to use an actual timer for polling and not lazy/event-driven polling like I said earlier.

            //try to get
            keyValueCacheItem = m_GetAndSubscribeCacheHash.at(getCouchbaseDocumentByKeyRequestFromWt->CouchbaseGetKeyInput);

            //CACHE HIT if we get this far and the exception isn't thrown
            //TO DOnereq: the 2nd user to subscribe might get to this code path before the very first 'get' has populated document/cas... so... wtf?
            if(!keyValueCacheItem->CurrentlyFetchingPossiblyNew)
            {
                GetCouchbaseDocumentByKeyRequest::respondWithCAS(getCouchbaseDocumentByKeyRequestFromWt, keyValueCacheItem->Document, keyValueCacheItem->DocumentCAS, true, false);
                keyValueCacheItem->Subscribers[getCouchbaseDocumentByKeyRequestFromWt->AnonymousBitcoinComputingWtGUIPointerForCallback] = getCouchbaseDocumentByKeyRequestFromWt;
            }
            else
            {
                //we'll wait for the one that's about to come (especially since there might not even be ONE yet)
                keyValueCacheItem->NewSubscribers[getCouchbaseDocumentByKeyRequestFromWt->AnonymousBitcoinComputingWtGUIPointerForCallback] = getCouchbaseDocumentByKeyRequestFromWt;
            }

            //--m_PendingGetCount; //TODOreq: ???

            //whether or not we're already subscribed, looking up takes just as long as inserting [again possibly], so we just insert. In fact inserting is faster if the key already exists, because then there's just one lookup


            return; //we return because the key was in the cache (even if the doc isn't). we added ourself to the subscribers list, and the timer is already active so we'll get all future updates :)
        }
        catch(std::out_of_range &keyNotInCacheException)
        {
            //CACHE MISS

            //add ourself and if get isn't already scheduled (we're the first), schedule it. If we get here then we might be the first, but we might also be very close after the first so that the very first get (which is already schedule by the first) hasn't finished yet. Once the first POLL is completed, we should never get here
            //^^NOPE, that is subscriber cache, this miss is for just the key itself (first miss creates it)

            //create cache item, even though we have nothing to put in it yet. TODOreq: we need to make sure that we wait for the first get to finish before we ever give someone the "Document" from the cache item (BECAUSE IT IS EMPTY)
            GetAndSubscribeCacheItem *newKeyValueCacheItem = new GetAndSubscribeCacheItem();

            //if we get here, we definitely need to do the first 'get' (when it returns, it sets up the 100ms poller (TODOreq: unless all users have unsubscribed by then xD))

            m_GetAndSubscribeCacheHash[getCouchbaseDocumentByKeyRequestFromWt->CouchbaseGetKeyInput] = newKeyValueCacheItem;
            keyValueCacheItem = newKeyValueCacheItem;
            keyValueCacheItem->NewSubscribers[getCouchbaseDocumentByKeyRequestFromWt->AnonymousBitcoinComputingWtGUIPointerForCallback] = getCouchbaseDocumentByKeyRequestFromWt;

            //we don't return, because we want to actually do the get!
        }
        //TODOreq: we need to 'return' at various places so as not to continue getting. we also need to NOT return at a certain code path too
    }

    //TODOreq: proper error handling inside this macro there is a if("error  != LCB_SUCCESS") section (how da fuq do you put comments in macros?!!?)
    DO_SCHEDULE_COUCHBASE_GET()
}
//struct event *AnonymousBitcoinComputingCouchbaseDB::getAddEventCallbackForWt0()
//{
//    return m_AddEventCallbackForWt0;
//}
BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES, COUCHBASE_LIBEVENTS_GETTER_MEMBER_DEFINITIONS_MACRO, Add)
BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES, COUCHBASE_LIBEVENTS_GETTER_MEMBER_DEFINITIONS_MACRO, Get)

//void AnonymousBitcoinComputingCouchbaseDB::eventSlotForWtAdd0Static(evutil_socket_t unusedSocket, short events, void *userData)
//{
//    (void)unusedSocket;
//    (void)events;
//    static_cast<AnonymousBitcoinComputingCouchbaseDB*>(userData)->eventSlotAdd0();
//}
BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES, COUCHBASE_LIBEVENTS_SLOT_STATIC_METHOD_DEFINITIONS_MACRO, Add)
BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES, COUCHBASE_LIBEVENTS_SLOT_STATIC_METHOD_DEFINITIONS_MACRO, Get)

//void AnonymousBitcoinComputingCouchbaseDB::eventSlotForWtAdd0()
//{
//    unsigned int priority;
//    message_queue::size_type actualMessageSize;
//    m_AddWtMessageQueue0->receive(m_AddMessageQueuesCurrentMessageBuffer,(message_queue::size_type)HACK_WT_TO_COUCHBASE_MAX_MESSAGE_SIZE_Add, actualMessageSize, priority);
//    eventSlotForWtAdd();
//}
BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES, COUCHBASE_LIBEVENTS_SLOT_METHOD_DEFINITIONS_MACRO, Add)
BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES, COUCHBASE_LIBEVENTS_SLOT_METHOD_DEFINITIONS_MACRO, Get)
