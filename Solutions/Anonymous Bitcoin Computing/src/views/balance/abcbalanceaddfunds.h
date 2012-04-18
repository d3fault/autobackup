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

#include <QMetaObject>

#include "wtqtutil.h"
#include "btcutil.h"
#include "appdbhelper.h"

class AbcBalanceAddFunds : public WContainerWidget
{
public:
    AbcBalanceAddFunds(WContainerWidget *parent = 0);

    void notifyOfPageChange();

    static const std::string ReadableText;
    static const std::string PreferredInternalPath;

    static bool isInternalPath(const std::string &internalPath);
    static bool requiresLogin();
private:
    bool m_AwaitingValues;

    WLineEdit *m_AddFundsKeyLineEdit;
    WLabel *m_PendingPaymentAmountActual;
    WLabel *m_ConfirmedPaymentAmountActual;

    void notifyCallback(AppDbResult updateOrResult);
    void processNewValues(AppDbResult result);

    void handleAddFundsBalanceButtonClicked();
};

#endif // ABCBALANCEADDFUNDS_H
