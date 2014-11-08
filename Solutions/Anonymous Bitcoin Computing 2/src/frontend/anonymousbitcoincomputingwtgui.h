#ifndef ANONYMOUSBITCOINCOMPUTINGWTGUI_H
#define ANONYMOUSBITCOINCOMPUTINGWTGUI_H

#include <string>
#include <utility>

#include <event2/event.h>

#include <Wt/WEnvironment>
#include <Wt/WApplication>
#include <Wt/WStackedWidget>
//#include <Wt/WAnimation>
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
#include <Wt/WDateTime>
#include <Wt/WFileUpload>
#include <Wt/WImage>
#include <Wt/WLink>
#include <Wt/WDefaultLoadingIndicator>
//#include <Wt/Chart/WCartesianChart>

#include <boost/preprocessor/repeat.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include "../abc2common.h"

#ifdef ABC_USE_BOOST_LOCKFREE_QUEUE
#include <boost/lockfree/queue.hpp>
using namespace boost::lockfree;
#else
#include <boost/interprocess/ipc/message_queue.hpp>
using namespace boost::interprocess;
#endif // ABC_USE_BOOST_LOCKFREE_QUEUE

#include "accounttabs/actuallazyloadedtabwidget.h"

#include "../frontend2backendRequests/storecouchbasedocumentbykeyrequest.h"
#include "../frontend2backendRequests/getcouchbasedocumentbykeyrequest.h"
#include "../frontend2backendRequests/viewquerycouchbasedocumentbykeyrequest.h"

#include "abc2couchbaseandjsonkeydefines.h"

using namespace Wt;
using namespace Wt::Utils;
using namespace boost::property_tree;
using namespace std;

/////////////////////////////////////////////////////BEGIN MACRO HELL///////////////////////////////////////////////

//100ms javascript interval
#define ABC_START_JS_INTERVAL_SNIPPET m_CurrentPriceDomPath + ".z0bj.tehIntervalz = setInterval(" + m_CurrentPriceDomPath + ".z0bj.tehIntervalzCallback, 100);" + m_CurrentPriceDomPath + ".z0bj.tehIntervalIsRunnan = true;"

#ifdef ABC_USE_BOOST_LOCKFREE_QUEUE
#define ABC_WT_STATIC_MESSAGE_QUEUE_SOURCE_DEFINITION_MACRO(text) \
    queue<text##CouchbaseDocumentByKeyRequest*> *AnonymousBitcoinComputingWtGUI::m_##text##WtMessageQueues[ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_##text];
#else
#define ABC_WT_STATIC_MESSAGE_QUEUE_SOURCE_DEFINITION_MACRO(text) \
    message_queue *AnonymousBitcoinComputingWtGUI::m_##text##WtMessageQueues[ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_##text];
#endif // ABC_USE_BOOST_LOCKFREE_QUEUE

#define ABC_WT_STATIC_EVENT_SOURCE_DEFINITION_MACRO(text) \
event *AnonymousBitcoinComputingWtGUI::m_##text##EventCallbacksForWt[ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_##text];

#ifdef ABC_USE_BOOST_LOCKFREE_QUEUE
    #define ABC_WT_STATIC_MUTEX_SOURCE_DEFINITION_MACRO(text)
#else
#define ABC_WT_STATIC_MUTEX_SOURCE_DEFINITION_MACRO(text) \
    boost::mutex AnonymousBitcoinComputingWtGUI::m_##text##MutexArray[ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_##text];
#endif // ABC_USE_BOOST_LOCKFREE_QUEUE

#ifdef ABC_USE_BOOST_LOCKFREE_QUEUE
#define ABC_WT_STATIC_MESSAGE_QUEUE_HEADER_DECLARATION_MACRO(text) \
    static queue<text##CouchbaseDocumentByKeyRequest*> *m_##text##WtMessageQueues[ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_##text];
#else
#define ABC_WT_STATIC_MESSAGE_QUEUE_HEADER_DECLARATION_MACRO(text) \
    static message_queue *m_##text##WtMessageQueues[ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_##text];
#endif // ABC_USE_BOOST_LOCKFREE_QUEUE

#define ABC_WT_STATIC_EVENT_HEADER_DECLARATION_MACRO(text) \
static event *m_##text##EventCallbacksForWt[ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_##text];

#ifdef ABC_USE_BOOST_LOCKFREE_QUEUE
#define ABC_WT_STATIC_MUTEX_HEADER_DECLARATION_MACRO(text)
#else
#define ABC_WT_STATIC_MUTEX_HEADER_DECLARATION_MACRO(text) \
    static boost::mutex m_##text##MutexArray[ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_##text];
#endif // ABC_USE_BOOST_LOCKFREE_QUEUE

#ifdef ABC_USE_BOOST_LOCKFREE_QUEUE
#define ABC_WT_NEW_AND_OPEN_MESSAGE_QUEUE_MACRO(z, n, text)
#else
#define ABC_WT_NEW_AND_OPEN_MESSAGE_QUEUE_MACRO(z, n, text) \
m_##text##WtMessageQueues[n] = new message_queue(open_only, ABC_WT_COUCHBASE_MESSAGE_QUEUES_BASE_NAME \
#text \
#n);
#endif // ABC_USE_BOOST_LOCKFREE_QUEUE


#if 0
#define ABC_WT_PER_QUEUE_SET_UNIFORM_INT_DISTRIBUTION_CONSTRUCTOR_INITIALIZATION_MACRO(text) \
uniform_int_distribution<> l_##text##MessageQueuesRandomIntDistribution(0, ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_##text - 1); \
m_Current##text##MessageQueueIndex = l_##text##MessageQueuesRandomIntDistribution(mersenneTwisterRandomNumberGenerator);
#endif

#define ABC_WT_PER_QUEUE_SET_UNIFORM_INT_DISTRIBUTION_CONSTRUCTOR_INITIALIZATION_MACRO(text) \
m_Current##text##MessageQueueIndex = rawUniqueId() % ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_##text;

#define ABC_WT_PER_QUEUE_CURRENT_RANDOM_INDEX_DECLARATION_MACRO(text) \
int m_Current##text##MessageQueueIndex;

#ifdef ABC_USE_BOOST_LOCKFREE_QUEUE
#define ABC_WT_DELETE_MESSAGE_QUEUE_MACRO(z, n, text) \
m_##text##WtMessageQueues[n] = NULL; //couchbase side deletes the lockfree queues
#else
#define ABC_WT_DELETE_MESSAGE_QUEUE_MACRO(z, n, text) \
delete m_##text##WtMessageQueues[n]; \
m_##text##WtMessageQueues[n] = NULL;
#endif /*ABC_USE_BOOST_LOCKFREE_QUEUE*/

#ifndef ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO
#define ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO(z, n, text) \
BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_SET##n, text, ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET##n)
#define ABC_sofdsufoMACRO_SUBSTITUTION_HACK_FUCK_EVERYTHINGisau(a,b) a(b)
#define ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_MACRO(z, n, text) \
ABC_sofdsufoMACRO_SUBSTITUTION_HACK_FUCK_EVERYTHINGisau(text,ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET##n)
#endif

//see cpp file for original code with comments
#ifdef ABC_USE_BOOST_LOCKFREE_QUEUE

#define ABC_SERIALIZE_COUCHBASE_REQUEST_AND_SEND_TO_COUCHBASE_ON_RANDOM_MUTEX_PROTECTED_MESSAGE_QUEUE(text) \
++m_Current##text##MessageQueueIndex; \
if(m_Current##text##MessageQueueIndex == ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_##text) \
{ \
    m_Current##text##MessageQueueIndex = 0; \
} \
int lockedMutexIndex = m_Current##text##MessageQueueIndex; \
int veryLastMutexIndexToBlockLock = (m_Current##text##MessageQueueIndex == 0 ? (ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_##text) : (m_Current##text##MessageQueueIndex-1)); \
while(true) \
{ \
    if(lockedMutexIndex == veryLastMutexIndexToBlockLock) \
    { \
        while(!m_##text##WtMessageQueues[lockedMutexIndex]->push(couchbaseRequest)) ; /* keep trying to push until success */ \
        break; \
    } \
    if(m_##text##WtMessageQueues[lockedMutexIndex]->push(couchbaseRequest)) \
    { \
        break; \
    } \
    ++lockedMutexIndex; \
    if(lockedMutexIndex == ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_##text) \
    { \
        lockedMutexIndex = 0; \
    } \
} \
event_active(m_##text##EventCallbacksForWt[lockedMutexIndex], EV_READ|EV_WRITE, 0);


#else // ABC_USE_BOOST_LOCKFREE_QUEUE

#define ABC_SERIALIZE_COUCHBASE_REQUEST_AND_SEND_TO_COUCHBASE_ON_RANDOM_MUTEX_PROTECTED_MESSAGE_QUEUE(text) \
std::ostringstream couchbaseRequestSerialized; \
{ \
    boost::archive::text_oarchive serializer(couchbaseRequestSerialized); \
    serializer << couchbaseRequest; \
} \
std::string couchbaseRequesSerializedString = couchbaseRequestSerialized.str(); \
size_t couchbaseRequesSerializedStringLength = couchbaseRequesSerializedString.length(); \
if(couchbaseRequesSerializedStringLength > ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_##text) \
{ \
    return; \
} \
++m_Current##text##MessageQueueIndex; \
if(m_Current##text##MessageQueueIndex == ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_##text) \
{ \
    m_Current##text##MessageQueueIndex = 0; \
} \
int lockedMutexIndex = m_Current##text##MessageQueueIndex; \
int veryLastMutexIndexToBlockLock = (m_Current##text##MessageQueueIndex == 0 ? (ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_##text) : (m_Current##text##MessageQueueIndex-1)); \
while(true) \
{ \
    if(lockedMutexIndex == veryLastMutexIndexToBlockLock) \
    { \
        m_##text##MutexArray[lockedMutexIndex].lock(); \
        break; \
    } \
    if(m_##text##MutexArray[lockedMutexIndex].try_lock()) \
    { \
        break; \
    } \
    ++lockedMutexIndex; \
    if(lockedMutexIndex == ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_##text) \
    { \
        lockedMutexIndex = 0; \
    } \
} \
m_##text##WtMessageQueues[lockedMutexIndex]->send(static_cast<const void*>(couchbaseRequesSerializedString.c_str()), couchbaseRequesSerializedStringLength, 0); \
event_active(m_##text##EventCallbacksForWt[lockedMutexIndex], EV_READ|EV_WRITE, 0); \
m_##text##MutexArray[lockedMutexIndex].unlock();

#endif // ABC_USE_BOOST_LOCKFREE_QUEUE

/////////////////////////////////////////////////////END MACRO HELL///////////////////////////////////////////////

class AdvertisingSellAdSpaceCreateNewAdCampaignWidget;
class AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget;
class AdvertisingBuyOwnersAdSpaceCampaignWithIndexWidget;
class LettersNumbersOnlyRegExpValidatorAndInputFilter;
class RegisterSuccessfulWidget;
class AddFundsAccountTabBody;
class NewAdSlotFillerAccountTabBody;
class ViewAllExistingAdSlotFillersAccountTabBody;

class AnonymousBitcoinComputingWtGUI : public WApplication
{
    //69'ing friend classes
    friend class AdvertisingSellAdSpaceCreateNewAdCampaignWidget;
    friend class AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget;
    friend class AddFundsAccountTabBody;
    friend class NewAdSlotFillerAccountTabBody;
    friend class ViewAllExistingAdSlotFillersAccountTabBody;

    friend class StoreCouchbaseDocumentByKeyRequest;
    friend class GetCouchbaseDocumentByKeyRequest;
    friend class ViewQueryCouchbaseDocumentByKeyRequest;

    void buildGui();
    WHBoxLayout *m_BodyHLayout;
    WVBoxLayout *m_LinksVLayout;
    WVBoxLayout *m_MainVLayout;

    WStackedWidget *m_LoginLogoutStackWidget; //2 widgets in the stack ("login" and "logout". login widget has link to register)
    WContainerWidget *m_LoginWidget; //since we don't use cookies/etc, it is IMPOSSIBLE to not have the login widget shown, so we set it up asap (constructor). Logout widget is state dependent though, so we initialize it to zero
    WLineEdit *m_LoginUsernameLineEdit;
    WLineEdit *m_LoginPasswordLineEdit;
    LettersNumbersOnlyRegExpValidatorAndInputFilter *m_LettersNumbersOnlyValidatorAndInputFilter;
    WText *m_LoginStatusMessagesPlaceholder;
    WContainerWidget *m_LogoutWidget;
    WAnchor *m_LinkToAccount;

    WStackedWidget *m_MainStack;
    WContainerWidget *m_404NotFoundWidget;

    //stack items for m_MainStack follow
    WContainerWidget *m_HomeWidget;
    void showHomeWidget();

    WContainerWidget *m_AdvertisingWidget;
    void showAdvertisingWidget();

    WContainerWidget *m_AdvertisingBuyAdSpaceWidget;
    void showAdvertisingBuyAdSpaceWidget();

#ifdef ABC_MULTI_CAMPAIGN_OWNER_MODE
    WContainerWidget *m_AdvertisingSellAdSpaceWidget;
    void showAdvertisingSellAdSpaceWidget();

    AdvertisingSellAdSpaceCreateNewAdCampaignWidget *m_AdvertisingSellAdSpaceCreateNewAdCampaignWidget;
    void showAdvertisingSellAdSpaceCreateNewAdCampaignWidget();
#endif // ABC_MULTI_CAMPAIGN_OWNER_MODE

    void showAccountWidget();
    void handleAccountTabWidgetTabChanged();
    ActualLazyLoadedTabWidget *m_AccountTabWidget;
    AddFundsAccountTabBody *m_AddFundsAccountTab;
    NewAdSlotFillerAccountTabBody *m_NewAdSlotFillerAccountTab;
    ViewAllExistingAdSlotFillersAccountTabBody *m_ViewAllExistingAdSlotFillersAccountTab;
    WScrollArea *m_AccountWidgetScrollArea;
    WContainerWidget *m_AccountWidget;

    WContainerWidget *m_AuthenticationRequiredWidget;
    void showAuthenticationRequiredWidget();

    WContainerWidget *m_RegisterWidget;
    WLineEdit *m_RegisterUsernameLineEdit;
    WLineEdit *m_RegisterPasswordLineEdit;
    WText *m_RegisterStatusMessagesPlaceholder;
    void showRegisterWidget();
    void registerAttemptFinished(bool lcbOpSuccess, bool dbError);
    RegisterSuccessfulWidget *m_RegisterSuccessfulWidget;

    void showAllUsersWithAtLeastOneAdCampaignWidget();
    AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget *m_AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget;

    //hardcoded (not as hardcoded as one might think! un-hardcoding wip if ABC_MULTI_CAMPAIGN_OWNER_MODE is defined)
    bool m_BuyInProgress;
    WContainerWidget *m_AdvertisingBuyAdSpaceD3faultWidget;
    void showAdvertisingBuyAdSpaceD3faultWidget();
    AdvertisingBuyOwnersAdSpaceCampaignWithIndexWidget *m_AdvertisingBuyAdSpaceD3faultCampaign0Widget;
    bool m_FirstPopulate;
    WText *m_CampaignLengthHoursLabel;
    WText *m_CampaignSlotCurrentlyForSaleStartDateTimeLabel;
    WText *m_CurrentPriceLabel;
    std::string m_CurrentPriceDomPath;
    WPushButton *m_BuySlotFillerStep1Button;

#ifdef ABC_MULTI_CAMPAIGN_OWNER_MODE
    void beginShowingAdvertisingBuyAdSpaceD3faultCampaign0Widget(const std::string &campaignOwner, const string &campaignIndex);
#else
    void beginShowingAdvertisingBuyAdSpaceD3faultCampaign0Widget();
#endif

    std::string m_HackedInD3faultCampaign0JsonDocForUpdatingLaterAfterSuccessfulPurchase;
    u_int64_t m_HackedInD3faultCampaign0CasForSafelyUpdatingCampaignDocLaterAfterSuccessfulPurchase;
    std::string m_HackedInD3faultCampaign0_MinPrice;
    std::string m_HackedInD3faultCampaign0_SlotLengthHours;
    bool m_HackedInD3faultCampaign0_NoPreviousSlotPurchases;
    std::string m_SlotIndexImmediatelyAfterBuyStep1wasPressed_aka_PreviousSlotIndexToTheOneTheyWantToBuy;
    std::string m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedSlotIndex;
    std::string m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedPurchaseTimestamp;
    std::string m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedStartTimestamp;
    std::string m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedPurchasePrice;
    double calculateCurrentPrice(double currentTime_x, double minPrice_y2, double lastSlotFilledAkaPurchasedPurchasePriceDoubled_y1, double lastSlotFilledAkaPurchasedExpireDateTime_x2, double lastSlotFilledAkaPurchasedPurchaseDateTime_x1);

#ifdef ABC_MULTI_CAMPAIGN_OWNER_MODE
    std::string m_CampaignOwnerForPageBeingViewed;
    std::string m_CampaignIndexForPageBeingViewed;
#endif // ABC_MULTI_CAMPAIGN_OWNER_MODE

    void finishShowingAdvertisingBuyAdSpaceD3faultCampaign0Widget(const std::string &advertisingBuyAdSpaceD3faultCampaign0JsonDocument, u_int64_t casForSafelyUpdatingCampaignDocAfterSuccesfulPurchase, bool lcbOpSuccess, bool dbError);
    void buySlotStep1d3faultCampaign0ButtonClicked();
    void ehhGetLatestValuesFromCampaignDocForNoJsUserWhichMayNotHaveEvenChangedBecauseTheyJustClickedBuyStep1(const std::string &advertisingBuyAdSpaceD3faultCampaign0JsonDocument, u_int64_t casForSafelyUpdatingCampaignDocAfterSuccesfulPurchase, bool lcbOpSuccess, bool dbError);
    void buySlotPopulateStep2d3faultCampaign0(const string &allSlotFillersJsonDoc, bool lcbOpSuccess, bool dbError);
    void ensureSlotDoesntExistThenContinueWithLockingUserAccountIntoAttemptingToBuyItAndThenGoAheadWithBuyEtcIfThatSucceeds(bool lcbOpSuccessAkaWhetherOrNotTheSlotWeWantToBuyExistsBitch, bool dbError);
    void buySlotStep2d3faultCampaign0ButtonClicked();
    void rollBackToBeforeBuyStep1ifNeeded();
    WContainerWidget *m_BuyStep2placeholder;
    int m_AllSlotFillersAdsCount;
    WComboBox *m_AllSlotFillersComboBox; //TODOoptimization: meh slot buying page needs to break out to it's own object methinks... fuck it for now
    std::string m_SlotFillerToUseInBuy;
    void verifyUserHasSufficientFundsAndThatTheirAccountIsntAlreadyLockedAndThenStartTryingToLockItIfItIsntAlreadyLocked(const string &userAccountJsonDoc, u_int64_t cas, bool lcbOpSuccess, bool dbError);
    void continueRecoveringLockedAccountAtLoginAttempt_OrAnalyzingSlotBuyBeatToThePunch(const string &maybeExistentSlot, bool lcbOpSuccess, bool dbError);
    void proceedToBitcoinKeySetNgettingAfterLockingUserAccountInto_GetAkeyFromPageYofSetNusingUuidPerKeyRequest_UnlessUserAccountAlreadyLocked();
    bool m_HackedInD3faultCampaign0_LastSlotPurchasesIsExpired;
    SatoshiInt m_CurrentPriceInSatoshisToUseForBuying;
    std::string m_LastSlotFilledAkaPurchasedExpireDateTime_ToBeUsedAsStartDateTimeIfTheBuySucceeds;
    std::string m_CurrentPriceInSatoshisToUseForBuyingString;
    std::string m_AdSlotIndexToBeFilledIfLockIsSuccessful_AndForUseInUpdateCampaignDocAfterPurchase;
    std::string m_AdSlotAboutToBeFilledIfLockIsSuccessful;
    void userAccountLockAttemptFinish_IfOkayDoTheActualSlotFillAdd(u_int64_t casFromLockSoWeCanSafelyUnlockLater, bool lcbOpSuccess, bool dbError);
    std::string m_StartTimestampUsedInNewPurchase;
    u_int64_t m_CasFromUserAccountLockSoWeCanSafelyUnlockLater;
    std::string m_PurchaseTimestampForUseInSlotItselfAndAlsoUpdatingCampaignDocAfterPurchase;
    std::string m_UserAccountUnlockedJustBeforeBuyingJson;
    u_int64_t m_UserAccountUnlockedJustBeforeBuyingCas;
    void slotFillAkaPurchaseAddAttemptFinished(bool lcbOpSuccess, bool dbError);
    void transactionDocCreatedSoCasSwapUnlockAcceptingFailUserAccountDebitting(bool dbError);
    void doneUnlockingUserAccountAfterSuccessfulPurchaseSoNowUpdateCampaignDocCasSwapAcceptingFail_SettingOurPurchaseAsLastPurchase(bool dbError);
    void doneUpdatingCampaignDocSoErrYeaTellUserWeAreCompletelyDoneWithTheSlotFillAkaPurchase(bool dbError);
    void doneAttemptingUserAccountUnlockingWithoutDebittingFor_LoginRecovery_And_SlotPurchaseBeatToThePunch(bool lcbOpSuccess, bool dbError);
    void doHackyOneTimeBuyEventUpdateThingoForNoJavascriptUser();

    //store
    void storeWithoutInputCasCouchbaseDocumentByKeyFinished(const std::string &keyToCouchbaseDocument, bool lcbOpSuccess, bool dbError);
    void storeCouchbaseDocumentByKeyWithInputCasFinished(const std::string &keyToCouchbaseDocument, bool lcbOpSuccess, bool dbError);
    void storeCouchbaseDocumentByKeyWithInputCasSavingOutputCasFinished(const std::string &keyToCouchbaseDocument, u_int64_t outputCas, bool lcbOpSuccess, bool dbError);
    void store_ADDbyDefault_WithoutInputCasCouchbaseDocumentByKeyBegin(const std::string &keyToCouchbaseDocument, const std::string &couchbaseDocument, StoreCouchbaseDocumentByKeyRequest::LcbStoreMode_AndWhetherOrNotThereIsInputCasEnum storeMode = StoreCouchbaseDocumentByKeyRequest::AddMode);
    void store_SETonly_CouchbaseDocumentByKeyWithInputCasBegin(const std::string &keyToCouchbaseDocument, const std::string &couchbaseDocument, u_int64_t cas, StoreCouchbaseDocumentByKeyRequest::WhatToDoWithOutputCasEnum whatToDoWithOutputCasEnum);
    void storeLarge_ADDbyDefault_WithoutInputCasCouchbaseDocumentByKeyBegin(const std::string &keyToCouchbaseDocument, const std::string &couchbaseDocument, StoreCouchbaseDocumentByKeyRequest::LcbStoreMode_AndWhetherOrNotThereIsInputCasEnum storeMode = StoreCouchbaseDocumentByKeyRequest::AddMode);
    void storeLarge_SETonly_CouchbaseDocumentByKeyWithInputCasBegin(const std::string &keyToCouchbaseDocument, const std::string &couchbaseDocument, u_int64_t cas, StoreCouchbaseDocumentByKeyRequest::WhatToDoWithOutputCasEnum whatToDoWithOutputCasEnum);

    //get
    void getCouchbaseDocumentByKeyBegin(const std::string &keyToCouchbaseDocument);
    void getCouchbaseDocumentByKeySavingCasBegin(const std::string &keyToCouchbaseDocument);
    void getCouchbaseDocumentByKeyFinished(const std::string &keyToCouchbaseDocument, const std::string &couchbaseDocument, bool lcbOpSuccess, bool dbError);
    void getCouchbaseDocumentByKeySavingCasFinished(const std::string &keyToCouchbaseDocument, const std::string &couchbaseDocument, u_int64_t cas, bool lcbOpSuccess, bool dbError);

    //get and subscribe
    void getAndSubscribeCouchbaseDocumentByKeySavingCas(const std::string &keyToCouchbaseDocument, GetCouchbaseDocumentByKeyRequest::GetAndSubscribeEnum subscribeMode = GetCouchbaseDocumentByKeyRequest::GetAndSubscribeMode);
    void getAndSubscribeCouchbaseDocumentByKeySavingCas_UPDATE(const std::string &keyToCouchbaseDocument, const std::string &couchbaseDocument, u_int64_t cas, bool lcbOpSuccess, bool dbError);

    //views (as in couchbase views. map/reduce 'views', not SHIT YOU LOOK AT views. guh shit naming)
    void queryCouchbaseViewBegin(const std::string &viewPathWithoutAnyParams, int pageNum_MustBeGreaterThanOrEqualToOne);
    void queryCouchbaseViewFinished(ViewQueryPageContentsType *pageContents, bool internalServerErrorOrJsonError);

    bool isHomePath(const std::string &pathToCheck);
    void handleInternalPathChanged(const std::string &newInternalPath);
    void handleRegisterButtonClicked();
    void handleLoginButtonClicked();
    std::string m_AccountLockedRecoveryWhatTheUserWasTryingToFillTheSlotWithHack;
    WText *m_CurrentlyLoggedInUsersBalanceForDisplayOnlyLabel;
    std::string m_CurrentlyLoggedInUsersBalanceStringForDisplayingOnly;
    void loginIfInputHashedEqualsDbInfo(const std::string &userProfileCouchbaseDocAsJson, u_int64_t casOnlyUsedWhenDoingRecoveryAtLogin, bool lcbOpSuccess, bool dbError);
    void unlockUserAccountWithoutDebittingIfSlotDeclaredAttemptingToPurchaseIsPurchasedBySomeoneElse(const std::string &slotLockedUserAccountIsDeclaredAttemptingToFill, const std::string &slotFillerLockedUserAccountIsDeclaredAttemptingToFillSlotWith, const std::string &lockedUserAccountJson, u_int64_t casOfLockedUserAccount);
    void usernameOrPasswordYouProvidedIsIncorrect();
    std::string m_UserAccountLockedJsonToMaybeUseInAccountRecoveryAtLogin;
    u_int64_t m_CasFromUserAccountLockedAndStuckLockedButErrRecordedDuringRecoveryProcessAfterLoginOrSomethingLoLWutIamHighButActuallyNotNeedMoneyToGetHighGuhLifeLoLSoErrLemmeTellYouAboutMyDay;
    void doLoginTasks();
    void handleLogoutButtonClicked();

    //int m_CurrentStoreMessageQueueIndex;
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_MACRO, ABC_WT_PER_QUEUE_CURRENT_RANDOM_INDEX_DECLARATION_MACRO)

    enum WhatTheStoreWithoutInputCasWasForEnum
    {
        INITIALINVALIDNULLSTOREWITHOUTINPUTCAS,
        BITCOINKEYSETNPAGEYCREATIONVIALCBADD,
        REGISTERATTEMPTSTOREWITHOUTINPUTCAS,
        LARGE_ADIMAGEUPLOADBUYERSETTINGUPADSLOTFILLERFORUSEINPURCHASINGLATERONSTOREWITHOUTINPUTCAS,
        BUYAKAFILLSLOTWITHSLOTFILLERSTOREWITHOUTINPUTCAS,
        CREATETRANSACTIONDOCSTOREWITHOUTINPUTCAS
#ifdef ABC_MULTI_CAMPAIGN_OWNER_MODE
        , ATTEMPTTOADDCAMPAIGNATINDEX
        , ADDCAMPAIGNINDEXCACHEIGNORINGRESPONSE
#endif

    };
    enum WhatTheStoreWithInputCasWasForEnum
    {
        INITIALINVALIDNULLSTOREWITHCAS,
        UNLOCKUSERACCOUNTFROMBITCOINGETTINGKEYTOBITCOINHAVEKEY,
        CLAIMBITCOINKEYONBITCOINKEYSETVIACASSWAP,
        HUGEBITCOINKEYLISTKEYRANGECLAIMATTEMPT,
        SAFELYUNLOCKBITCOINKEYSETNCURRENTPAGEDOCFAILINGTOLERABLY,
        HUGEBITCOINKEYLISTPAGECHANGE,
        RELOCKBITCOINKEYSETN_CURRENTPAGETONONEXISTENTPAGEZASNECESSARYOPTIMIZATION,
        CREDITCONFIRMEDBITCOINBALANCEFORCURRENTPAYMENTKEYCASSWAPUSERACCOUNT,
        UPDATEALLADSLOTFILLERSDOCSINCEWEJUSTCREATEDNEWADSLOTFILLER,
        HACKEDIND3FAULTCAMPAIGN0BUYPURCHASSUCCESSFULSOUNLOCKUSERACCOUNTSAFELYUSINGCAS,
        HACKEDIND3FAULTCAMPAIGN0USERACCOUNTUNLOCKDONESOUPDATECAMPAIGNDOCSETWITHINPUTCAS,
        LOGINACCOUNTRECOVERY_AND_SLOTPURCHASEBEATTOTHEPUNCH_UNLOCKINGWITHOUTDEBITTINGUSERACCOUNT
#ifdef ABC_MULTI_CAMPAIGN_OWNER_MODE
        , CASSWAPCAMPAIGNINDEXCACHEIGNORINGRESPONSE
#endif
    };
    enum WhatTheStoreWithInputCasSavingOutputCasWasForEnum
    {
        INITIALINVALIDNULLSTOREWITHCASSAVINGCAS,
        USERACCOUNTBITCOINLOCKEDINTOGETTINGKEYMODE,
        LOCKINGBITCOINKEYSETNINTOFILLINGNEXTPAGEMODE,
        HACKEDIND3FAULTCAMPAIGN0BUYSTEP2bLOCKACCOUNTFORBUYINGSETWITHCASSAVINGCAS
    };
    enum WhatTheGetWasForEnum
    {
        INITIALINVALIDNULLGET,
        GETNICKNAMEOFADSLOTFILLERNOTINALLADSLOTFILLERSDOCFORADDINGITTOIT_THEN_TRYADDINGTONEXTSLOTFILLERINDEXPLZ,
        HACKEDIND3FAULTCAMPAIGN0BUYSTEP1GET,
        ENSURESLOTABOUTTOLOCKUSERACCOUNTTOWARDSATTEMPTINGTOPURCHASEDOESNTEXISTBECAUSEIFITDOESANDWEBOUGHTITSTATEWOULDBEFUCKED,
        GETALLADSLOTFILLERSDOCFORVIEWING,
        HACKYMULTIGETAPAGEWORTHOFADSLOTFILLERS,
        ONLOGINACCOUNTLOCKEDRECOVERY_AND_SLOTPURCHASEBEATTOTHEPUNCH_DOESSLOTEXISTCHECK
    };
    enum WhatTheGetSavingCasWasForEnum
    {
        INITIALINVALIDNULLGETSAVINGCAS,
        LOGINATTEMPTGET,
        DETERMINEBITCOINSTATEBUTDONTLETTHEMPROCEEDIFLOCKEDATTEMPTINGTOFILLAKAPURCHASESLOT,
        GETBITCOINKEYSETNCURRENTPAGETOSEEWHATPAGEITISONANDIFITISLOCKED,
        GETBITCOINKEYSETNACTUALPAGETOSEEIFUUIDONITENOUGHROOM,
        GETUSERACCOUNTFORGOINGINTOGETTINGBITCOINKEYMODE,
        GETBITCOINKEYSETNPAGEYANDIFITEXISTSLOOPAROUNDCHECKINGUUIDBUTIFNOTEXISTMAKEITEXISTBITCH,
        GETHUGEBITCOINKEYLISTCURRENTPAGE,
        GETHUGEBITCOINKEYLISTACTUALPAGEFORANALYZINGANDMAYBECLAIMINGKEYRANGE,
        CREDITCONFIRMEDBITCOINAMOUNTAFTERANALYZINGUSERACCOUNT,
        GETALLADSLOTFILLERSTODETERMINENEXTINDEXANDTOUPDATEITAFTERADDINGAFILLERGETSAVINGCAS,
        //HACKEDIND3FAULTCAMPAIGN0GET,
        HACKEDIND3FAULTCAMPAIGN0BUYSTEP2aVERIFYBALANCEANDGETCASFORSWAPLOCKGET
#ifdef ABC_MULTI_CAMPAIGN_OWNER_MODE
        , GETADSPACECAMPAIGNINDEXCACHEEVENTHOUGHITMIGHTNOTEXIST
#endif
    };
    enum CurrentGetAndSubscribeMode
    {
        INITIALINVALIDNULLNOTSUBSCRIBEDTOANYTHING = 0,
        HACKEDIND3FAULTCAMPAIGN0GETANDSUBSCRIBESAVINGCAS = 1, //this used to be for d3fault campaign 0 only, but now it's for all campaigns
        SINGLESUBSCRIPTIONUPDATEFORNOJAVASCRIPTUSERSHACKPLXTHX = 2, //no-js equivalent of above subscription (hack)
        NOJSNEEDSTOVERIFYCAMPAIGNDOCSHITAFTERBUYSTEP1CLICKEDDOESNTNEEDTOBEENTIRELYACCURATEBUTISDUMBNOTTOCHECK = 3 //another use of no-js hack, except this time we are getting campaign doc for 'just after buy step 1 clicked', because otherwise we can/do show stale pricing (despite 'recalculating', the values themselves we calculate from could be stale)
    };
    enum WhatTheQueryCouchbaseViewWasForEnum
    {
        INITIALINVALIDNULLQUERYCOUCHBASEVIEW,
        ALLUSERSWITHATLEASTONEADCAMPAIGNQUERYCOUCHBASEVIEW
    };

    //TODOoptimziation: can probably use callbacks (boost::bind comes to mind) for these and would maybe be more efficient (idk)
    //TODOoptional: might make sense to set them back to null after using, but as of writing i don't need to
    WhatTheStoreWithoutInputCasWasForEnum m_WhatTheStoreWithoutInputCasWasFor;
    WhatTheStoreWithInputCasWasForEnum m_WhatTheStoreWithInputCasWasFor;
    WhatTheStoreWithInputCasSavingOutputCasWasForEnum m_WhatTheStoreWithInputCasSavingOutputCasWasFor;
    WhatTheGetWasForEnum m_WhatTheGetWasFor;
    WhatTheGetSavingCasWasForEnum m_WhatTheGetSavingCasWasFor;
    std::pair<CurrentGetAndSubscribeMode, std::string /*keyToDocCurrentlySubscribedTo*/> m_CurrentlySubscribedTo;
    WhatTheQueryCouchbaseViewWasForEnum m_WhatTheQueryCouchbaseViewWasFor;

    bool m_LoggedIn;
    std::string m_CurrentlyLoggedInUsername; //only valid if logged in

    void show404notFoundWidget();
public:
    AnonymousBitcoinComputingWtGUI(const WEnvironment &myEnv);
    virtual void finalize();

    static void newAndOpenAllWtMessageQueues();
    static void deleteAllWtMessageQueues();

    //static message_queue *m_StoreWtMessageQueues[ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_Store];
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_MACRO, ABC_WT_STATIC_MESSAGE_QUEUE_HEADER_DECLARATION_MACRO)

    //static event *m_StoreEventCallbacksForWt[ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_Store];
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_MACRO, ABC_WT_STATIC_EVENT_HEADER_DECLARATION_MACRO)

    //static boost::mutex m_StoreMutexArray[ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_Store];
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_MACRO, ABC_WT_STATIC_MUTEX_HEADER_DECLARATION_MACRO)
};

#endif // ANONYMOUSBITCOINCOMPUTINGWTGUI_H
