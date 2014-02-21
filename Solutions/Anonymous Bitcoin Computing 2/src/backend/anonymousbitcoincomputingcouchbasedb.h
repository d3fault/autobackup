#ifndef ANONYMOUSBITCOINCOMPUTINGCOUCHBASEDB_H
#define ANONYMOUSBITCOINCOMPUTINGCOUCHBASEDB_H

#include <libcouchbase/couchbase.h>
#include <event2/event.h>
#include <event2/thread.h>

#include <boost/preprocessor/repeat.hpp>
#include <boost/thread.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/function.hpp>
#include <boost/unordered_map.hpp>
#include <boost/foreach.hpp>

#include "getandsubscribecacheitem.h"

using namespace boost::interprocess;

typedef boost::unordered_map<std::string /*key*/, GetAndSubscribeCacheItem* /*doc,cas,listOfSubscribers*/> GetAndSubscribeCacheHashType;

/////////////////////////////////////////////////////BEGIN MACRO HELL///////////////////////////////////////////////

#define ABC_COUCHBASE_LIBEVENTS_MEMBER_DECLARATIONS_MACRO(z, n, text) \
struct event *m_##text##EventCallbackForWt##n;

#define ABC_COUCHBASE_MESSAGE_QUEUE_DECLARATIONS_MACRO(z, n, text) \
message_queue *m_##text##WtMessageQueue##n;

#define ABC_COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_DECLARATION_MACRO(text) \
void *m_##text##MessageQueuesCurrentMessageBuffer; //TODOoptimization: really only need one message buffer queue on backend, but it needs to be the size of the largest type of message (store,storeLarge,get <- most likely storeLarge's size in those 3 xD)

#define ABC_COUCHBASE_LIBEVENTS_GETTER_MEMBER_DECLARATIONS_MACRO(z, n, text) \
struct event *get##text##EventCallbackForWt##n();

#define ABC_COUCHBASE_LIBEVENTS_SLOTS_METHOD_DECLARATIONS_MACRO(z, n, text) \
void eventSlotForWt##text##n();

#define ABC_COUCHBASE_LIBEVENTS_SLOTS_STATIC_METHOD_DECLARATIONS_MACRO(z, n, text) \
static void eventSlotForWt##text##n##Static(evutil_socket_t unusedSocket, short events, void *userData);

#define ABC_COUCHBASE_LIBEVENTS_SINGULAR_SLOT_METHOD_DECLARATIONS_MACRO(text) \
void eventSlotForWt##text();

#define ABC_COUCHBASE_LIBEVENTS_MEMBER_DEFINITIONS_MACRO(z, n, text) \
, m_##text##EventCallbackForWt##n(NULL)

#define ABC_COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_DEFINITION_MACRO(text) \
, m_##text##MessageQueuesCurrentMessageBuffer(NULL)

#define ABC_COUCHBASE_MESSAGE_QUEUE_NULL_INITIALIZATION_MACRO(z, n, text) \
, m_##text##WtMessageQueue##n(NULL)

#define ABC_SETUP_WT_TO_COUCHBASE_CALLBACKS_VIA_EVENT_NEW_MACRO(z, n, text) \
m_##text##EventCallbackForWt##n = event_new(LibEventBaseScopedDeleterInstance.LibEventBase, -1, EV_READ, eventSlotForWt##text##n##Static, this);

#define ABC_FREE_COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_MACRO(text) \
free(m_##text##MessageQueuesCurrentMessageBuffer); \
m_##text##MessageQueuesCurrentMessageBuffer = NULL;

#define ABC_REMOVE_ALL_MESSAGE_QUEUES_MACRO(z, n, text) \
message_queue::remove(ABC_WT_COUCHBASE_MESSAGE_QUEUES_BASE_NAME \
#text \
#n);

#define ABC_CLOSE_AND_DELETE_ALL_NEWD_COUCHBASE_MESSAGE_QUEUES_MACRO(z, n, text) \
delete m_##text##WtMessageQueue##n; \
m_##text##WtMessageQueue##n = NULL;

#define ABC_MALLOC_COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_MACRO(text) \
m_##text##MessageQueuesCurrentMessageBuffer = malloc(ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_##text);

#define ABC_NEW_AND_CREATE_MY_MESSAGE_QUEUES_MACRO(z, n, text) \
m_##text##WtMessageQueue##n = new message_queue(create_only, ABC_WT_COUCHBASE_MESSAGE_QUEUES_BASE_NAME \
#text \
#n, MAX_NUMBER_OF_WT_TO_COUCHBASE_MESSAGES_IN_EACH_QUEUE_FOR_##text, ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_##text);

#define ABC_COUCHBASE_LIBEVENTS_GETTER_MEMBER_DEFINITIONS_MACRO(z, n, text) \
struct event *AnonymousBitcoinComputingCouchbaseDB::get##text##EventCallbackForWt##n() \
{ \
    return m_##text##EventCallbackForWt##n; \
}

#define ABC_COUCHBASE_LIBEVENTS_SLOT_STATIC_METHOD_DEFINITIONS_MACRO(z, n, text) \
void AnonymousBitcoinComputingCouchbaseDB::eventSlotForWt##text##n##Static(evutil_socket_t unusedSocket, short events, void *userData) \
{ \
    (void)unusedSocket; \
    (void)events; \
    static_cast<AnonymousBitcoinComputingCouchbaseDB*>(userData)->eventSlotForWt##text##n(); \
}

#define ABC_COUCHBASE_LIBEVENTS_SLOT_METHOD_DEFINITIONS_MACRO(z, n, text) \
void AnonymousBitcoinComputingCouchbaseDB::eventSlotForWt##text##n() \
{ \
    unsigned int priority; \
    message_queue::size_type actualMessageSize; \
    m_##text##WtMessageQueue##n->receive(m_##text##MessageQueuesCurrentMessageBuffer,(message_queue::size_type)ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_##text, actualMessageSize, priority); \
    eventSlotForWt##text(); \
}

#define ABC_TELL_MAIN_THREAD_THAT_COUCHBASE_FAILED_MACRO \
{ \
    boost::lock_guard<boost::mutex> couchbaseConnectingLock(m_IsConnectedMutex); \
    m_IsDoneInitializing = true; \
} \
m_IsConnectedWaitCondition.notify_one();

#define ABC_GOT_A_PENDING_RESPONSE_FROM_COUCHBASE(GetOrStore) \
--m_Pending##GetOrStore##Count; \
if(m_NoMoreAllowedMuahahaha && m_PendingStoreCount == 0 && m_PendingGetCount == 0) \
{ \
    notifyMainThreadWeAreFinishedWithAllPendingRequests(); \
}

#define ABC_END_OF_WT_REQUEST_LIFETIME_IN_DB_BACKEND_MACRO(GetOrStore) \
delete originalRequest; \
ABC_GOT_A_PENDING_RESPONSE_FROM_COUCHBASE(GetOrStore)

#define ABC_DO_SCHEDULE_COUCHBASE_GET() \
lcb_get_cmd_t cmd; \
const lcb_get_cmd_t *cmds[1]; \
cmds[0] = &cmd; \
const char *keyInput = getCouchbaseDocumentByKeyRequestFromWt->CouchbaseGetKeyInput.c_str(); \
memset(&cmd, 0, sizeof(cmd)); \
cmd.v.v0.key = keyInput; \
cmd.v.v0.nkey = strlen(keyInput); \
lcb_error_t error = lcb_get(m_Couchbase, getCouchbaseDocumentByKeyRequestFromWt, 1, cmds); \
if(error  != LCB_SUCCESS) \
{ \
    cerr << "Failed to setup get request: " << lcb_strerror(m_Couchbase, error) << endl; \
} \
++m_PendingGetCount; //TODOoptional: overflow? meh not worried about it for now xD

#define ABC_DO_SCHEDULE_COUCHBASE_STORE(storeSize) \
if(m_NoMoreAllowedMuahahaha) \
{ \
    return; \
} \
++m_PendingStoreCount; \
StoreCouchbaseDocumentByKeyRequest *originalRequest = new StoreCouchbaseDocumentByKeyRequest(); \
{ \
    std::istringstream originalRequestSerialized(static_cast<const char*>(m_##storeSize##MessageQueuesCurrentMessageBuffer)); \
    boost::archive::text_iarchive deSerializer(originalRequestSerialized); \
    deSerializer >> *originalRequest; \
} \
lcb_store_cmd_t cmd; \
const lcb_store_cmd_t *cmds[1]; \
cmds[0] = &cmd; \
memset(&cmd, 0, sizeof(cmd)); \
const char *keyInput = originalRequest->CouchbaseStoreKeyInput.c_str(); \
const char *documentInput = originalRequest->CouchbaseStoreDocumentInput.c_str(); \
cmd.v.v0.key = keyInput; \
cmd.v.v0.nkey = originalRequest->CouchbaseStoreKeyInput.length(); \
cmd.v.v0.bytes = documentInput; \
cmd.v.v0.nbytes = originalRequest->CouchbaseStoreDocumentInput.length(); \
if(originalRequest->LcbStoreModeIsAdd) \
{ \
    cmd.v.v0.operation = LCB_ADD; \
} \
else \
{ \
    cmd.v.v0.operation = LCB_SET; \
    if(originalRequest->HasCasInput) \
    { \
        cmd.v.v0.cas = originalRequest->CasInput; \
    } \
} \
lcb_error_t err = lcb_store(m_Couchbase, originalRequest, 1, cmds); \
if(err != LCB_SUCCESS) \
{ \
    cerr << "Failed to set up store request: " << lcb_strerror(m_Couchbase, err) << endl; \
    if(originalRequest->SaveCasOutput) \
    { \
        StoreCouchbaseDocumentByKeyRequest::respondWithCas(originalRequest, 0, false, true); \
    } \
    else \
    { \
        StoreCouchbaseDocumentByKeyRequest::respond(originalRequest, false, true); \
    } \
    ABC_END_OF_WT_REQUEST_LIFETIME_IN_DB_BACKEND_MACRO(Store) \
}

//You know you're a god when you can use a BOOST_PP_REPEAT within a call to BOOST_PP_REPEAT
#ifndef ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO
#define ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO(z, n, text) \
BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_SET##n, text, ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET##n)
#define ABC_sofdsufoMACRO_SUBSTITUTION_HACK_FUCK_EVERYTHINGisau(a,b) a(b)
#define ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_MACRO(z, n, text) \
ABC_sofdsufoMACRO_SUBSTITUTION_HACK_FUCK_EVERYTHINGisau(text,ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET##n)
#endif

/////////////////////////////////////////////////////END MACRO HELL///////////////////////////////////////////////

class AnonymousBitcoinComputingCouchbaseDB
{
public:
    AnonymousBitcoinComputingCouchbaseDB();
    boost::condition_variable *isConnectedWaitCondition();
    boost::condition_variable *isFinishedWithAllPendingRequestsWaitCondition();
    boost::mutex *isConnectedMutex();
    boost::mutex *isFinishedWithAllPendingRequestsMutex();
    struct event *beginStoppingCouchbaseCleanlyEvent();
    struct event *finalCleanUpAndJoinEvent();
    bool isDoneInitializing();
    bool isConnected();
    bool isInEventLoop();
    bool isFinishedWithAllPendingRequests();
    void waitForJoin();
    bool threadExittedCleanly();

    //struct event *getStoreEventCallbackForWt0();
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO, ABC_COUCHBASE_LIBEVENTS_GETTER_MEMBER_DECLARATIONS_MACRO)
private:
    boost::thread m_Thread;
    lcb_t m_Couchbase;
    boost::condition_variable m_IsConnectedWaitCondition;
    boost::condition_variable m_IsFinishedWithAllPendingRequestsWaitCondition;
    boost::mutex m_IsConnectedMutex;
    boost::mutex m_IsFinishedWithAllPendingRequestsMutex;
    bool m_IsDoneInitializing;
    bool m_IsConnected;
    bool m_ThreadExittedCleanly;
    bool m_IsInEventLoop;
    bool m_NoMoreAllowedMuahahaha;
    unsigned int m_PendingStoreCount;
    unsigned int m_PendingGetCount; //subscribers do NOT increment the pending count (well, one of them does when it's being used hackily in the polling mechanism). pending count ONLY reflects dispatched-to-couchbase 'gets' that have not returned yet. most subscribers do not do that, hence no incremement. when we 'clean up', we simply forget all the subscribers.
    bool m_IsFinishedWithAllPendingRequests;

    GetAndSubscribeCacheHashType m_GetAndSubscribeCacheHash;
    std::vector<void*> m_UnsubscribeRequestsToProcessMomentarily;
    std::vector<void*> m_ChangeSessionIdRequestsToProcessMomentarily;
    static const struct timeval m_OneHundredMilliseconds;
    struct event *m_GetAndSubscribePollingTimeout; //all keys share a timeout for now (and possibly forever), KISS

    void threadEntryPoint();

    static void errorCallbackStatic(lcb_t instance, lcb_error_t error, const char *errinfo);
    void errorCallback(lcb_error_t error, const char *errinfo);

    static void configurationCallbackStatic(lcb_t instance, lcb_configuration_t config);
    void configurationCallback(lcb_configuration_t config);

    static void storeCallbackStatic(lcb_t instance, const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp);
    void storeCallback(const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp);

    static void getCallbackStatic(lcb_t instance, const void *cookie, lcb_error_t error, const lcb_get_resp_t *resp);
    void getCallback(const void *cookie, lcb_error_t error, const lcb_get_resp_t *resp);

    static void durabilityCallbackStatic(lcb_t instance, const void *cookie, lcb_error_t error, const lcb_durability_resp_t *resp);
    void durabilityCallback(const void *cookie, lcb_error_t error, const lcb_durability_resp_t *resp);

    static void getAndSubscribePollingTimeoutEventSlotStatic(evutil_socket_t unusedSocket, short events, void *userData);
    void getAndSubscribePollingTimeoutEventSlot();

    static void beginStoppingCouchbaseCleanlyEventSlotStatic(evutil_socket_t unusedSocket, short events, void *userData);
    void beginStoppingCouchbaseCleanlyEventSlot();

    static void finalCleanupAndJoin(evutil_socket_t unusedSocket, short events, void *userData);

    void notifyMainThreadWeAreFinishedWithAllPendingRequests();


    struct event *m_BeginStoppingCouchbaseCleanlyEvent;
    struct event *m_FinalCleanUpAndJoinEvent;
    //TODOreq: = NULL;

    //message_queue *m_StoreWtMessageQueue0 = NULL;
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO, ABC_COUCHBASE_MESSAGE_QUEUE_DECLARATIONS_MACRO)

    //void *m_StoreMessageQueue0CurrentMessageBuffer;
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_MACRO, ABC_COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_DECLARATION_MACRO)

    //struct event *m_StoreEventCallbackForWt0 = NULL;
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO, ABC_COUCHBASE_LIBEVENTS_MEMBER_DECLARATIONS_MACRO)

    //static void eventSlotForWtStore0Static(evutil_socket_t unusedSocket, short events, void *userData);
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO, ABC_COUCHBASE_LIBEVENTS_SLOTS_STATIC_METHOD_DECLARATIONS_MACRO)

    //void eventSlotForWtStore0();
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO, ABC_COUCHBASE_LIBEVENTS_SLOTS_METHOD_DECLARATIONS_MACRO)

    //the static methods catch parameters (evutil_socket_t unusedSocket, short events), but we don't use them currently
    //void eventSlotForWtStore();
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_MACRO, ABC_COUCHBASE_LIBEVENTS_SINGULAR_SLOT_METHOD_DECLARATIONS_MACRO)
};

#endif // ANONYMOUSBITCOINCOMPUTINGCOUCHBASEDB_H
