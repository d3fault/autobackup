#ifndef ANONYMOUSBITCOINCOMPUTINGWTGUI_H
#define ANONYMOUSBITCOINCOMPUTINGWTGUI_H

#include <event2/event.h>

#include <Wt/WEnvironment>
#include <Wt/WApplication>
#include <Wt/WStackedWidget>
#include <Wt/WAnimation>
#include <Wt/WContainerWidget>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WAnchor>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>

//TODOoptimization: a compile time switch alternating between message_queue and lockfree::queue (lockfree::queue doesn't need mutexes or the try, try try, blockLock logic)
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/preprocessor/repeat.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/random/taus88.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/uuid/sha1.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>

using namespace Wt;
using namespace boost::interprocess;
using namespace boost::random;
using namespace boost::property_tree;
using namespace boost::archive::iterators;
using namespace std;

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

//see cpp file for original code with comments
#define SERIALIZE_COUCHBASE_REQUEST_AND_SEND_TO_COUCHBASE_ON_RANDOM_MUTEX_PROTECTED_MESSAGE_QUEUE(NormalOperationText, UpperCaseOperationText) \
std::ostringstream couchbaseRequestSerialized; \
{ \
    boost::archive::text_oarchive serializer(couchbaseRequestSerialized); \
    serializer << couchbaseRequest; \
} \
std::string couchbaseRequesSerializedString = couchbaseRequestSerialized.str(); \
size_t couchbaseRequesSerializedStringLength = couchbaseRequesSerializedString.length(); \
if(couchbaseRequesSerializedStringLength > WT_TO_COUCHBASE_##UpperCaseOperationText##_MAX_MESSAGE_SIZE) \
{ \
    return; \
} \
++m_Current##NormalOperationText##MessageQueueIndex; \
if(m_Current##NormalOperationText##MessageQueueIndex == NUMBER_OF_WT_TO_COUCHBASE_##UpperCaseOperationText##_MESSAGE_QUEUES) \
{ \
    m_Current##NormalOperationText##MessageQueueIndex = 0; \
} \
int lockedMutexIndex = m_Current##NormalOperationText##MessageQueueIndex; \
int veryLastMutexIndexToBlockLock = (m_Current##NormalOperationText##MessageQueueIndex == 0 ? (NUMBER_OF_WT_TO_COUCHBASE_##UpperCaseOperationText##_MESSAGE_QUEUES) : (m_Current##NormalOperationText##MessageQueueIndex-1)); \
while(true) \
{ \
    if(lockedMutexIndex == veryLastMutexIndexToBlockLock) \
    { \
        m_##NormalOperationText##MutexArray[lockedMutexIndex].lock(); \
        break; \
    } \
    if(m_##NormalOperationText##MutexArray[lockedMutexIndex].try_lock()) \
    { \
        break; \
    } \
    ++lockedMutexIndex; \
    if(lockedMutexIndex == NUMBER_OF_WT_TO_COUCHBASE_##UpperCaseOperationText##_MESSAGE_QUEUES) \
    { \
        lockedMutexIndex = 0; \
    } \
} \
m_##NormalOperationText##WtMessageQueues[lockedMutexIndex]->send(couchbaseRequesSerializedString.c_str(), couchbaseRequesSerializedStringLength, 0); \
event_active(m_##NormalOperationText##EventCallbacksForWt[lockedMutexIndex], EV_READ|EV_WRITE, 0); \
m_##NormalOperationText##MutexArray[lockedMutexIndex].unlock();

/////////////////////////////////////////////////////END MACRO HELL///////////////////////////////////////////////

typedef transform_width< binary_from_base64<string::const_iterator>, 8, 6 > Base64ToBinaryBoostTypedef;
typedef base64_from_binary<transform_width<string::const_iterator, 6, 8 > > BinaryToBase64BoostTypedef;

class AnonymousBitcoinComputingWtGUI : public WApplication
{
    friend class AddCouchbaseDocumentByKeyRequest;
    friend class GetCouchbaseDocumentByKeyRequest;

    void buildGui();
    WHBoxLayout *m_HeaderHlayout;
    WVBoxLayout *m_MainVLayout;

    WStackedWidget *m_LoginLogoutStackWidget; //2 widgets in the stack ("login" and "logout". login widget has link to register)
    WContainerWidget *m_LoginWidget; //since we don't use cookies/etc, it is IMPOSSIBLE to not have the login widget shown, so we set it up asap (constructor). Logout widget is state dependent though, so we initialize it to zero
    WLineEdit *m_LoginUsernameLineEdit;
    WLineEdit *m_LoginPasswordLineEdit;
    WContainerWidget *m_LogoutWidget;

    WStackedWidget *m_MainStack;

    //stack items for m_MainStack follow
    WContainerWidget *m_HomeWidget;
    void showHomeWidget();

    WContainerWidget *m_AdvertisingWidget;
    void showAdvertisingWidget();

    WContainerWidget *m_AdvertisingBuyAdSpaceWidget;
    void showAdvertisingBuyAdSpaceWidget();

    WContainerWidget *m_RegisterWidget;
    WLineEdit *m_RegisterUsername;
    WLineEdit *m_RegisterPassword;
    void showRegisterWidget();
    //WContainerWidget *m_RegisterSuccessfulWidget;

    //hardcoded
    WContainerWidget *m_AdvertisingBuyAdSpaceD3faultWidget;
    void showAdvertisingBuyAdSpaceD3faultWidget();
    WContainerWidget *m_AdvertisingBuyAdSpaceD3faultCampaign0Widget;
    void beginShowingAdvertisingBuyAdSpaceD3faultCampaign0Widget();
    void finishShowingAdvertisingBuyAdSpaceD3faultCampaign0Widget(const std::string &couchbaseDocument);

    void beginGetCouchbaseDocumentByKey(const std::string &keyToCouchbaseDocument);
    void beginAddCouchbaseDocumentByKey(const std::string &keyToCouchbaseDocument, const std::string &couchbaseDocument);
    void getCouchbaseDocumentByKeyFinished(const std::string &keyToCouchbaseDocument, const std::string &couchbaseDocument);
    void addCouchbaseDocumentByKeyFinished(const std::string &keyToCouchbaseDocument);

    bool isHomePath(const std::string &pathToCheck);
    void handleInternalPathChanged(const std::string &newInternalPath);
    void handleRegisterButtonClicked();
    void handleLoginButtonClicked();
    void loginIfInputHashedEqualsDbInfo(const std::string &userProfileCouchbaseDocAsJson);
    void handleLogoutButtonClicked();

    taus88 m_RandomNumberGenerator;
    uniform_int_distribution<> m_AddMessageQueuesRandomIntDistribution;
    uniform_int_distribution<> m_GetMessageQueuesRandomIntDistribution;
    int m_CurrentAddMessageQueueIndex;
    int m_CurrentGetMessageQueueIndex;

    enum WhatTheAddWasForEnum { INITIALINVALIDNULLADD, REGISTERATTEMPTADD };
    enum WhatTheGetWasForEnum { INITIALINVALIDNULLGET, HACKEDIND3FAULTCAMPAIGN0GET, LOGINATTEMPTGET };

    WhatTheAddWasForEnum m_WhatTheAddWasFor;
    WhatTheGetWasForEnum m_WhatTheGetWasFor;

    bool m_LoggedIn;
    WString m_Username; //only valid if logged in
public:
    AnonymousBitcoinComputingWtGUI(const WEnvironment &myEnv);

    static string sha1string(const std::string &inputString);
    static std::string toBase64(const std::string &inputString);

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
