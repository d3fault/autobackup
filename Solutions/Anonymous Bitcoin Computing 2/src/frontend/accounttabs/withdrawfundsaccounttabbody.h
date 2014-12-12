#ifndef WITHDRAWFUNDSACCOUNTTABBODY_H
#define WITHDRAWFUNDSACCOUNTTABBODY_H

#include "iaccounttabwidgettabbody.h"

using namespace Wt;

class WithdrawFundsAccountTabBody : public IAccountTabWidgetTabBody
{
public:
    WithdrawFundsAccountTabBody(AnonymousBitcoinComputingWtGUI *abcApp);
private:
    friend class AnonymousBitcoinComputingWtGUI;

    WLineEdit *m_AmountToWithdrawLineEdit;
    WLineEdit *m_TotalWithdrawAmountVisualOnlyLineEdit;
    WLineEdit *m_BitcoinPayoutKeyLineEdit;

    std::string m_WithdrawRequestIndexToTryLcbAddingAt;
    u_int64_t m_CasOfWithdrawRequestIndexDoc_OrZeroIfDocDoesNotExist;
    std::string m_WithdrawRequestDocBeingCreated;

    void populateAndInitialize();
    void calculateTotalAmountWithdrawnAfterFeeForVisual();
    void handleRequestWithdrawalButtonClicked();
    void verifyBalanceIsGreaterThanOrEqualToTheirRequestedWithdrawAmountAndThenContinueSchedulingWithdrawRequest(const std::string &userProfileDoc, bool lcbOpSuccess, bool dbError);
    void useNextAvailableWithdrawRequestIndexToScheduleTheWithdrawRequest(const std::string &withdrawRequestNextAvailalbeIndexDoc, u_int64_t cas, bool lcbOpSuccess, bool dbError);
    void attemptToLcbAddWithdrawRequest();
    void handleAttemptToAddWithdrawRequestAtIndexFinished(bool lcbOpSuccess, bool dbError);
};

#endif // WITHDRAWFUNDSACCOUNTTABBODY_H
