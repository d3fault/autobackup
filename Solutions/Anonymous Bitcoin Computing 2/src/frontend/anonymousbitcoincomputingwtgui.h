#ifndef ANONYMOUSBITCOINCOMPUTINGWTGUI_H
#define ANONYMOUSBITCOINCOMPUTINGWTGUI_H

#include <string>

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
#include <Wt/Http/Client>
#include <Wt/WDefaultLoadingIndicator>
#include <Wt/WScrollArea>
//#include <Wt/Chart/WCartesianChart>

//TODOoptimization: a compile time switch alternating between message_queue and lockfree::queue (lockfree::queue doesn't need mutexes or the try, try try, blockLock logic)
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/preprocessor/repeat.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/replace.hpp>

#include "filedeletingfileresource.h"
#include "../frontend2backendRequests/storecouchbasedocumentbykeyrequest.h"
#include "../frontend2backendRequests/getcouchbasedocumentbykeyrequest.h"

using namespace Wt;
using namespace Wt::Utils;
using namespace boost::interprocess;
using namespace boost::random;
using namespace boost::property_tree;
using namespace std;

/////////////////////////////////////////////////////BEGIN MACRO HELL///////////////////////////////////////////////

//100ms javascript interval
#define ABC_START_JS_INTERVAL_SNIPPET m_CurrentPriceDomPath + ".z0bj.tehIntervalz = setInterval(" + m_CurrentPriceDomPath + ".z0bj.tehIntervalzCallback, 100);" + m_CurrentPriceDomPath + ".z0bj.tehIntervalIsRunnan = true;"

#define ABC_WT_STATIC_MESSAGE_QUEUE_SOURCE_DEFINITION_MACRO(text) \
message_queue *AnonymousBitcoinComputingWtGUI::m_##text##WtMessageQueues[ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_##text];

#define ABC_WT_STATIC_EVENT_SOURCE_DEFINITION_MACRO(text) \
event *AnonymousBitcoinComputingWtGUI::m_##text##EventCallbacksForWt[ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_##text];

#define ABC_WT_STATIC_MUTEX_SOURCE_DEFINITION_MACRO(text) \
boost::mutex AnonymousBitcoinComputingWtGUI::m_##text##MutexArray[ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_##text];

#define ABC_WT_STATIC_MESSAGE_QUEUE_HEADER_DECLARATION_MACRO(text) \
static message_queue *m_##text##WtMessageQueues[ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_##text];

#define ABC_WT_STATIC_EVENT_HEADER_DECLARATION_MACRO(text) \
static event *m_##text##EventCallbacksForWt[ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_##text];

#define ABC_WT_STATIC_MUTEX_HEADER_DECLARATION_MACRO(text) \
static boost::mutex m_##text##MutexArray[ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_##text];

#define ABC_WT_NEW_AND_OPEN_MESSAGE_QUEUE_MACRO(z, n, text) \
m_##text##WtMessageQueues[n] = new message_queue(open_only, ABC_WT_COUCHBASE_MESSAGE_QUEUES_BASE_NAME \
#text \
#n);

#define ABC_WT_PER_QUEUE_SET_UNIFORM_INT_DISTRIBUTION_CONSTRUCTOR_INITIALIZATION_MACRO(text) \
uniform_int_distribution<> l_##text##MessageQueuesRandomIntDistribution(0, ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_##text - 1); \
m_Current##text##MessageQueueIndex = l_##text##MessageQueuesRandomIntDistribution(mersenneTwisterRandomNumberGenerator);

#define ABC_WT_PER_QUEUE_CURRENT_RANDOM_INDEX_DECLARATION_MACRO(text) \
int m_Current##text##MessageQueueIndex;

#define ABC_WT_DELETE_MESSAGE_QUEUE_MACRO(z, n, text) \
delete m_##text##WtMessageQueues[n]; \
m_##text##WtMessageQueues[n] = NULL;

#ifndef ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO
#define ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO(z, n, text) \
BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_SET##n, text, ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET##n)
#define ABC_sofdsufoMACRO_SUBSTITUTION_HACK_FUCK_EVERYTHINGisau(a,b) a(b)
#define ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_MACRO(z, n, text) \
ABC_sofdsufoMACRO_SUBSTITUTION_HACK_FUCK_EVERYTHINGisau(text,ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET##n)
#endif

//see cpp file for original code with comments
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

/////////////////////////////////////////////////////END MACRO HELL///////////////////////////////////////////////

class AnonymousBitcoinComputingWtGUI : public WApplication
{
    friend class StoreCouchbaseDocumentByKeyRequest;
    friend class GetCouchbaseDocumentByKeyRequest;

    void buildGui();
    WHBoxLayout *m_BodyHLayout;
    WVBoxLayout *m_LinksVLayout;
    WVBoxLayout *m_MainVLayout;

    WStackedWidget *m_LoginLogoutStackWidget; //2 widgets in the stack ("login" and "logout". login widget has link to register)
    WContainerWidget *m_LoginWidget; //since we don't use cookies/etc, it is IMPOSSIBLE to not have the login widget shown, so we set it up asap (constructor). Logout widget is state dependent though, so we initialize it to zero
    WLineEdit *m_LoginUsernameLineEdit;
    WLineEdit *m_LoginPasswordLineEdit;
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

    void showAccountWidget();
    WScrollArea *m_AccountWidgetScrollArea;
    WContainerWidget *m_AccountWidget;
    WContainerWidget *m_AuthenticationRequiredWidget;
    void handleAddFundsClicked();
    WVBoxLayout *m_AddFundsPlaceholderLayout;
    WText *m_PendingBitcoinBalanceLabel;
    WText *m_ConfirmedBitcoinBalanceLabel;
    double m_ConfirmedBitcoinBalanceToBeCredittedWhenDoneButtonClicked;
    WLineEdit *m_UploadNewSlotFiller_NICKNAME;
    WLineEdit *m_UploadNewSlotFiller_HOVERTEXT;
    WLineEdit *m_UploadNewSlotFiller_URL;
    WContainerWidget *m_AdImageUploaderPlaceholder;
    WFileUpload *m_AdImageUploader;
    WPushButton *m_AdImageUploadButton;
    WVBoxLayout *m_AdImageUploadResultsVLayout;
    void setUpAdImageUploaderAndPutItInPlaceholder();
    void handleAdSlotFillerSubmitButtonClickedAkaImageUploadFinished();
    void handleAdImageUploadFailedFileTooLarge();
    void resetAdSlotFillerImageUploadFieldsForAnotherUpload();
    bool m_MostRecentlyUploadedImageWasSetAsPreview;
    FileDeletingFileResource *m_MostRecentlyUploadedImageAsFileResource;

    WContainerWidget *m_RegisterWidget;
    WLineEdit *m_RegisterUsernameLineEdit;
    WLineEdit *m_RegisterPasswordLineEdit;
    void showRegisterWidget();
    void registerAttemptFinished(bool lcbOpSuccess, bool dbError);
    //WContainerWidget *m_RegisterSuccessfulWidget;

    //hardcoded
    bool m_BuyInProgress;
    WContainerWidget *m_AdvertisingBuyAdSpaceD3faultWidget;
    void showAdvertisingBuyAdSpaceD3faultWidget();
    WContainerWidget *m_AdvertisingBuyAdSpaceD3faultCampaign0Widget;
    bool m_FirstPopulate;
    WText *m_CampaignLengthHoursLabel;
    WText *m_CampaignSlotCurrentlyForSaleStartDateTimeLabel;
    WText *m_CurrentPriceLabel;
    std::string m_CurrentPriceDomPath;
    WPushButton *m_BuySlotFillerStep1Button;
    void beginShowingAdvertisingBuyAdSpaceD3faultCampaign0Widget();
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
    void finishShowingAdvertisingBuyAdSpaceD3faultCampaign0Widget(const std::string &advertisingBuyAdSpaceD3faultCampaign0JsonDocument, u_int64_t casForSafelyUpdatingCampaignDocAfterSuccesfulPurchase);
    void buySlotStep1d3faultCampaign0ButtonClicked();
    void buySlotPopulateStep2d3faultCampaign0(const string &allSlotFillersJsonDoc, bool lcbOpSuccess, bool dbError);
    void buySlotStep2d3faultCampaign0ButtonClicked();
    std::string m_UserAccountJsonForLockingIntoGettingBitcoinKey;
    u_int64_t m_UserAccountCASforBitcoinGettingKeyLockingAndUnlocking; //used both to lock the account into 'getting key', and to unlock it from that
    bool m_UserAccountBitcoinDoingGettingKeyRecoverySoAccountLocked;
    std::string m_BitcoinKeySetIndex_aka_setN;
    std::string m_BitcoinKeySetPage_aka_PageY;
    std::string m_PerGetBitcoinKeyUUID;
    void checkNotAttemptingToFillAkaPurchaseSlotThenTransitionIntoGettingBitcoinKeyState(const string &userAccountDoc, u_int64_t cas, bool lcbOpSuccess, bool dbError);
    void rollBackToBeforeBuyStep1ifNeeded();
    WContainerWidget *m_BuyStep2placeholder;
    WComboBox *m_AllSlotFillersComboBox; //TODOoptimization: meh slot buying page needs to break out to it's own object methinks... fuck it for now
    std::string m_SlotFillerToUseInBuy;
    void verifyUserHasSufficientFundsAndThatTheirAccountIsntAlreadyLockedAndThenStartTryingToLockItIfItIsntAlreadyLocked(const string &userAccountJsonDoc, u_int64_t cas, bool lcbOpSuccess, bool dbError);
    void determineBitcoinStateButDontLetThemProceedForwardIfLockedAttemptingToFillAkaPurchaseSlot(const string &userAccountJsonDoc, u_int64_t casOnlyUsedWhenBitcoinStateIsInGettingKey_aka_lightRecovery, bool lcbOpSuccess, bool dbError);
    void presentBitcoinKeyForPaymentsToUser(const std::string &bitcoinKey);
    std::string m_CurrentBitcoinKeyForPayments;
    std::string m_BitcoinKeyToGiveToUserOncePerKeyRequestUuidIsOnABitcoinKeySetPage;
    void gotBitcoinKeySetNpageYSoAnalyzeItForUUIDandEnoughRoomEtc(const std::string &bitcoinKeySetNpageY, u_int64_t bitcoinKeySetNpageY_CAS, bool lcbOpSuccess, bool dbError);
    void getBitcoinKeySetNPageYAttemptFinishedSoCheckItIfItExistsAndMakeItIfItDont(const std::string &bitcoinKeySetNpageY_orNot, u_int64_t bitcoinKeySetNpageY_CAS_orNot, bool lcbOpSuccess, bool dbError);
    void getHugeBitcoinKeyListActualPageAttemptCompleted(const std::string &hugeBitcoinKeyListActualPage, u_int64_t casForUsingInSafelyInsertingOurPerFillUuid, bool lcbOpSuccess, bool dbError);
    void creditConfirmedBitcoinAmountAfterAnalyzingUserAccount(const std::string &userAccountJsonDoc, u_int64_t userAccountCAS, bool lcbOpSuccess, bool dbError);
    vector<string> m_BitcoinKeysVectorToUseForNextPageFillOncePerFillUuidIsSeenOnHugeBitcoinKeyList; //i think this might be the first time i've used an std::vector xD
    void uuidPerRefillIsSeenOnHugeBitcoinListSoProceedWithActualNextPageFill();
    void unlockUserAccountSafelyFromBitcoinGettingKeyBecauseShitWeGotAfuckingKey_NoSweatIfBeatenToIt();
    void checkForPendingBitcoinBalanceButtonClicked();
    void checkForConfirmedBitcoinBalanceButtonClicked();
    void doneSendingBitcoinsToCurrentAddressButtonClicked();
    void sendRpcJsonBalanceRequestToBitcoinD();
    void handleBitcoinAddressBalancePollerReceivedResponse(boost::system::error_code err, const Http::Message& response);
    void handleGetBitcoinKeyButtonClicked();
    ptree m_RunningAllSlotFillersJsonDoc;
    u_int64_t m_RunningAllSlotFillersJsonDocCAS;
    void determineNextSlotFillerIndexAndThenAddSlotFillerToIt(const string &allAdSlotFillersJsonDoc, u_int64_t casOfAllSlotFillersDocForUpdatingSafely, bool lcbOpSuccess, bool dbError);
    void tryToAddAdSlotFillerToCouchbase(const std::string &slotFillerIndexToAttemptToAdd);
    void continueRecoveringLockedAccountAtLoginAttempt(const string &maybeExistentSlot, bool lcbOpSuccess, bool dbError);
    void analyzeBitcoinKeySetN_CurrentPageDocToSeeWhatPageItIsOnAndIfItIsLocked(const std::string &bitcoinKeySetCurrentPageDoc, u_int64_t casOnlyForUseInFillingNextPageWayLaterOn, bool lcbOpSuccess, bool dbError);
    u_int64_t m_BitcoinKeySetCurrentPageCASForFillingNextPageWayLater;
    void proceedToBitcoinKeySetNgettingAfterLockingUserAccountInto_GetAkeyFromPageYofSetNusingUuidPerKeyRequest_UnlessUserAccountAlreadyLocked();
    void hugeBitcoinKeyListCurrentPageGetComplete(const std::string &hugeBitcoinKeyList_CurrentPageJson, u_int64_t cas, bool lcbOpSuccess, bool dbError);
    std::string m_FillingNextBitcoinKeySetPerFillUuid_ForAfterCASswapLockSucceeds;
    std::string m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList;
    std::string m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList_NON_CHANGING;
    void getAdSlotFillerThatIsntInAllAdSlotFillersAttemptFinished_soAddItToAllAddSlotFillersAndInitiateSlotFillerAddAtNextIndex(const string &adSlotFillerToExtractNicknameFrom, bool lcbOpSuccess, bool dbError);
    bool m_HackedInD3faultCampaign0_LastSlotPurchasesIsExpired;
    double m_CurrentPriceToUseForBuying;
    std::string m_LastSlotFilledAkaPurchasedExpireDateTime_ToBeUsedAsStartDateTimeIfTheBuySucceeds;
    std::string m_CurrentPriceToUseForBuyingString;
    std::string m_AdSlotIndexToBeFilledIfLockIsSuccessful_AndForUseInUpdateCampaignDocAfterPurchase;
    std::string m_AdSlotAboutToBeFilledIfLockIsSuccessful;
    void userAccountLockAttemptFinish_IfOkayDoTheActualSlotFillAdd(u_int64_t casFromLockSoWeCanSafelyUnlockLater, bool lcbOpSuccess, bool dbError);
    void userAccountBitcoinLockedIntoGettingKeyAttemptComplete(u_int64_t casFromLockSoWeCanSafelyUnlockLater, bool lcbOpSuccess, bool dbError);
    void userAccountBitcoinGettingKeyLocked_So_GetSavingCASbitcoinKeySetNPageY();
    void attemptToLockBitcoinKeySetNintoFillingNextPageModeComplete(u_int64_t casFromLockSoWeCanSafelyUnlockAfterNextPageFillComplete, bool lcbOpSuccess, bool dbError);
    void bitcoinKeySetN_currentPage_Locked_soDoKeyRangeClaimAndNextPageCreation();
    std::string m_StartTimestampUsedInNewPurchase;
    u_int64_t m_CasFromUserAccountLockSoWeCanSafelyUnlockLater;
    std::string m_PurchaseTimestampForUseInSlotItselfAndAlsoUpdatingCampaignDocAfterPurchase;
    std::string m_UserAccountLockedDuringBuyJson;
    void slotFillAkaPurchaseAddAttemptFinished(bool lcbOpSuccess, bool dbError);
    void transactionDocCreatedSoCasSwapUnlockAcceptingFailUserAccountDebitting(bool dbError);
    void storeLargeAdImageInCouchbaseDbAttemptComplete(const string &keyToSlotFillerAttemptedToAddTo, bool lcbOpSuccess, bool dbError); //read a huge article yesterday by the soup and was almost convinced to start using exceptions for error handling.... but how the fuck do you throw an exception across a thread O_o? (RpcGenerator's error mechanism was (is?) teh pro-est)
    void doneAttemptingBitcoinKeySetNnextPageYcreation(bool dbError);
    void doneUnlockingUserAccountAfterSuccessfulPurchaseSoNowUpdateCampaignDocCasSwapAcceptingFail_SettingOurPurchaseAsLastPurchase(bool dbError);
    void doneUpdatingCampaignDocSoErrYeaTellUserWeAreCompletelyDoneWithTheSlotFillAkaPurchase(bool dbError);
    void doneAttemptingUserAccountLockedRecoveryUnlockWithoutDebitting(bool lcbOpSuccess, bool dbError);
    void doneAttemptingToUnlockUserAccountFromBitcoinGettingKeyToBitcoinHaveKey(bool lcbOpSuccess, bool dbError);
    void doneAttemptingToUpdateBitcoinKeySetViaCASswapAkaClaimingAKey(bool lcbOpSuccess, bool dbError);
    void doneAttemptingHugeBitcoinKeyListKeyRangeClaim(bool lcbOpSuccess, bool dbError);
    void doneAttemptingToUnlockBitcoinKeySetN_CurrentPage(bool lcbOpSuccess, bool dbError);
    void doneChangingHugeBitcoinKeyListCurrentPage(bool dbError);
    void doneReLockingBitcoinKeySetN_CurrentPage_withNewFromPageZvalue(bool dbError);
    void doneAttemptingCredittingConfirmedBitcoinBalanceForCurrentPaymentKeyCasSwapUserAccount(bool lcbOpSuccess, bool dbError);
    void showOutOfBitcoinKeysErrorToUserInAddFundsPlaceholderLayout();
    void doneAttemptingToUpdateAllAdSlotFillersDocSinceWeJustCreatedANewAdSlotFiller(bool lcbOpSuccess, bool dbError);
    bool m_PageZisFromCurrentPageInDbSoCasSwapIncremementCurrentPageWhenIncrementingPageZ_AsOpposedToPageZbeingSeenFromLockedRecoveryInWhichCaseDont;
    u_int64_t m_CasToUseForSafelyUpdatingHugeBitcoinKeyList_CurrentPageUsingIncrementedPageZ;

    //store
    void storeWithoutInputCasCouchbaseDocumentByKeyFinished(const std::string &keyToCouchbaseDocument, bool lcbOpSuccess, bool dbError);
    void storeCouchbaseDocumentByKeyWithInputCasFinished(const std::string &keyToCouchbaseDocument, bool lcbOpSuccess, bool dbError);
    void storeCouchbaseDocumentByKeyWithInputCasSavingOutputCasFinished(const std::string &keyToCouchbaseDocument, u_int64_t outputCas, bool lcbOpSuccess, bool dbError);
    void store_ADDbyDefault_WithoutInputCasCouchbaseDocumentByKeyBegin(const std::string &keyToCouchbaseDocument, const std::string &couchbaseDocument, StoreCouchbaseDocumentByKeyRequest::LcbStoreMode_AndWhetherOrNotThereIsInputCasEnum storeMode = StoreCouchbaseDocumentByKeyRequest::AddMode);
    void store_SETonly_CouchbaseDocumentByKeyWithInputCasBegin(const std::string &keyToCouchbaseDocument, const std::string &couchbaseDocument, u_int64_t cas, StoreCouchbaseDocumentByKeyRequest::WhatToDoWithOutputCasEnum whatToDoWithOutputCasEnum);
    void storeLarge_ADDbyDefault_WithoutInputCasCouchbaseDocumentByKeyBegin(const std::string &keyToCouchbaseDocument, const std::string &couchbaseDocument, StoreCouchbaseDocumentByKeyRequest::LcbStoreMode_AndWhetherOrNotThereIsInputCasEnum storeMode = StoreCouchbaseDocumentByKeyRequest::AddMode);

    //get
    void getCouchbaseDocumentByKeyBegin(const std::string &keyToCouchbaseDocument);
    void getCouchbaseDocumentByKeySavingCasBegin(const std::string &keyToCouchbaseDocument);
    void getCouchbaseDocumentByKeyFinished(const std::string &keyToCouchbaseDocument, const std::string &couchbaseDocument, bool lcbOpSuccess, bool dbError);
    void getCouchbaseDocumentByKeySavingCasFinished(const std::string &keyToCouchbaseDocument, const std::string &couchbaseDocument, u_int64_t cas, bool lcbOpSuccess, bool dbError);

    //get and subscribe
    void getAndSubscribeCouchbaseDocumentByKeySavingCas(const std::string &keyToCouchbaseDocument, GetCouchbaseDocumentByKeyRequest::GetAndSubscribeEnum subscribeMode = GetCouchbaseDocumentByKeyRequest::GetAndSubscribeMode);
    void getAndSubscribeCouchbaseDocumentByKeySavingCas_UPDATE(const std::string &keyToCouchbaseDocument, const std::string &couchbaseDocument, u_int64_t cas, bool lcbOpSuccess, bool dbError);

    bool isHomePath(const std::string &pathToCheck);
    void handleInternalPathChanged(const std::string &newInternalPath);
    void handleRegisterButtonClicked();
    void handleLoginButtonClicked();
    std::string m_AccountLockedRecoveryWhatTheUserWasTryingToFillTheSlotWithHack;
    void loginIfInputHashedEqualsDbInfo(const std::string &userProfileCouchbaseDocAsJson, u_int64_t casOnlyUsedWhenDoingRecoveryAtLogin, bool lcbOpSuccess, bool dbError);
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
        LOGINACCOUNTRECOVERYUNLOCKINGWITHOUTDEBITTINGUSERACCOUNT
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
        ONLOGINACCOUNTLOCKEDRECOVERYDOESSLOTEXISTCHECK
    };
    enum WhatTheGetSavingCasWasForEnum
    {
        INITIALINVALIDNULLGETSAVINGCAS,
        LOGINATTEMPTGET,
        ADDFUNDSBUTTONCLICKEDSODETERMINEBITCOINSTATEBUTDONTLETTHEMPROCEEDIFLOCKEDATTEMPTINGTOFILLAKAPURCHASESLOT,
        GETBITCOINKEYSETNCURRENTPAGETOSEEWHATPAGEITISONANDIFITISLOCKED,
        GETBITCOINKEYSETNACTUALPAGETOSEEIFUUIDONITENOUGHROOM,
        GETUSERACCOUNTFORGOINGINTOGETTINGBITCOINKEYMODE,
        GETBITCOINKEYSETNPAGEYANDIFITEXISTSLOOPAROUNDCHECKINGUUIDBUTIFNOTEXISTMAKEITEXISTBITCH,
        GETHUGEBITCOINKEYLISTCURRENTPAGE,
        GETHUGEBITCOINKEYLISTACTUALPAGEFORANALYZINGANDMAYBECLAIMINGKEYRANGE,
        CREDITCONFIRMEDBITCOINAMOUNTAFTERANALYZINGUSERACCOUNT,
        ALLADSLOTFILLERSTODETERMINENEXTINDEXANDTOUPDATEITAFTERADDINGAFILLERGETSAVINGCAS,
        HACKEDIND3FAULTCAMPAIGN0GET,
        HACKEDIND3FAULTCAMPAIGN0BUYSTEP2aVERIFYBALANCEANDGETCASFORSWAPLOCKGET
    };
    enum WhatTheGetAndSubscribeSavingCasWasForEnum
    {
        INITIALINVALIDNULLNOTSUBSCRIBEDTOANYTHING,
        HACKEDIND3FAULTCAMPAIGN0GETANDSUBSCRIBESAVINGCAS
    };

    //TODOoptimziation: can probably use callbacks (boost::bind comes to mind) for these and would maybe be more efficient (idk)
    //TODOoptional: might make sense to set them back to null after using, but as of writing i don't need to
    WhatTheStoreWithoutInputCasWasForEnum m_WhatTheStoreWithoutInputCasWasFor;
    WhatTheStoreWithInputCasWasForEnum m_WhatTheStoreWithInputCasWasFor;
    WhatTheStoreWithInputCasSavingOutputCasWasForEnum m_WhatTheStoreWithInputCasSavingOutputCasWasFor;
    WhatTheGetWasForEnum m_WhatTheGetWasFor;
    WhatTheGetSavingCasWasForEnum m_WhatTheGetSavingCasWasFor;
    WhatTheGetAndSubscribeSavingCasWasForEnum m_CurrentlySubscribedTo; //hack insted of 'bool m_CurrentlySubscribed' (which isn't future proof anyways)

    bool m_LoggedIn;
    std::string m_CurrentlyLoggedInUsername; //only valid if logged in

    bool m_BitcoinAddressBalancePollerPollingPendingBalance;
    Wt::Http::Client *m_BitcoinAddressBalancePoller;
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
