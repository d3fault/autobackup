#include <stdio.h>

#include <Wt/WApplication>
#include <Wt/WServer>

#include <boost/preprocessor/repeat.hpp>
#include <boost/thread.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/function.hpp>

#ifdef __cplusplus
extern "C" {
#endif
#include <libcouchbase/couchbase.h>
#include <event2/event.h>
#include <event2/thread.h>
#ifdef __cplusplus
} // extern "C"
#endif

#include "setvaluebykeyrequestfromwt.h"
#include "wtkeysetandgetwidget.h"

using namespace Wt;
using namespace boost::interprocess;

#define NUMBER_OF_MUTEXES_AND_MESSAGE_QUEUES_SHOULD_BE_ROUGHLY_EQUAL_TO_WTS_THREAD_POOL_MAX_THREADS 10 //if change here, change in WtKeySetAndGetWidget header as well
#define SET_VALUE_BY_KEY_REQUEST_FROM_WT_MESSAGE_QUEUE_BASE_NAME WtCouchbaseKeySetAndGetTestMessageQueueKeys
#define SET_VALUE_BY_KEY_REQUEST_FROM_WT_MESSAGE_QUEUE_MAX_MESSAGE_SIZE 102400

boost::condition_variable couchbaseIsConnectedWaitCondition;
boost::mutex couchbaseIsConnectedMutex;
bool couchbaseDoneInitializing = false; //whether connection is successful or not, it is done trying
//^I recall a lecture by Thiago on the qt mailing lists after I told someone that declaring a bool volatile makes it suitable as a cross-thread flag (I think there was still a mutex protecting it, unsure)... and after assloads of low level speak the gist I got from it was "no volatile sucks you should always use atomics etc". The couchbaseDoneInitializing bool isn't even declared volatile and is still used across threads as flagging. So WTF? On one hand it says in the Boost docs to do it that way (and indeed it works (but "it working" is actually a very shit argument in the realm of [proper] programming)) and Boost is a very credible source. On the other, Thiago's a fucking genius too and knows his shit. So I either misunderstood Thiago's lecture or Boost is doing it wrong :-/. tl;dr: dgaf for now (it's only used during startup so... fuck it...)
bool couchbaseIsConnected = false;
bool couchbaseThreadExittedCleanly = false;

//core cross-thread (Wt -> Couchbase) events. Wt <- Couchbase uses WServer::post
message_queue *setValueByKeyRequestFromWtMessageQueues[NUMBER_OF_MUTEXES_AND_MESSAGE_QUEUES_SHOULD_BE_ROUGHLY_EQUAL_TO_WTS_THREAD_POOL_MAX_THREADS];
void *setValueByKeyRequestFromWtMessageQueueCurrentMessageBuffers[NUMBER_OF_MUTEXES_AND_MESSAGE_QUEUES_SHOULD_BE_ROUGHLY_EQUAL_TO_WTS_THREAD_POOL_MAX_THREADS];
struct event *setValueByKeyMessageQueueEvents[NUMBER_OF_MUTEXES_AND_MESSAGE_QUEUES_SHOULD_BE_ROUGHLY_EQUAL_TO_WTS_THREAD_POOL_MAX_THREADS];
struct event *triggerCouchbaseThreadStopEvent;

#define COUCHBASE_FAILED_INIT_MACRO \
{ \
    boost::lock_guard<boost::mutex> couchbaseConnectingLock(couchbaseIsConnectedMutex); \
    couchbaseDoneInitializing = true; \
} \
couchbaseIsConnectedWaitCondition.notify_one();

//original version with comments down below
#define setValueByKeyRequestFromWtEventSlotMacro(z, n, text) \
static void setValueByKeyRequestFromWtEventSlot##n(evutil_socket_t unusedSocket, short events, void *userData) \
{ \
    (void)unusedSocket; \
    (void)events; \
    unsigned int priority; \
    message_queue::size_type actualMessageSize; \
    setValueByKeyRequestFromWtMessageQueues[n]->receive(setValueByKeyRequestFromWtMessageQueueCurrentMessageBuffers[n],(message_queue::size_type)SET_VALUE_BY_KEY_REQUEST_FROM_WT_MESSAGE_QUEUE_MAX_MESSAGE_SIZE, actualMessageSize, priority); \
    \
    SetValueByKeyRequestFromWt *setValueByKeyRequestFromWt = new SetValueByKeyRequestFromWt(); \
    { \
        std::istringstream setValueByKeyRequestFromWtSerialized((const char*)setValueByKeyRequestFromWtMessageQueueCurrentMessageBuffers[n]); \
        boost::archive::text_iarchive deSerializer(setValueByKeyRequestFromWtSerialized); \
        deSerializer >> *setValueByKeyRequestFromWt; \
    } \
    fprintf(stdout, "Trying to set a key...\n"); \
    lcb_t *couchbaseInstance = (lcb_t*)userData; \
    lcb_store_cmd_t cmd; \
    const lcb_store_cmd_t *cmds[1]; \
    cmds[0] = &cmd; \
    memset(&cmd, 0, sizeof(cmd)); \
    cmd.v.v0.key = setValueByKeyRequestFromWt->CouchbaseSetKey.c_str(); \
    cmd.v.v0.nkey = strlen(setValueByKeyRequestFromWt->CouchbaseSetKey.c_str()); \
    cmd.v.v0.bytes = setValueByKeyRequestFromWt->CouchbaseSetValue.c_str(); \
    cmd.v.v0.nbytes = strlen(setValueByKeyRequestFromWt->CouchbaseSetValue.c_str()); \
    cmd.v.v0.operation = LCB_ADD; \
    lcb_error_t err = lcb_store(*couchbaseInstance, setValueByKeyRequestFromWt, 1, cmds); \
    if(err != LCB_SUCCESS) \
    { \
    fprintf(stderr, "Failed to set up store request: %s\n", lcb_strerror(*couchbaseInstance, err)); \
    return; \
    } \
    fprintf(stdout, "It appears we've' sent a key set request\n"); \
}

BOOST_PP_REPEAT(NUMBER_OF_MUTEXES_AND_MESSAGE_QUEUES_SHOULD_BE_ROUGHLY_EQUAL_TO_WTS_THREAD_POOL_MAX_THREADS, setValueByKeyRequestFromWtEventSlotMacro, ~)

#define NEW_AND_OPEN_MESSAGE_QUEUE_CALLER_MACRO(z, n, text) WtKeySetAndGetWidget::newAndOpenSetValueByKeyMessageQueue(n, "SET_VALUE_BY_KEY_REQUEST_FROM_WT_MESSAGE_QUEUE_BASE_NAME" \
#n);
#define REMOVE_ALL_MESSAGE_QUEUES_MACRO(z, n, text) message_queue::remove("SET_VALUE_BY_KEY_REQUEST_FROM_WT_MESSAGE_QUEUE_BASE_NAME" \
#n);
#define CREATE_MY_MESSAGE_QUEUES_MACRO(z, n, text) setValueByKeyRequestFromWtMessageQueues[n] = new message_queue(create_only, "SET_VALUE_BY_KEY_REQUEST_FROM_WT_MESSAGE_QUEUE_BASE_NAME" \
#n, 20000, SET_VALUE_BY_KEY_REQUEST_FROM_WT_MESSAGE_QUEUE_MAX_MESSAGE_SIZE);
//TODOreq, pick new message queue sizes now that we're using "10" of (perhaps factor in the "10" when calculating a size (dynamically, that is))
//2gb ram max, 20k messages @ 100kb each... i wish these was dynamic'er. IF ONLY THERE WAS A GENERATION UTILITY THAT ALLOWED YOU TO SPECIFY VARIOUS MESSAGE TYPES AHEAD OF TIME SO THAT THEY WOULD TAKE UP EXACTLY THE BYTES NEEDED (ok ok, QString dynamically allocates fuggit) AND YOU COULD THEN USE THOSE TO CALL PROCEDURES REMOTELY/INTER-PROCESS'dly
#define DELETE_ALL_NEWD_MESSAGE_QUEUES_MACRO(z, n, text) delete setValueByKeyRequestFromWtMessageQueues[n];

#define MALLOC_SET_VALUE_BY_KEY_REQUEST_FROM_WT_MESSAGE_QUEUE_CURRENT_MESSAGE_BUFFERS(z, n, text) setValueByKeyRequestFromWtMessageQueueCurrentMessageBuffers[n] = malloc(SET_VALUE_BY_KEY_REQUEST_FROM_WT_MESSAGE_QUEUE_MAX_MESSAGE_SIZE);

#define EVENT_NEW_MACRO_USING_MACRO_CREATED_EVENT_CALLBACKS_WOOT(z, n, text) setValueByKeyMessageQueueEvents[n] = event_new(eventBase, -1, EV_READ, setValueByKeyRequestFromWtEventSlot##n, &couchbaseInstance);

#define FREE_SET_VALUE_BY_KEY_REQUEST_FROM_WT_MESSAGE_QUEUE_CURRENT_MESSAGE_BUFFERS(z, n, text) free(setValueByKeyRequestFromWtMessageQueueCurrentMessageBuffers[n]);

#define TELL_WT_KEY_SET_AND_GET_WIDGET_TO_DELETE_MESSAGE_QUEUES_MACRO(z, n, text) WtKeySetAndGetWidget::deleteSetValueByKeyMessageQueue(n);


//TODOreq: come to think of it, I think my fprintf calls from various threads are unsafe (maybe not?)... but fuck it anyways this is just testing n shit...

static void couchbaseErrorCallback(lcb_t instance, lcb_error_t error, const char *errinfo)
{
    fprintf(stderr, "ERROR: %s %s\n", lcb_strerror(instance, error), errinfo);

    if(!couchbaseIsConnected) //mutex protect? methinks not... plus would be costly since regular "timeouts" would now need to lock/unlock that lol
    {
        COUCHBASE_FAILED_INIT_MACRO
    }

    //TODOreq: failed connections is handled above, but we need to handle all relevant types/places (exit() doesn't cut it)
    //exit(EXIT_FAILURE);
}
static void couchbaseConfigurationCallback(lcb_t instance, lcb_configuration_t config)
{
    (void)instance;
    if(config == LCB_CONFIGURATION_NEW)
    {
        //tell main thread that we're now ready to receive events
        {
            boost::lock_guard<boost::mutex> couchbaseConnectingLock(couchbaseIsConnectedMutex);
            couchbaseDoneInitializing = true;
            couchbaseIsConnected = true;

            //clean-up previous crashed exits
            BOOST_PP_REPEAT(NUMBER_OF_MUTEXES_AND_MESSAGE_QUEUES_SHOULD_BE_ROUGHLY_EQUAL_TO_WTS_THREAD_POOL_MAX_THREADS, REMOVE_ALL_MESSAGE_QUEUES_MACRO, ~)

            //TODOreq: setValueByKeyMessageQueue and currentValueByKeyMessageBuffer need to be delete'd/free'd at various error cases

            BOOST_PP_REPEAT(NUMBER_OF_MUTEXES_AND_MESSAGE_QUEUES_SHOULD_BE_ROUGHLY_EQUAL_TO_WTS_THREAD_POOL_MAX_THREADS, CREATE_MY_MESSAGE_QUEUES_MACRO, ~)

            BOOST_PP_REPEAT(NUMBER_OF_MUTEXES_AND_MESSAGE_QUEUES_SHOULD_BE_ROUGHLY_EQUAL_TO_WTS_THREAD_POOL_MAX_THREADS, MALLOC_SET_VALUE_BY_KEY_REQUEST_FROM_WT_MESSAGE_QUEUE_CURRENT_MESSAGE_BUFFERS, ~)
        }
        couchbaseIsConnectedWaitCondition.notify_one();
    }
}

//orig, now macro'fied and de-commented above
#if 0
static void setValueByKeyRequestFromWtEventSlot(evutil_socket_t unusedSocket, short events, void *userData)
{
    (void)unusedSocket;
    (void)events; //oh that's not an int! still could have used it xD... still can.... lockfree::queue doesn't sound portable and there was even a statement saying it completely fails at some point. but eh i bet for the most part it's fine. STILL, i wouldn't be surprised if libevent is faster. Hell tbh I haven't a clue which is faster... maybe they both use atomics? I'd need to time them and anything else before that is just blind guessing gg. But also if they are both atomics/equal, then libevent has the advantage of it "already needs to be triggered". Still I'm pretty damn sure this is C10k winrar already so fuck it premature optmization is bleh
    //TODOoptimization: i could read all of the messages off of the queue here (and combine them all into one couchbase command (unless i can't give each command it's own cookie, but i think i can)) and then just ignore when it's empty [because i read them 'earlier' than usual]. Ideally it would be good to not "event_active" when I'm able to do that optimization, but eh I don't know the internals of libevent on how to cancel events about to happen lol fuck it
    //^^^NVM FUCK IT, ONE COOKIE PER "lcb_store" xD (unless i made the cookie itself able to do "multiples" (possible but worth it?))

    unsigned int priority;
    message_queue::size_type actualMessageSize;
    setValueByKeyRequestFromWtMessageQueue->receive(setValueByKeyRequestFromWtMessageQueueCurrentMessageBuffers, (message_queue::size_type)SET_VALUE_BY_KEY_REQUEST_FROM_WT_MESSAGE_QUEUE_MAX_MESSAGE_SIZE, actualMessageSize, priority);

    //TODOreq: 'delete' for various error exits (we're using it as cookie :-P). Already deleting at proper normal use place
    SetValueByKeyRequestFromWt *setValueByKeyRequestFromWt = new SetValueByKeyRequestFromWt();

    {
        std::istringstream setValueByKeyRequestFromWtSerialized((const char*)setValueByKeyRequestFromWtMessageQueueCurrentMessageBuffers);
        boost::archive::text_iarchive deSerializer(setValueByKeyRequestFromWtSerialized);
        deSerializer >> *setValueByKeyRequestFromWt;
    }

    fprintf(stdout, "Trying to set a key...\n");
    lcb_t *couchbaseInstance = (lcb_t*)userData;
    lcb_store_cmd_t cmd;
    const lcb_store_cmd_t *cmds[1];
    cmds[0] = &cmd;
    memset(&cmd, 0, sizeof(cmd));
    cmd.v.v0.key = setValueByKeyRequestFromWt->CouchbaseSetKey.c_str(); //TODOoptimization: maybe serialize into c_str to minimize conversions? idk if boost.serialization can do it, but meh i'd guess yes it can
    cmd.v.v0.nkey = strlen(setValueByKeyRequestFromWt->CouchbaseSetKey.c_str());
    cmd.v.v0.bytes = setValueByKeyRequestFromWt->CouchbaseSetValue.c_str();
    cmd.v.v0.nbytes = strlen(setValueByKeyRequestFromWt->CouchbaseSetValue.c_str());
    cmd.v.v0.operation = LCB_ADD;
    lcb_error_t err = lcb_store(*couchbaseInstance, setValueByKeyRequestFromWt, 1, cmds);
    if(err != LCB_SUCCESS)
    {
        fprintf(stderr, "Failed to set up store request: %s\n", lcb_strerror(*couchbaseInstance, err));

        //TODOreq: handle failed store request. we should err ehh uhm...
        //exit(EXIT_FAILURE);
        return;
    }
    fprintf(stdout, "It appears we've' sent a key set request\n");
}
#endif
static void triggerCouchbaseThreadStopCallback(evutil_socket_t unusedSocket, short events, void *userData)
{
    (void)unusedSocket;
    (void)events;
    event_base_loopbreak((struct event_base *)userData);
}
static void couchbaseStoreCallback(lcb_t instance, const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp)
{
    (void)cookie;
    (void)operation;
    (void)resp;
    if(error != LCB_SUCCESS)
    {
        fprintf(stderr, "Failed to store key: %s\n", lcb_strerror(instance, error));

        //TODOreq: handle failed store. we should err ehh uhm...
        //exit(EXIT_FAILURE);
        return;
    }
    fprintf(stdout, "Key has been stored on master, now calling lcb_durability_poll\n");
    lcb_durability_opts_t lcbDurabilityOptions;
    memset(&lcbDurabilityOptions, 0, sizeof(lcb_durability_opts_t));
    lcbDurabilityOptions.v.v0.replicate_to = 2;
    lcbDurabilityOptions.v.v0.timeout = 3000000; //3 second timeout is PLENTY, and/but we should probably change this in the place it gets its default if we leave it to zero [just as a TODOoptimization to lessen memory writing]
    lcb_durability_cmd_t lcbDurabilityCommand;
    memset(&lcbDurabilityCommand, 0, sizeof(lcb_durability_cmd_t));
    lcbDurabilityCommand.v.v0.key = resp->v.v0.key;
    lcbDurabilityCommand.v.v0.nkey = resp->v.v0.nkey;
    //in my testing, setting lcb->cas to resp->cas didn't work as expected... BUT i don't think it matters at all for "LCB_ADD" operations...
    lcb_durability_cmd_t *lcbDurabilityCommandList[1];
    lcbDurabilityCommandList[0] = &lcbDurabilityCommand;
    error = lcb_durability_poll(instance, cookie, &lcbDurabilityOptions, 1, lcbDurabilityCommandList);
    if(error != LCB_SUCCESS)
    {
        fprintf(stderr, "Failed to schedule lcb_durability_poll: %s\n", lcb_strerror(instance, error));
        //TODOreq: proper error handling bah
    }
}
static void couchbaseDurabilityCallback(lcb_t instance, const void *cookie, lcb_error_t error, const lcb_durability_resp_t *resp)
{
    if(error != LCB_SUCCESS)
    {
        fprintf(stderr, "Error: Generic lcb_durability_poll callback failure: %s\n", lcb_strerror(instance, error));
        //TODOreq: handle errors
        return;
    }
    if(resp->v.v0.err != LCB_SUCCESS)
    {
        fprintf(stderr, "Error: Specific lcb_durability_poll callback failure: %s\n", lcb_strerror(instance, resp->v.v0.err));
        //TODOreq: handle errors
        return;
    }
    printf("lcb_durability_callback success\n");
    //tempted to "check" npersisted >= 2 etc, but eh fuck it trust couchbase
    SetValueByKeyRequestFromWt *setValueByKeyRequestFromWt = (SetValueByKeyRequestFromWt*)cookie;
    //we could have gotten the key (and maybe value, idk) from the resp instead

    Wt::WServer::instance()->post(setValueByKeyRequestFromWt->WtSessionId, boost::bind(boost::bind(&WtKeySetAndGetWidget::valueSetByKeyCallback, setValueByKeyRequestFromWt->WtKeySetAndGetWidgetPointerForCallback, _1, _2), setValueByKeyRequestFromWt->CouchbaseSetKey, setValueByKeyRequestFromWt->CouchbaseSetValue));

    delete setValueByKeyRequestFromWt;
}
static void couchbaseThreadEntryPoint()
{
    //libevent
    struct event_base *eventBase = event_base_new();
    if(!eventBase)
    {
        fprintf(stderr, "Error: Libevent failed to initialize\n");
        COUCHBASE_FAILED_INIT_MACRO
        return;
    }
    if(evthread_use_pthreads() < 0) //or evthread_use_windows_threads
    {
        fprintf(stderr, "Error: Failed to evthread_use_pthreads");
        event_base_free(eventBase);
        COUCHBASE_FAILED_INIT_MACRO
        return;
    }
    if(evthread_make_base_notifiable(eventBase) < 0)
    {
        fprintf(stderr, "Error: Failed to make event base notifiable\n");
        event_base_free(eventBase);
        COUCHBASE_FAILED_INIT_MACRO
        return;
    }
    struct lcb_create_io_ops_st couchbaseCreateIoOps;
    lcb_io_opt_t couchbaseIoOps;
    lcb_error_t error;
    memset(&couchbaseCreateIoOps, 0, sizeof(couchbaseCreateIoOps));
    couchbaseCreateIoOps.v.v0.type = LCB_IO_OPS_LIBEVENT;
    couchbaseCreateIoOps.v.v0.cookie = eventBase;
    error = lcb_create_io_ops(&couchbaseIoOps, &couchbaseCreateIoOps);
    if(error != LCB_SUCCESS)
    {
        fprintf(stderr, "Error: Failed to create an IOOPS structure for libevent: %s\n", lcb_strerror(NULL, error));
        event_base_free(eventBase);
        COUCHBASE_FAILED_INIT_MACRO
        return;
    }

    //couchbase
    lcb_t couchbaseInstance;
    struct lcb_create_st copts;
    memset(&copts, 0, sizeof(copts));
    copts.v.v0.host = "192.168.56.10:8091";
    copts.v.v0.io = couchbaseIoOps;
    if((error = lcb_create(&couchbaseInstance, &copts)) != LCB_SUCCESS)
    {
        fprintf(stderr, "Failed to create a libcouchbase instance: %s\n", lcb_strerror(NULL, error));
        lcb_destroy_io_ops(couchbaseIoOps);
        event_base_free(eventBase);
        COUCHBASE_FAILED_INIT_MACRO
        return;
    }
    //couchbase callbacks
    lcb_set_error_callback(couchbaseInstance, couchbaseErrorCallback);
    lcb_set_configuration_callback(couchbaseInstance, couchbaseConfigurationCallback);
    //lcb_set_get_callback(instance, couchbaseGetCallback);
    lcb_set_store_callback(couchbaseInstance, couchbaseStoreCallback);
    lcb_set_durability_callback(couchbaseInstance, couchbaseDurabilityCallback);
    if((error = lcb_connect(couchbaseInstance)) != LCB_SUCCESS)
    {
        fprintf(stderr, "Failed to connect libcouchbase instance: %s\n", lcb_strerror(NULL, error));
        lcb_destroy(couchbaseInstance);
        lcb_destroy_io_ops(couchbaseIoOps);
        event_base_free(eventBase);
        COUCHBASE_FAILED_INIT_MACRO
        return;
    }

    lcb_set_cookie(couchbaseInstance, eventBase);

    BOOST_PP_REPEAT(NUMBER_OF_MUTEXES_AND_MESSAGE_QUEUES_SHOULD_BE_ROUGHLY_EQUAL_TO_WTS_THREAD_POOL_MAX_THREADS, EVENT_NEW_MACRO_USING_MACRO_CREATED_EVENT_CALLBACKS_WOOT, ~)

    triggerCouchbaseThreadStopEvent = event_new(eventBase, -1, EV_READ, triggerCouchbaseThreadStopCallback, eventBase);

    event_base_loop(eventBase, 0); //wait until event_base_loopbreak is called, processing all events of course

    //cleanup
    //TODOreq: we could try to receive any left over messages in queue here, or just disregard them and quit...
    BOOST_PP_REPEAT(NUMBER_OF_MUTEXES_AND_MESSAGE_QUEUES_SHOULD_BE_ROUGHLY_EQUAL_TO_WTS_THREAD_POOL_MAX_THREADS, FREE_SET_VALUE_BY_KEY_REQUEST_FROM_WT_MESSAGE_QUEUE_CURRENT_MESSAGE_BUFFERS, ~)
    BOOST_PP_REPEAT(NUMBER_OF_MUTEXES_AND_MESSAGE_QUEUES_SHOULD_BE_ROUGHLY_EQUAL_TO_WTS_THREAD_POOL_MAX_THREADS, REMOVE_ALL_MESSAGE_QUEUES_MACRO, ~)
    BOOST_PP_REPEAT(NUMBER_OF_MUTEXES_AND_MESSAGE_QUEUES_SHOULD_BE_ROUGHLY_EQUAL_TO_WTS_THREAD_POOL_MAX_THREADS, DELETE_ALL_NEWD_MESSAGE_QUEUES_MACRO, ~)
    lcb_destroy(couchbaseInstance);
    if(lcb_destroy_io_ops(couchbaseIoOps) != LCB_SUCCESS)
    {
        fprintf(stderr, "Error: Failed to lcb_destroy_io_ops\n");
        return;
    }

    //First it took me multiple hours to get rid of a valgrind error solved via lcb_destroy_io_ops (couchbase C sdk examples suck), then it took me a few more to realize that lcb_destroy_io_ops is freeing the events and base too, so I was double free'ing. Doh! Finally got valgrind to not give any reports zomgfuckwyreah
    //event_free(triggerCouchbaseThreadStopEvent);
    //event_free(setValueByKeyEvent);
    //event_base_free(eventBase);

    couchbaseThreadExittedCleanly = true;
}
WApplication *handleNewWtConnection(const WEnvironment& env)
{
    return new WtKeySetAndGetWidget(env);
}

//At the very least, this proggy demonstrates why "files/folders" systems suck and why "tagging" is superior (this could go in Prototypes/Wt (well, if that dir even existed lol) just as easily as Prototypes/Couchbase (correct solution is to use tagging system and tag it both Wt and Couchbase) /IRRELEVANT)
int main(int argc, char **argv)
{
    boost::unique_lock<boost::mutex> couchbaseConnectingLock(couchbaseIsConnectedMutex); //originally had this below the thread dispatch, but i think putting it here will evade a race condition where the thread calls notify_one() (errors, or just luck/fast (you never know with threads)) before we are waiting on it (and we therefore miss it !?!?!?)
    boost::thread couchbaseThread(&couchbaseThreadEntryPoint);
    while(!couchbaseDoneInitializing)
    {
        couchbaseIsConnectedWaitCondition.wait(couchbaseConnectingLock);
    }
    if(!couchbaseIsConnected)
        return 1;

    //If we get here, we know the global setValueByKeyEvent has been created, so grab events/event-pointers from couchbase thread (it is idle [since we haven't done anything yet] so this SHOULD be safe) and store them somewhere that wt can see them
    BOOST_PP_REPEAT(NUMBER_OF_MUTEXES_AND_MESSAGE_QUEUES_SHOULD_BE_ROUGHLY_EQUAL_TO_WTS_THREAD_POOL_MAX_THREADS, NEW_AND_OPEN_MESSAGE_QUEUE_CALLER_MACRO, ~)

    for(int i = 0; i < NUMBER_OF_MUTEXES_AND_MESSAGE_QUEUES_SHOULD_BE_ROUGHLY_EQUAL_TO_WTS_THREAD_POOL_MAX_THREADS; ++i)
    {
        WtKeySetAndGetWidget::m_SetValueByKeyRequestFromWtEvent[i] = setValueByKeyMessageQueueEvents[i];
    }

    fprintf(stdout, "Now Starting Wt...\n");
    int ret = WRun(argc, argv, &handleNewWtConnection);

    BOOST_PP_REPEAT(NUMBER_OF_MUTEXES_AND_MESSAGE_QUEUES_SHOULD_BE_ROUGHLY_EQUAL_TO_WTS_THREAD_POOL_MAX_THREADS, TELL_WT_KEY_SET_AND_GET_WIDGET_TO_DELETE_MESSAGE_QUEUES_MACRO, ~)

    //tell the couchbase thread to stop
    //event_base_loopbreak(eventBase);
    //^^event_base_loopbreak needs to be called on the thread where event_base_loop is being run, so we need another "regular" event to call it properly (THANKS VALGRIND <3)
    event_active(triggerCouchbaseThreadStopEvent, EV_READ|EV_WRITE, 0);

    couchbaseThread.join();
    if(!couchbaseThreadExittedCleanly)
        return 1;

    fprintf(stdout, "Now Exitting\n");
    return ret;
}
