#ifndef ABC2WITHDRAWREQUESTPROCESSOR_H
#define ABC2WITHDRAWREQUESTPROCESSOR_H

#include <QObject>
#include "isynchronouslibcouchbaseuser.h"

#include <boost/property_tree/ptree.hpp>

#include <QStringList>
#include <QSet>

#include "abc2couchbaseandjsonkeydefines.h"

using namespace boost::property_tree;

class QNetworkAccessManager;
class QNetworkReply;

class Abc2WithdrawRequestProcessor : public QObject, public ISynchronousLibCouchbaseUser
{
    Q_OBJECT
public:
    enum Abc2WithdrawRequestProcessorModeEnum
    {
          CalculateMode = 0
        , ExecuteMode = 1
    };
    enum WayToHandleBitcoindCommunicationsErrorEnum
    {
          RevertWithdrawalRequestStateToUnprocessedAndContinueProcessingWithdrawalRequests = 1
        , RevertWithdrawalRequestStateToUnprocessedAndStopProcessingWithdrawalRequests = 2
        , SetWithdrawalRequestStateToBitcoindReturnedErrorAndContinueProcessingWithdrawalRequests = 3
        , SetWithdrawalRequestStateToBitcoindReturnedErrorAndStopProcessingWithdrawalRequests = 4
    };
    explicit Abc2WithdrawRequestProcessor(QObject *parent = 0);
private:
    Abc2WithdrawRequestProcessorModeEnum m_Mode;

    //CALCULATE mode members begin
    SatoshiInt m_RunningTotalOfAllWithdrawalRequestsInSatoshis;
    QHash<QString /*username*/, SatoshiInt /*rolling debitting balance*/> m_UserBalancesInSatoshisDuringCalculationMode;
    bool debitCurrentWithdrawalRequestFromUsersCalculatedRollingBalance_AndReturnTrueIfWeHaveCalculatedTheyHaveSufficientFundsForThisWithdrawalRequest();
    //CALCULATE mode members end

    bool m_ViewQueryHadError;
    ptree m_ViewQueryPropertyTree;
    QStringList m_ViewQueryResultsAsStringListBecauseEasierForMeToIterate;
    int m_CurrentIndexIntoViewQueryResultsStringList;
    int m_ViewQueryResultsStringListSize;
    QSet<QString> m_UsersWithProfilesFoundLocked;
    std::string m_CurrentKeyToNotDoneWithdrawalRequest;
    std::string m_CurrentUserRequestingWithdrawal;
    ptree m_CurrentWithdrawalRequestPropertyTree;
    ptree m_CurrentUserProfilePropertyTree;
    SatoshiInt m_CurrentUserBalanceInSatoshis;
    SatoshiInt m_CurrentWithdrawRequestDesiredAmountToWithdrawInSatoshis;
    //SatoshiInt m_CurrentWithdrawRequestTotalAmountToWithdrawIncludingWithdrawalFeeInSatoshis;
    lcb_cas_t m_CurrentUserProfileInLockedWithdrawingStateCas;
    lcb_cas_t m_CurrentWithdrawalRequestInProcessingStateCas;
    QString m_CurrentBitcoinCommand;
    QNetworkAccessManager *m_NetworkAccessManager;
    lcb_cas_t m_WithdrawRequestInProcessedButProfileNeedsDeductingAndUnlockingCas;

    void errorOutput(const string &errorString);

    static void viewQueryCompleteCallbackStatic(lcb_http_request_t request, lcb_t instance, const void *cookie, lcb_error_t error, const lcb_http_resp_t *resp);
    void viewQueryCompleteCallback(lcb_error_t error, const lcb_http_resp_t *resp);

    void processNextWithdrawalRequestOrEmitFinishedIfNoMore();
    void processWithdrawalRequest(const QString &currentKeyToNotDoneWithdrawalRequest);
    void readInUserBalanceAndCalculateWithdrawalFeesEtc();
    bool revertCurrentWithdrawalRequestStateToUnprocessed__AndUnlockWithoutDebittingUserProfile();
    bool setCurrentWithdrawalRequestToStateBitcoindReturnedError_Done__AndUnlockWithoutDebittingUserProfile(const QString &bitcoinDcommunicationsErrorToStoreInDb);
    bool unlockUserProfileWithoutDebitting();
    //careful changing any body of methods below this point, see the "NOTE:" in the source file
    void continueAt_deductAmountFromCurrentUserProfileBalanceAndCasSwapUnlockIt_StageOfWithdrawRequestProcessor();
    void continueAt_setWithdrawalRequestStateToProcessedAndDone_StageOfWithdrawRequestProcessor();
signals:
    void e(const QString &msg);
    void o(const QString &msg);
    void calculationIterationComplete_SoWaitForUserInputBeforeContinuingOntoExecutionIteration(const QString &calcuatedAmount);
    void bitcoindCommunicationsErrorDetectedSoWeNeedToAskTheUserHowToProceed(const QString &sourceOfError, const QString &theErrorItself);
    void withdrawalRequestProcessingFinished(bool success);
public slots:
    void processWithdrawalRequests();
    void proceedOntoExecutionIteration();
    void userWantsUsToHandleTheBitcoindCommunicationsErrorThisWay(Abc2WithdrawRequestProcessor::WayToHandleBitcoindCommunicationsErrorEnum userSelectedWayToHandleBitcoindCommunicationsError, const QString &errorStringToStoreInDb);
private slots:
    void handleNetworkReplyFinished(QNetworkReply *reply);
};
Q_DECLARE_METATYPE(Abc2WithdrawRequestProcessor::WayToHandleBitcoindCommunicationsErrorEnum)

#endif // ABC2WITHDRAWREQUESTPROCESSOR_H
