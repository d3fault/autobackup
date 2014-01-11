#include <stdio.h>
#include <Wt/WApplication>
#include <boost/thread.hpp>
#include <boost/scope_exit.hpp>

#ifdef __cplusplus
extern "C" {
#endif
#include <libcouchbase/couchbase.h>
#include <event2/event.h>
#include <event2/thread.h>
#ifdef __cplusplus
} // extern "C"
#endif

#include "wtkeysetandgetwidget.h"

using namespace Wt;

boost::condition_variable couchbaseIsConnectedWaitCondition;
boost::mutex couchbaseIsConnectedMutex;
bool couchbaseDoneInitializing = false; //whether connection is successful or not, it is done trying
bool couchbaseIsConnected = false;
bool couchbaseThreadExittedCleanly = false;

//core cross-thread (Wt -> Couchbase, for now) events
struct event *setValueByKeyEvent;
struct event *triggerCouchbaseThreadStopEvent;

#define COUCHBASE_FAILED_INIT_MACRO \
{ \
    boost::lock_guard<boost::mutex> couchbaseConnectingLock(couchbaseIsConnectedMutex); \
    couchbaseDoneInitializing = true; \
} \
couchbaseIsConnectedWaitCondition.notify_one();


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
    if(config == LCB_CONFIGURATION_NEW)
    {
        //tell main thread that we're now ready to receive events
        {
            boost::lock_guard<boost::mutex> couchbaseConnectingLock(couchbaseIsConnectedMutex);
            couchbaseDoneInitializing = true;
            couchbaseIsConnected = true;
        }
        couchbaseIsConnectedWaitCondition.notify_one();
    }
}
static void setValueByKeyCallback(evutil_socket_t wat, short events, void *userData)
{
    fprintf(stdout, "Trying to set a key...\n");
    fprintf(stdout, "Value of 'events' during setValueByKeyCallback: %04x\n", events);
    lcb_t *couchbaseInstance = (lcb_t*)userData;
    lcb_store_cmd_t cmd;
    const lcb_store_cmd_t *cmds[1];
    cmds[0] = &cmd;
    memset(&cmd, 0, sizeof(cmd));
    cmd.v.v0.key = "butt";
    cmd.v.v0.nkey = 4;
    cmd.v.v0.bytes = "sex";
    cmd.v.v0.nbytes = 3;
    cmd.v.v0.operation = LCB_SET;
    lcb_error_t err = lcb_store(*couchbaseInstance, NULL, 1, cmds);
    if(err != LCB_SUCCESS)
    {
        fprintf(stderr, "Failed to set up store request: %s\n", lcb_strerror(*couchbaseInstance, err));

        //TODOreq: handle failed store request. we should err ehh uhm...
        //exit(EXIT_FAILURE);
        return;
    }
    fprintf(stdout, "It appears we've' sent a key set request\n");
}
static void triggerCouchbaseThreadStopCallback(evutil_socket_t wat, short events, void *userData)
{
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
    fprintf(stdout, "Key has been stored");
}
void couchbaseThreadEntryPoint()
{
    //libevent
    struct event_base *eventBase = event_base_new();
    if(!eventBase)
    {
        fprintf(stderr, "Error: Libevent failed to initialize\n");
        COUCHBASE_FAILED_INIT_MACRO
        return;
    }
    evthread_use_pthreads(); //or evthread_use_windows_threads
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
        event_base_free(eventBase);
        COUCHBASE_FAILED_INIT_MACRO
        return;
    }
    //couchbase callbacks
    lcb_set_error_callback(couchbaseInstance, couchbaseErrorCallback);
    lcb_set_configuration_callback(couchbaseInstance, couchbaseConfigurationCallback);
    //lcb_set_get_callback(instance, couchbaseGetCallback);
    lcb_set_store_callback(couchbaseInstance, couchbaseStoreCallback);
    if((error = lcb_connect(couchbaseInstance)) != LCB_SUCCESS)
    {
        fprintf(stderr, "Failed to connect libcouchbase instance: %s\n", lcb_strerror(NULL, error));
        lcb_destroy(couchbaseInstance);
        event_base_free(eventBase);
        COUCHBASE_FAILED_INIT_MACRO
        return;
    }

    lcb_set_cookie(couchbaseInstance, eventBase);
    setValueByKeyEvent = event_new(eventBase, -1, EV_READ, setValueByKeyCallback, &couchbaseInstance);
    triggerCouchbaseThreadStopEvent = event_new(eventBase, -1, EV_READ, triggerCouchbaseThreadStopCallback, eventBase);

    event_base_loop(eventBase, 0); //wait until event_base_loopbreak is called, processing all events of course

    //cleanup
    lcb_destroy(couchbaseInstance);
    event_free(triggerCouchbaseThreadStopEvent);
    event_free(setValueByKeyEvent);
    event_base_free(eventBase);
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
    WtKeySetAndGetWidget::m_SetValueByKeyEvent = setValueByKeyEvent; //dirty hack? fuggit, i'll just be happy if this works...

    fprintf(stdout, "Now Starting Wt...\n");
    int ret = WRun(argc, argv, &handleNewWtConnection);

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
