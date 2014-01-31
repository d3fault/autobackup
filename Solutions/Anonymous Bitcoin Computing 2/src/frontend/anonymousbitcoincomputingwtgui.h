#ifndef ANONYMOUSBITCOINCOMPUTINGWTGUI_H
#define ANONYMOUSBITCOINCOMPUTINGWTGUI_H

#include <string>

#include <event2/event.h>

#include <Wt/WEnvironment>
#include <Wt/WApplication>
#include <Wt/WStackedWidget>
#include <Wt/WAnimation>
#include <Wt/WContainerWidget>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WGridLayout>
#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WAnchor>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WComboBox>
#include <Wt/Utils>
#include <Wt/WDateTime> //TODOreq: take out time.h above and deps
//#include <Wt/Chart/WCartesianChart>

//TODOoptimization: a compile time switch alternating between message_queue and lockfree::queue (lockfree::queue doesn't need mutexes or the try, try try, blockLock logic)
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/preprocessor/repeat.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/random/taus88.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include "../frontend2backendRequests/storecouchbasedocumentbykeyrequest.h"
#include "../frontend2backendRequests/getcouchbasedocumentbykeyrequest.h"

using namespace Wt;
using namespace Wt::Utils;
using namespace boost::interprocess;
using namespace boost::random;
using namespace boost::property_tree;
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

class AnonymousBitcoinComputingWtGUI : public WApplication
{
    friend class StoreCouchbaseDocumentByKeyRequest;
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
    WLineEdit *m_RegisterUsernameLineEdit;
    WLineEdit *m_RegisterPasswordLineEdit;
    void showRegisterWidget();
    void registerAttemptFinished(bool lcbOpSuccess, bool dbError);
    //WContainerWidget *m_RegisterSuccessfulWidget;

    //hardcoded
    WContainerWidget *m_AdvertisingBuyAdSpaceD3faultWidget;
    void showAdvertisingBuyAdSpaceD3faultWidget();
    WContainerWidget *m_AdvertisingBuyAdSpaceD3faultCampaign0Widget;
    bool m_FirstPopulate;
    WText *m_CurrentPriceLabel;
    WPushButton *m_BuySlotFillerStep1Button;
    void beginShowingAdvertisingBuyAdSpaceD3faultCampaign0Widget();
    std::string m_HackedInD3faultCampaign0JsonDocForUpdatingLaterAfterSuccessfulPurchase;
    u_int64_t m_HackedInD3faultCampaign0CasForSafelyUpdatingCampaignDocLaterAfterSuccessfulPurchase;
    std::string m_HackedInD3faultCampaign0_MinPrice;
    std::string m_HackedInD3faultCampaign0_SlotLengthHours;
    bool m_HackedInD3faultCampaign0_NoPreviousSlotPurchases;
    std::string m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedSlotIndex;
    std::string m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedPurchaseTimestamp;
    std::string m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedStartTimestamp;
    std::string m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedPurchasePrice;
    double calculateCurrentPrice(double currentTime_x, double minPrice_y2, double lastSlotFilledAkaPurchasedPurchasePriceDoubled_y1, double lastSlotFilledAkaPurchasedExpireDateTime_x2, double lastSlotFilledAkaPurchasedPurchaseDateTime_x1);
    void finishShowingAdvertisingBuyAdSpaceD3faultCampaign0Widget(const std::string &couchbaseDocument, u_int64_t casForSafelyUpdatingCampaignDocAfterSuccesfulPurchase);
    void buySlotStep1d3faultCampaign0ButtonClicked();
    void buySlotPopulateStep2d3faultCampaign0(const string &allSlotFillersJsonDoc, bool lcbOpSuccess, bool dbError);
    void buySlotStep2d3faultCampaign0ButtonClicked();
    WComboBox *m_AllSlotFillersComboBox; //TODOoptimization: meh slot buying page needs to break out to it's own object methinks... fuck it for now
    std::string m_SlotFillerToUseInBuy;
    void verifyUserHasSufficientFundsAndThatTheirAccountIsntAlreadyLockedAndThenStartTryingToLockItIfItIsntAlreadyLocked(const string &userAccountJsonDoc, u_int64_t cas, bool lcbOpSuccess, bool dbError);
    bool m_HackedInD3faultCampaign0_LastSlotPurchasesIsExpired;
    double m_CurrentPriceToUseForBuying;
    std::string m_LastSlotFilledAkaPurchasedExpireDateTime_ToBeUsedAsStartDateTimeIfTheBuySucceeds;
    std::string m_CurrentPriceToUseForBuyingString;
    std::string m_AdSlotIndexToBeFilledIfLockIsSuccessful_AndForUseInUpdateCampaignDocAfterPurchase;
    std::string m_AdSlotAboutToBeFilledIfLockIsSuccessful;
    void userAccountLockAttemptFinish_IfOkayDoTheActualSlotFillAdd(u_int64_t casFromLockSoWeCanSafelyUnlockLater, bool lcbOpSuccess, bool dbError);
    std::string m_StartTimestampUsedInNewPurchase;
    u_int64_t m_CasFromUserAccountLockSoWeCanSafelyUnlockLater;
    std::string m_PurchaseTimestampForUseInSlotItselfAndAlsoUpdatingCampaignDocAfterPurchase;
    std::string m_UserAccountLockedDuringBuyJson;
    void slotFillAkaPurchaseAddAttemptFinished(bool lcbOpSuccess, bool dbError);
    void transactionDocCreatedSoCasSwapUnlockAcceptingFailUserAccountDebitting(bool dbError);
    void doneUnlockingUserAccountAfterSuccessfulPurchaseSoNowUpdateCampaignDocCasSwapAcceptingFail_SettingOurPurchaseAsLastPurchase(bool dbError);
    void doneUpdatingCampaignDocSoErrYeaTellUserWeAreCompletelyDoneWithTheSlotFillAkaPurchase(bool dbError);

    void getCouchbaseDocumentByKeyBegin(const std::string &keyToCouchbaseDocument);
    void getCouchbaseDocumentByKeySavingCasBegin(const std::string &keyToCouchbaseDocument);
    void getAndSubscribeCouchbaseDocumentByKeySavingCas(const std::string &keyToCouchbaseDocument);
    void storeWithoutInputCasCouchbaseDocumentByKeyBegin(const std::string &keyToCouchbaseDocument, const std::string &couchbaseDocument, StoreCouchbaseDocumentByKeyRequest::LcbStoreMode_AndWhetherOrNotThereIsInputCasEnum storeMode = StoreCouchbaseDocumentByKeyRequest::AddMode);
    void setCouchbaseDocumentByKeyWithInputCasBegin(const std::string &keyToCouchbaseDocument, const std::string &couchbaseDocument, u_int64_t cas, StoreCouchbaseDocumentByKeyRequest::WhatToDoWithOutputCasEnum whatToDoWithOutputCasEnum);
    void getCouchbaseDocumentByKeyFinished(const std::string &keyToCouchbaseDocument, const std::string &couchbaseDocument, bool lcbOpSuccess, bool dbError);
    void getCouchbaseDocumentByKeySavingCasFinished(const std::string &keyToCouchbaseDocument, const std::string &couchbaseDocument, u_int64_t cas, bool lcbOpSuccess, bool dbError);
    void getAndSubscribeCouchbaseDocumentByKeySavingCas_UPDATE(const std::string &keyToCouchbaseDocument, const std::string &couchbaseDocument, u_int64_t cas, bool lcbOpSuccess, bool dbError);

    bool isHomePath(const std::string &pathToCheck);
    void handleInternalPathChanged(const std::string &newInternalPath);
    void handleRegisterButtonClicked();
    void handleLoginButtonClicked();
    void loginIfInputHashedEqualsDbInfo(const std::string &userProfileCouchbaseDocAsJson, bool lcbOpSuccess, bool dbError);
    void handleLogoutButtonClicked();

    taus88 m_RandomNumberGenerator;
    uniform_int_distribution<> m_AddMessageQueuesRandomIntDistribution;
    uniform_int_distribution<> m_GetMessageQueuesRandomIntDistribution;
    int m_CurrentAddMessageQueueIndex;
    int m_CurrentGetMessageQueueIndex;

    enum WhatTheStoreWithoutInputCasWasForEnum
    {
        INITIALINVALIDNULLSTOREWITHOUTINPUTCAS,
        REGISTERATTEMPTSTOREWITHOUTINPUTCAS,
        BUYAKAFILLSLOTWITHSLOTFILLERSTOREWITHOUTINPUTCAS,
        CREATETRANSACTIONDOCSTOREWITHOUTINPUTCAS
    };
    enum WhatTheSetWithInputCasWasForEnum
    {
        INITIALINVALIDNULLSETWITHCAS,
        //now there aren't any ops i'm using that set with an input cas without using it again later (saving the cas), but there probably will be in the future...
        //SEE!
        HACKEDIND3FAULTCAMPAIGN0BUYPURCHASSUCCESSFULSOUNLOCKUSERACCOUNTSAFELYUSINGCAS,
        HACKEDIND3FAULTCAMPAIGN0USERACCOUNTUNLOCKDONESOUPDATECAMPAIGNDOCSETWITHINPUTCAS
    };
    enum WhatTheSetWithInputCasSavingOutputCasWasForEnum
    {
        INITIALINVALIDNULLSETWITHCASSAVINGCAS,
        HACKEDIND3FAULTCAMPAIGN0BUYSTEP2bLOCKACCOUNTFORBUYINGSETWITHCASSAVINGCAS
    };
    enum WhatTheGetWasForEnum
    {
        INITIALINVALIDNULLGET,
        LOGINATTEMPTGET,
        HACKEDIND3FAULTCAMPAIGN0BUYSTEP1GET
    };
    enum WhatTheGetSavingCasWasForEnum
    {
        INITIALINVALIDNULLGETSAVINGCAS,
        HACKEDIND3FAULTCAMPAIGN0GET,
        HACKEDIND3FAULTCAMPAIGN0BUYSTEP2aVERIFYBALANCEANDGETCASFORSWAPLOCKGET
    };
    enum WhatTheGetAndSubscribeSavingCasWasForEnum
    {
        INITIALINVALIDNULLGETANDSUBSCRIBESAVINGCAS,
        HACKEDIND3FAULTCAMPAIGN0GETANDSUBSCRIBESAVINGCAS
    };

    WhatTheStoreWithoutInputCasWasForEnum m_WhatTheStoreWIthoutInputCasWasFor;
    WhatTheSetWithInputCasWasForEnum m_WhatTheSetWithInputCasWasFor;
    WhatTheSetWithInputCasSavingOutputCasWasForEnum m_WhatTheSetWithInputCasSavingOutputCasWasFor;
    WhatTheGetWasForEnum m_WhatTheGetWasFor;
    WhatTheGetSavingCasWasForEnum m_WhatTheGetSavingCasWasFor;
    WhatTheGetAndSubscribeSavingCasWasForEnum m_WhatTheGetAndSubscribeSavingCasWasFor;

    bool m_LoggedIn;
    WString m_BuyerUsername; //only valid if logged in
public:
    AnonymousBitcoinComputingWtGUI(const WEnvironment &myEnv);
    virtual void finalize();

    static void newAndOpenAllWtMessageQueues();
    static void deleteAllWtMessageQueues();

    //had these private + friend class'd and it worked a little ago, but now it's bitching. try to move them back to private if you want
    void storeWIthoutInputCasCouchbaseDocumentByKeyFinished(const std::string &keyToCouchbaseDocument, bool lcbOpSuccess, bool dbError);
    void setCouchbaseDocumentByKeyWithInputCasFinished(const std::string &keyToCouchbaseDocument, bool lcbOpSuccess, bool dbError);
    void setCouchbaseDocumentByKeyWithInputCasSavingOutputCasFinished(const std::string &keyToCouchbaseDocument, u_int64_t outputCas, bool lcbOpSuccess, bool dbError);

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
