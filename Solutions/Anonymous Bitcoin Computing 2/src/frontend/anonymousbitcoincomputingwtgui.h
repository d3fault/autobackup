#ifndef ANONYMOUSBITCOINCOMPUTINGWTGUI_H
#define ANONYMOUSBITCOINCOMPUTINGWTGUI_H

#include <event2/event.h>

#include <Wt/WEnvironment>
#include <Wt/WApplication>
#include <Wt/WStackedWidget>
#include <Wt/WAnimation>
#include <Wt/WContainerWidget>
#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WAnchor>

//TODOoptimization: a compile time switch alternating between message_queue and lockfree::queue (lockfree::queue doesn't need mutexes or the try, try try, blockLock logic)
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/preprocessor/repeat.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/random/taus88.hpp>
#include <boost/random/uniform_int_distribution.hpp>

using namespace Wt;
using namespace boost::interprocess;
using namespace boost::random;

/////////////////////////////////////////////////////BEGIN MACRO HELL///////////////////////////////////////////////

#if 0
#define WT_MESSAGE_QUEUE_DECLARATIONS_MACRO(z, n, text) \
static message_queue *m_##text##WtMessageQueue##n;
#endif

#define WT_MESSAGE_QUEUE_DEFINITIONS_MACRO(z, n, text) \
message_queue *AnonymousBitcoinComputingWtGUI::m_##text##WtMessageQueues[n] = NULL;

#define WT_NEW_AND_OPEN_ADD_MESSAGE_QUEUE_MACRO(z, n, text) \
m_AddWtMessageQueues[n] = new message_queue(open_only, WT_COUCHBASE_MESSAGE_QUEUES_BASE_NAME \
"Add" \
#n);

#define WT_NEW_AND_OPEN_GET_MESSAGE_QUEUE_MACRO(z, n, text) \
m_GetWtMessageQueues[n] = new message_queue(open_only, WT_COUCHBASE_MESSAGE_QUEUES_BASE_NAME \
"Get" \
#n);

#define WT_DELETE_MESSAGE_QUEUE_MACRO(z, n, text) \
delete m_##text##WtMessageQueues[n]; \
m_##text##WtMessageQueues[n] = NULL;

#if 0
#define WT_LIBEVENTS_MEMBER_DECLARATIONS_MACRO(z, n, text) \
static event *m_##text##EventCallbackForWt##n;
#endif

#define WT_LIBEVENTS_MEMBER_DEFINITIONS_MACRO(z, n, text) \
event *AnonymousBitcoinComputingWtGUI::m_##text##EventCallbacksForWt[n] = NULL;

#if 0
#define WT_MESSAGE_QUEUES_MUTEXES_MEMBER_DECLARATIONS_MACRO(z, n, text) \
static boost::mutex m_##text##WtMessageQueue##n##Mutex;
#endif

/////////////////////////////////////////////////////END MACRO HELL///////////////////////////////////////////////

class AnonymousBitcoinComputingWtGUI : public WApplication
{
    friend class GetCouchbaseDocumentByKeyRequest;
    WStackedWidget *m_MainStack;

    WContainerWidget *m_HomeWidget;
    void showHomeWidget();

    WContainerWidget *m_AdvertisingWidget;
    void showAdvertisingWidget();

    WContainerWidget *m_AdvertisingBuyAdSpaceWidget;
    void showAdvertisingBuyAdSpaceWidget();


    //hardcoded
    WContainerWidget *m_AdvertisingBuyAdSpaceD3faultWidget;
    void showAdvertisingBuyAdSpaceD3faultWidget();
    WContainerWidget *m_AdvertisingBuyAdSpaceD3faultCampaign0Widget;
    void showAdvertisingBuyAdSpaceD3faultCampaign0Widget();


    void beginGetCouchbaseDocumentByKey(const std::string &keyToCouchbaseDocument);
    void getCouchbaseDocumentByKeyFinished(const std::string &keyToCouchbaseDocument, const std::string &couchbaseDocument);


    void handleInternalPathChanged(const std::string &newInternalPath);
    void handleLoggedIn();

    taus88 m_RandomNumberGenerator;
    uniform_int_distribution<> m_AddMessageQueuesRandomIntDistribution;
    uniform_int_distribution<> m_GetMessageQueuesRandomIntDistribution;
    int m_CurrentAddMessageQueueIndex;
    int m_CurrentGetMessageQueueIndex;
public:
    AnonymousBitcoinComputingWtGUI(const WEnvironment &myEnv);

    static void newAndOpenAllWtMessageQueues();
    static void deleteAllWtMessageQueues();

#if 0 //turns out arrays are easier than macros on this side of things
    //static message_queue *m_AddWtMessageQueue0;
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES, WT_MESSAGE_QUEUE_DECLARATIONS_MACRO, Add)
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES, WT_MESSAGE_QUEUE_DECLARATIONS_MACRO, Get)

    //static event *m_AddEventCallbackForWt0;
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES, WT_LIBEVENTS_MEMBER_DECLARATIONS_MACRO, Add)
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES, WT_LIBEVENTS_MEMBER_DECLARATIONS_MACRO, Get)


    //static boost::mutex m_AddWtMessageQueue0Mutex
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES, WT_MESSAGE_QUEUES_MUTEXES_MEMBER_DECLARATIONS_MACRO, Add)
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES, WT_MESSAGE_QUEUES_MUTEXES_MEMBER_DECLARATIONS_MACRO, Get)
#endif
    static message_queue *m_AddWtMessageQueues[NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES];
    static message_queue *m_GetWtMessageQueues[NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES];

    static event *m_AddEventCallbacksForWt[NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES];
    static event *m_GetEventCallbacksForWt[NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES];

    static boost::mutex m_AddMutexArray[NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES];
    static boost::mutex m_GetMutexArray[NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES];
};

#endif // ANONYMOUSBITCOINCOMPUTINGWTGUI_H
