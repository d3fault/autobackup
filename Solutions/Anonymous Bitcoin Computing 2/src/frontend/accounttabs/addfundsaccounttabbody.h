#ifndef ADDFUNDSACCOUNTTABBODY_H
#define ADDFUNDSACCOUNTTABBODY_H

#include <Wt/WContainerWidget>
#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WVBoxLayout>
#include <Wt/Http/Client>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "iaccounttabwidgettabbody.h"

using namespace std;
using namespace Wt;

class AddFundsAccountTabBody : public IAccountTabWidgetTabBody
{
public:
    AddFundsAccountTabBody(AnonymousBitcoinComputingWtGUI *abcApp);
private:
    friend class AnonymousBitcoinComputingWtGUI;

    std::string m_UserAccountJsonForLockingIntoGettingBitcoinKey;
    u_int64_t m_UserAccountCASforBitcoinGettingKeyLockingAndUnlocking; //used both to lock the account into 'getting key', and to unlock it from that
    bool m_UserAccountBitcoinDoingGettingKeyRecoverySoAccountLocked;
    std::string m_BitcoinKeySetIndex_aka_setN;
    std::string m_BitcoinKeySetPage_aka_PageY;
    std::string m_PerGetBitcoinKeyUUID;
    bool m_BitcoinAddressBalancePollerPollingPendingBalance;
    Wt::Http::Client *m_BitcoinAddressBalancePoller;
    std::string m_FillingNextBitcoinKeySetPerFillUuid_ForAfterCASswapLockSucceeds;
    std::string m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList;
    std::string m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList_NON_CHANGING;
    u_int64_t m_BitcoinKeySetCurrentPageCASForFillingNextPageWayLater;
    bool m_PageZisFromCurrentPageInDbSoCasSwapIncremementCurrentPageWhenIncrementingPageZ_AsOpposedToPageZbeingSeenFromLockedRecoveryInWhichCaseDont;
    u_int64_t m_CasToUseForSafelyUpdatingHugeBitcoinKeyList_CurrentPageUsingIncrementedPageZ;
    std::string m_CurrentBitcoinKeyForPayments;
    std::string m_BitcoinKeyToGiveToUserOncePerKeyRequestUuidIsOnABitcoinKeySetPage;
    WText *m_PendingBitcoinBalanceLabel;
    WText *m_ConfirmedBitcoinBalanceLabel;
    double m_ConfirmedBitcoinBalanceToBeCredittedWhenDoneButtonClicked;
    vector<string> m_BitcoinKeysVectorToUseForNextPageFillOncePerFillUuidIsSeenOnHugeBitcoinKeyList; //i think this might be the first time i've used an std::vector xD

    void populateAndInitialize();
    //WVBoxLayout *m_AddFundsPlaceholderLayout;
    void handleGetBitcoinKeyButtonClicked();
    void checkNotAttemptingToFillAkaPurchaseSlotThenTransitionIntoGettingBitcoinKeyState(const string &userAccountDoc, u_int64_t cas, bool lcbOpSuccess, bool dbError);
    void analyzeBitcoinKeySetN_CurrentPageDocToSeeWhatPageItIsOnAndIfItIsLocked(const string &bitcoinKeySetCurrentPageDoc, u_int64_t casOnlyForUseInFillingNextPageWayLaterOn, bool lcbOpSuccess, bool dbError);
    void proceedToBitcoinKeySetNgettingAfterLockingUserAccountInto_GetAkeyFromPageYofSetNusingUuidPerKeyRequest_UnlessUserAccountAlreadyLocked();
    void userAccountBitcoinLockedIntoGettingKeyAttemptComplete(u_int64_t casFromLockSoWeCanSafelyUnlockLater, bool lcbOpSuccess, bool dbError);
    void userAccountBitcoinGettingKeyLocked_So_GetSavingCASbitcoinKeySetNPageY();
    void doneAttemptingToUpdateBitcoinKeySetViaCASswapAkaClaimingAKey(bool lcbOpSuccess, bool dbError);
    void unlockUserAccountSafelyFromBitcoinGettingKeyBecauseShitWeGotAfuckingKey_NoSweatIfBeatenToIt();
    void doneAttemptingToUnlockUserAccountFromBitcoinGettingKeyToBitcoinHaveKey(bool lcbOpSuccess, bool dbError);
    void presentBitcoinKeyForPaymentsToUser(const string &bitcoinKey);
    void checkForPendingBitcoinBalanceButtonClicked();
    void checkForConfirmedBitcoinBalanceButtonClicked();
    void doneSendingBitcoinsToCurrentAddressButtonClicked();
    void sendRpcJsonBalanceRequestToBitcoinD();
    void handleBitcoinAddressBalancePollerReceivedResponse(boost::system::error_code err, const Http::Message &response);
    void bitcoinKeySetN_currentPage_Locked_soDoKeyRangeClaimAndNextPageCreation();
    void uuidPerRefillIsSeenOnHugeBitcoinListSoProceedWithActualNextPageFill();
    void determineBitcoinStateButDontLetThemProceedForwardIfLockedAttemptingToFillAkaPurchaseSlot(const string &userAccountJsonDoc, u_int64_t casOnlyUsedWhenBitcoinStateIsInGettingKey_aka_lightRecovery, bool lcbOpSuccess, bool dbError);
    void gotBitcoinKeySetNpageYSoAnalyzeItForUUIDandEnoughRoomEtc(const string &bitcoinKeySetNpageY, u_int64_t bitcoinKeySetNpageY_CAS, bool lcbOpSuccess, bool dbError);
    void getBitcoinKeySetNPageYAttemptFinishedSoCheckItIfItExistsAndMakeItIfItDont(const std::string &bitcoinKeySetNpageY_orNot, u_int64_t bitcoinKeySetNpageY_CAS_orNot, bool lcbOpSuccess, bool dbError);
    void getHugeBitcoinKeyListActualPageAttemptCompleted(const std::string &hugeBitcoinKeyListActualPage, u_int64_t casForUsingInSafelyInsertingOurPerFillUuid, bool lcbOpSuccess, bool dbError);
    void creditConfirmedBitcoinAmountAfterAnalyzingUserAccount(const std::string &userAccountJsonDoc, u_int64_t userAccountCAS, bool lcbOpSuccess, bool dbError);
    void attemptToLockBitcoinKeySetNintoFillingNextPageModeComplete(u_int64_t casFromLockSoWeCanSafelyUnlockAfterNextPageFillComplete, bool lcbOpSuccess, bool dbError);
    void doneAttemptingHugeBitcoinKeyListKeyRangeClaim(bool lcbOpSuccess, bool dbError);
    void doneAttemptingToUnlockBitcoinKeySetN_CurrentPage(bool lcbOpSuccess, bool dbError);
    void doneChangingHugeBitcoinKeyListCurrentPage(bool dbError);
    void doneReLockingBitcoinKeySetN_CurrentPage_withNewFromPageZvalue(bool dbError);
    void doneAttemptingCredittingConfirmedBitcoinBalanceForCurrentPaymentKeyCasSwapUserAccount(bool lcbOpSuccess, bool dbError);
    void showOutOfBitcoinKeysErrorToUserInAddFundsPlaceholderLayout();
    void hugeBitcoinKeyListCurrentPageGetComplete(const std::string &hugeBitcoinKeyList_CurrentPageJson, u_int64_t cas, bool lcbOpSuccess, bool dbError);
    void doneAttemptingBitcoinKeySetNnextPageYcreation(bool dbError);
};

#endif // ADDFUNDSACCOUNTTABBODY_H
