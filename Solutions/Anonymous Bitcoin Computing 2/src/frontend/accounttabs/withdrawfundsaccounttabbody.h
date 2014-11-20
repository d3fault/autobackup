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
    WLineEdit *m_BitcoinPayoutKeyLineEdit;

    void populateAndInitialize();
    void handleRequestWithdrawalButtonClicked();
};

#endif // WITHDRAWFUNDSACCOUNTTABBODY_H
