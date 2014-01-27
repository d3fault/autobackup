#ifndef ANONYMOUSBITCOINCOMPUTINGCOUCHBASEDB_H
#define ANONYMOUSBITCOINCOMPUTINGCOUCHBASEDB_H

#include <libcouchbase/couchbase.h>
#include <event2/event.h>
#include <event2/thread.h>

#include <boost/preprocessor/repeat.hpp>
#include <boost/thread.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/function.hpp>

using namespace boost::interprocess;

/////////////////////////////////////////////////////BEGIN MACRO HELL///////////////////////////////////////////////

#define COUCHBASE_LIBEVENTS_MEMBER_DECLARATIONS_MACRO(z, n, text) \
struct event *m_##text##EventCallbackForWt##n;

#define COUCHBASE_LIBEVENTS_MEMBER_DEFINITIONS_MACRO(z, n, text) \
, m_##text##EventCallbackForWt##n(NULL)

#define COUCHBASE_LIBEVENTS_SLOTS_STATIC_METHOD_DECLARATIONS_MACRO(z, n, text) \
static void eventSlotForWt##text##n##Static(evutil_socket_t unusedSocket, short events, void *userData);

#define COUCHBASE_LIBEVENTS_SLOTS_METHOD_DECLARATIONS_MACRO(z, n, text) \
void eventSlotForWt##text##n();

#define HACK_WT_TO_COUCHBASE_MAX_MESSAGE_SIZE_Add WT_TO_COUCHBASE_ADD_MAX_MESSAGE_SIZE
#define HACK_WT_TO_COUCHBASE_MAX_MESSAGE_SIZE_Get WT_TO_COUCHBASE_GET_MAX_MESSAGE_SIZE

#define COUCHBASE_LIBEVENTS_SLOT_STATIC_METHOD_DEFINITIONS_MACRO(z, n, text) \
void AnonymousBitcoinComputingCouchbaseDB::eventSlotForWt##text##n##Static(evutil_socket_t unusedSocket, short events, void *userData) \
{ \
    (void)unusedSocket; \
    (void)events; \
    static_cast<AnonymousBitcoinComputingCouchbaseDB*>(userData)->eventSlotForWt##text##n(); \
}

#define COUCHBASE_LIBEVENTS_SLOT_METHOD_DEFINITIONS_MACRO(z, n, text) \
void AnonymousBitcoinComputingCouchbaseDB::eventSlotForWt##text##n() \
{ \
    unsigned int priority; \
    message_queue::size_type actualMessageSize; \
    m_##text##WtMessageQueue##n->receive(m_##text##MessageQueuesCurrentMessageBuffer,(message_queue::size_type)HACK_WT_TO_COUCHBASE_MAX_MESSAGE_SIZE_##text, actualMessageSize, priority); \
    eventSlotForWt##text(); \
}

#define COUCHBASE_LIBEVENTS_GETTER_MEMBER_DECLARATIONS_MACRO(z, n, text) \
struct event *get##text##EventCallbackForWt##n();

#define COUCHBASE_LIBEVENTS_GETTER_MEMBER_DEFINITIONS_MACRO(z, n, text) \
struct event *AnonymousBitcoinComputingCouchbaseDB::get##text##EventCallbackForWt##n() \
{ \
    return m_##text##EventCallbackForWt##n; \
}

#define COUCHBASE_MESSAGE_QUEUE_DECLARATIONS_MACRO(z, n, text) \
message_queue *m_##text##WtMessageQueue##n;

#define COUCHBASE_MESSAGE_QUEUE_DEFINITIONS_MACRO(z, n, text) \
, m_##text##WtMessageQueue##n(NULL)

#if 0 //only need one buffer, not a buffer per queue
#define COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_DECLARATIONS_MACRO(z, n, text) \
void *m_##text##MessageQueue##n##CurrentMessageBuffer;
//TODOreq: = NULL;
#endif
#define COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_DECLARATION_MACRO(text) \
void *m_##text##MessageQueuesCurrentMessageBuffer;

#define COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_DEFINITION_MACRO(text) \
, m_##text##MessageQueuesCurrentMessageBuffer(NULL)

#if 0 //only need one buffer, not a buffer per queue
#define MALLOC_COUCHBASE_ADD_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFERS_MACRO(z, n, text) \
m_AddMessageQueue##n##CurrentMessageBuffer = malloc(WT_TO_COUCHBASE_ADD_MAX_MESSAGE_SIZE);
#define MALLOC_COUCHBASE_GET_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFERS_MACRO(z, n, text) \
m_GetMessageQueue##n##CurrentMessageBuffer = malloc(WT_TO_COUCHBASE_GET_MAX_MESSAGE_SIZE);
#endif
#define MALLOC_COUCHBASE_ADD_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_MACRO() \
m_AddMessageQueuesCurrentMessageBuffer = malloc(WT_TO_COUCHBASE_ADD_MAX_MESSAGE_SIZE);
#define MALLOC_COUCHBASE_GET_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_MACRO() \
m_GetMessageQueuesCurrentMessageBuffer = malloc(WT_TO_COUCHBASE_GET_MAX_MESSAGE_SIZE);

#if 0 //only need one buffer, not a buffer per queue
#define FREE_COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFERS_MACRO(z, n, text) \
free(m_##text##MessageQueue##n##CurrentMessageBuffer); \
m_##text##MessageQueue##n##CurrentMessageBuffer = NULL;
#endif
#define FREE_COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_MACRO(text) \
free(m_##text##MessageQueuesCurrentMessageBuffer); \
m_##text##MessageQueuesCurrentMessageBuffer = NULL;

#define SETUP_WT_TO_COUCHBASE_CALLBACKS_VIA_EVENT_NEW_MACRO(z, n, text) \
m_##text##EventCallbackForWt##n = event_new(LibEventBaseScopedDeleterInstance.LibEventBase, -1, EV_READ, eventSlotForWt##text##n##Static, this);

#define TELL_MAIN_THREAD_THAT_COUCHBASE_FAILED_MACRO \
{ \
    boost::lock_guard<boost::mutex> couchbaseConnectingLock(m_IsConnectedMutex); \
    m_IsDoneInitializing = true; \
} \
m_IsConnectedWaitCondition.notify_one();

#define REMOVE_ALL_MESSAGE_QUEUES_MACRO(z, n, text) \
message_queue::remove(WT_COUCHBASE_MESSAGE_QUEUES_BASE_NAME \
#text \
#n);

#define NEW_AND_CREATE_MY_ADD_MESSAGE_QUEUES_MACRO(z, n, text) \
m_AddWtMessageQueue##n = new message_queue(create_only, WT_COUCHBASE_MESSAGE_QUEUES_BASE_NAME \
"Add" \
#n, WT_TO_COUCHBASE_ADD_MAX_MESSAGES_IN_QUEUE, WT_TO_COUCHBASE_ADD_MAX_MESSAGE_SIZE);

#define NEW_AND_CREATE_MY_GET_MESSAGE_QUEUES_MACRO(z, n, text) \
m_GetWtMessageQueue##n = new message_queue(create_only, WT_COUCHBASE_MESSAGE_QUEUES_BASE_NAME \
"Get" \
#n, WT_TO_COUCHBASE_GET_MAX_MESSAGES_IN_QUEUE, WT_TO_COUCHBASE_GET_MAX_MESSAGE_SIZE);

#define CLOSE_AND_DELETE_ALL_NEWD_COUCHBASE_MESSAGE_QUEUES_MACRO(z, n, text) \
delete m_##text##WtMessageQueue##n; \
m_##text##WtMessageQueue##n = NULL;

#define END_OF_WT_REQUEST_LIFETIME_IN_DB_BACKEND_MACRO(GetOrAdd) \
delete originalRequest; \
--m_Pending##GetOrAdd##Count; \
if(m_NoMoreAllowedMuahahaha && m_PendingAddCount == 0 && m_PendingGetCount == 0) \
{ \
    notifyMainThreadWeAreFinishedWithAllPendingRequests(); \
} \

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

    //struct event *getAddEventCallbackForWt0();
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES, COUCHBASE_LIBEVENTS_GETTER_MEMBER_DECLARATIONS_MACRO, Add)
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES, COUCHBASE_LIBEVENTS_GETTER_MEMBER_DECLARATIONS_MACRO, Get)
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
    unsigned int m_PendingAddCount;
    unsigned int m_PendingGetCount;
    bool m_IsFinishedWithAllPendingRequests;

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

    static void beginStoppingCouchbaseCleanlyEventSlotStatic(evutil_socket_t unusedSocket, short events, void *userData);
    void beginStoppingCouchbaseCleanlyEventSlot();

    static void finalCleanupAndJoin(evutil_socket_t unusedSocket, short events, void *userData);

    void notifyMainThreadWeAreFinishedWithAllPendingRequests();


    struct event *m_BeginStoppingCouchbaseCleanlyEvent;
    struct event *m_FinalCleanUpAndJoinEvent;
    //TODOreq: = NULL;

    //message_queue *m_AddWtMessageQueue0 = NULL;
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES, COUCHBASE_MESSAGE_QUEUE_DECLARATIONS_MACRO, Add)
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES, COUCHBASE_MESSAGE_QUEUE_DECLARATIONS_MACRO, Get)

    //void *m_AddMessageQueue0CurrentMessageBuffer = NULL;
#if 0 //only need one buffer, not a buffer per queue (one altogether would work, except that they are different sizes)
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES, COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_DECLARATIONS_MACRO, Add)
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES, COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_DECLARATIONS_MACRO, Get)
#endif
    COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_DECLARATION_MACRO(Add)
    COUCHBASE_MESSAGE_QUEUES_CURRENT_MESSAGE_BUFFER_DECLARATION_MACRO(Get)

    //struct event *m_AddEventCallbackForWt0 = NULL;
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES, COUCHBASE_LIBEVENTS_MEMBER_DECLARATIONS_MACRO, Add)
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES, COUCHBASE_LIBEVENTS_MEMBER_DECLARATIONS_MACRO, Get)

    //static void eventSlotForWtAdd0Static(evutil_socket_t unusedSocket, short events, void *userData);
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES, COUCHBASE_LIBEVENTS_SLOTS_STATIC_METHOD_DECLARATIONS_MACRO, Add)
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES, COUCHBASE_LIBEVENTS_SLOTS_STATIC_METHOD_DECLARATIONS_MACRO, Get)

    //void eventSlotForWtAdd0();
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES, COUCHBASE_LIBEVENTS_SLOTS_METHOD_DECLARATIONS_MACRO, Add)
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES, COUCHBASE_LIBEVENTS_SLOTS_METHOD_DECLARATIONS_MACRO, Get)

    //the static methods catch parameters (evutil_socket_t unusedSocket, short events), but we don't use them currently
    void eventSlotForWtAdd();
    void eventSlotForWtGet();
};

#endif // ANONYMOUSBITCOINCOMPUTINGCOUCHBASEDB_H
