#ifndef ABCBALANCEADDFUNDS_H
#define ABCBALANCEADDFUNDS_H

#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WLineEdit>
#include <Wt/WLabel>
#include <Wt/WBreak>
#include <Wt/WPushButton>
using namespace Wt;

class AbcBalanceAddFunds : public WContainerWidget
{
public:
    AbcBalanceAddFunds(WContainerWidget *parent = 0);
private:
    WLineEdit *m_AddFundsKeyLineEdit;
    WLabel *m_PendingPaymentAmountActual;
    WLabel *m_ConfirmedPaymentAmountActual;

    void handleAddFundsBalanceButtonClicked();
};

#endif // ABCBALANCEADDFUNDS_H
