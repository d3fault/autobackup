#include "anonymousbitcoincomputingcouchbasedb.h"

#include "../addcouchbasedocumentbykeyrequest.h"
#include "../getcouchbasedocumentbykeyrequest.h"

using namespace std;

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

    //couchbase callbacks
    lcb_set_error_callback(m_Couchbase, AnonymousBitcoinComputingCouchbaseDB::errorCallbackStatic);
    lcb_set_configuration_callback(m_Couchbase, AnonymousBitcoinComputingCouchbaseDB::configurationCallbackStatic);
    lcb_set_get_callback(m_Couchbase, AnonymousBitcoinComputingCouchbaseDB::getCallbackStatic);
    lcb_set_store_callback(m_Couchbase, AnonymousBitcoinComputingCouchbaseDB::storeCallbackStatic);
    lcb_set_durability_callback(m_Couchbase, AnonymousBitcoinComputingCouchbaseDB::durabilityCallbackStatic);
    if((error = lcb_connect(m_Couchbase)) != LCB_SUCCESS)
    {
        cerr << "Failed to connect libcouchbase instance: " << lcb_strerror(m_Couchbase, error) << endl;
        TELL_MAIN_THREAD_THAT_COUCHBASE_FAILED_MACRO
        lcb_destroy(m_Couchbase);
        return;
    }

    lcb_set_cookie(m_Couchbase, this);

    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES, SETUP_WT_TO_COUCHBASE_CALLBACKS_VIA_EVENT_NEW_MACRO, Add)
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES, SETUP_WT_TO_COUCHBASE_CALLBACKS_VIA_EVENT_NEW_MACRO, Get)

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
    ((AnonymousBitcoinComputingCouchbaseDB*)(lcb_get_cookie(instance)))->errorCallback(error, errinfo);
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
    ((AnonymousBitcoinComputingCouchbaseDB*)(lcb_get_cookie(instance)))->configurationCallback(config);
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
    ((AnonymousBitcoinComputingCouchbaseDB*)(lcb_get_cookie(instance)))->storeCallback(cookie, operation, error, resp);
}
void AnonymousBitcoinComputingCouchbaseDB::storeCallback(const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp)
{
    (void)operation; //TODOoptional: i was thinking of using a flag in my "from wt reqeust object (cookie here)" and to NOT do durability when it's a simple "STORE" (because STOREs _TEND_ to be not as important as "ADD" (though that certainly isn't ALWAYS true~)). the purpose of this comment is to point out that we can see if it's a STORE/ADD right here in the 'operation' variable :-P
    if(error != LCB_SUCCESS)
    {
        if(error != LCB_KEY_EEXISTS)
        {
            cerr << "Failed to store key: " << lcb_strerror(m_Couchbase, error) << endl;
            //TODOreq: handle failed store (including decrementing 'pendingAddCount' if appropriate). we should err ehh uhm...
            return;
        }

        //if LCB_ADD'ing, the key already exists. If LCB_SET, then your cas (which is implied otherwise we can't get this error) didn't match when setting (someone modified since you did the get)

        //TODOreq: error handling, except this is a VERY common error (for both add and set) so high as fuck priority. still KISS and doing all "success" cases first...
        //example LCB_ADD fails: 1) buy ad slot [with filler] -- the core operation of abc. 2) username already exists
        //example LCB_SET fails: lock-account-before-buying-ad-slot[with-filler]-just-after-verifying-users-balance-is-high-enough

        cerr << "lcb_key_exists handling not implemented yet: " << lcb_strerror(m_Couchbase, error) << endl;

        //we'll probably want to 'return;' here, doesn't make sense to do the durability stuff...
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

    if(static_cast<const AddCouchbaseDocumentByKeyRequest*>(cookie)->HasCAS) //TODOoptimization i am dumb i think the reason my static_casting didn't work earlier was because i didn't use const keyword. change the ugly casts to these kinds in other occurances
    {
        //LCB_SET implied
        lcbDurabilityCommand.v.v0.cas = resp->v.v0.cas; //TODOreq this is the cas of the new doc? relevant? i think yes...
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
    ((AnonymousBitcoinComputingCouchbaseDB*)(lcb_get_cookie(instance)))->getCallback(cookie, error, resp);
}
void AnonymousBitcoinComputingCouchbaseDB::getCallback(const void *cookie, lcb_error_t error, const lcb_get_resp_t *resp)
{
    GetCouchbaseDocumentByKeyRequest *request = (GetCouchbaseDocumentByKeyRequest*)cookie;

    if(error != LCB_SUCCESS)
    {
        cerr << "Error couchbase getCallback:" << lcb_strerror(m_Couchbase, error) << endl;
        //TODOreq: you know the drill~
    }

    //TODOreq: who takes ownership of resp and is responsible for deleting it? me or couchbase? when does that occur (since I'm now looking into 'hold onto it because Wt might CAS-swap it')? This memory consideration also applies to all the rest of my couchbase callbacks I'd imagine (but valgrind hasn't complained about them... so idfk. I think since I'm not deleting them, it's safe to assume(xD) that they're only valid during the duration of this callback)
    if(request->SaveCAS) //TODOoptimization: __unlikely or whatever i can find it boost
    {
        GetCouchbaseDocumentByKeyRequest::respondWithCAS(request, resp->v.v0.bytes, resp->v.v0.nbytes, resp->v.v0.cas);
    }
    else
    {
        GetCouchbaseDocumentByKeyRequest::respond(request, resp->v.v0.bytes, resp->v.v0.nbytes);
    }
    delete request;

    //TODOreq: error cases need to account for these pending add/get counts
    --m_PendingGetCount;
    if(m_NoMoreAllowedMuahahaha && m_PendingGetCount == 0 && m_PendingAddCount == 0)
    {
        notifyMainThreadWeAreFinishedWithAllPendingRequests();
    }
}
void AnonymousBitcoinComputingCouchbaseDB::durabilityCallbackStatic(lcb_t instance, const void *cookie, lcb_error_t error, const lcb_durability_resp_t *resp)
{
    ((AnonymousBitcoinComputingCouchbaseDB*)(lcb_get_cookie(instance)))->durabilityCallback(cookie, error, resp);
}
void AnonymousBitcoinComputingCouchbaseDB::durabilityCallback(const void *cookie, lcb_error_t error, const lcb_durability_resp_t *resp)
{
    if(error != LCB_SUCCESS)
    {
        cerr << "Error: Generic lcb_durability_poll callback failure: " << lcb_strerror(m_Couchbase, error) << endl;
        //TODOreq: handle errors
        return;
    }
    if(resp->v.v0.err != LCB_SUCCESS)
    {
        cerr << "Error: Specific lcb_durability_poll callback failure: " << lcb_strerror(m_Couchbase, resp->v.v0.err) << endl;
        //TODOreq: handle errors
        return;
    }

    AddCouchbaseDocumentByKeyRequest *request = (AddCouchbaseDocumentByKeyRequest*)cookie;
    AddCouchbaseDocumentByKeyRequest::respond(request);
    delete request;

    --m_PendingAddCount;
    if(m_NoMoreAllowedMuahahaha && m_PendingAddCount == 0 && m_PendingGetCount == 0)
    {
        notifyMainThreadWeAreFinishedWithAllPendingRequests();
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

    //maybe we got lucky
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
    AddCouchbaseDocumentByKeyRequest *storeCouchbaseDocumentByKeyRequestFromWt = new AddCouchbaseDocumentByKeyRequest();

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
    if(storeCouchbaseDocumentByKeyRequestFromWt->LcbModeIsAdd)
    {
        cmd.v.v0.operation = LCB_ADD;
    }
    else
    {
        cmd.v.v0.operation = LCB_SET;
        if(storeCouchbaseDocumentByKeyRequestFromWt->HasCAS)
        {
            cmd.v.v0.cas = storeCouchbaseDocumentByKeyRequestFromWt->CAS;
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
    ++m_PendingGetCount; //TODOreq: overflow? meh not worried about it for now

    //TODOreq: 'delete' for various error exits (we're using it as cookie :-P), including proper/normal place
    GetCouchbaseDocumentByKeyRequest *getCouchbaseDocumentByKeyRequestFromWt = new GetCouchbaseDocumentByKeyRequest();

    {
        std::istringstream getCouchbaseDocumentByKeyRequestFromWtSerialized((const char*)m_GetMessageQueuesCurrentMessageBuffer);
        boost::archive::text_iarchive deSerializer(getCouchbaseDocumentByKeyRequestFromWtSerialized);
        deSerializer >> *getCouchbaseDocumentByKeyRequestFromWt;
    }

    lcb_get_cmd_t cmd;
    const lcb_get_cmd_t *cmds[1];
    cmds[0] = &cmd;

    const char *keyInput = getCouchbaseDocumentByKeyRequestFromWt->CouchbaseGetKeyInput.c_str();

    memset(&cmd, 0, sizeof(cmd));
    cmd.v.v0.key = keyInput;
    cmd.v.v0.nkey = strlen(keyInput);
    lcb_error_t error = lcb_get(m_Couchbase, getCouchbaseDocumentByKeyRequestFromWt, 1, cmds);
    if(error  != LCB_SUCCESS)
    {
        cerr << "Failed to setup get request: " << lcb_strerror(m_Couchbase, error) << endl;
        //TODOreq: proper error handling
    }
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
