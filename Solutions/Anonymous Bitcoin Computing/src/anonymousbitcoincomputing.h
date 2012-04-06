#ifndef ANONYMOUSBITCOINCOMPUTING_H
#define ANONYMOUSBITCOINCOMPUTING_H

#define ANONYMOUS_BITCOIN_COMPUTING_BASE_TITLE "Anonymous Bitcoin Computing"

#define ANONYMOUS_BITCOIN_COMPUTING_MENU_SUB_ITEM_PREFIX "--"

#define ANONYMOUS_BITCOIN_COMPUTING_BALANCE_TEXT "Balance"
#define ANONYMOUS_BITCOIN_COMPUTING_BALANCE_ADD_FUNDS_TEXT "Add Funds"
#define ANONYMOUS_BITCOIN_COMPUTING_BALANCE_REQUEST_PAYOUT_TEXT "Request Disbursement"

#define ANONYMOUS_BITCOIN_COMPUTING_ADVERTISING_TEXT "Advertising"
#define ANONYMOUS_BITCOIN_COMPUTING_ADVERTISING_SELL_AD_SPACE_TEXT "Sell Ad Space"
#define ANONYMOUS_BITCOIN_COMPUTING_ADVERTISING_BUY_AD_SPACE_TEXT "Buy Ad Space"

#include <map>

#include <Wt/WApplication>
#include <Wt/WStackedWidget>
#include <Wt/WAnchor>
#include <Wt/WWidget>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WAnimation>
#include <Wt/WText>
using namespace Wt;


#include "database.h"

#include "views/abchome.h"
#include "views/abcindex.h"
#include "views/balance/abcbalanceaddfunds.h"
#include "views/balance/abcbalancerequestpayout.h"
#include "views/advertising/abcadvertisingselladspace.h"
#include "views/advertising/abcadvertisingbuyadspace.h"

class AnonymousBitcoinComputing : public WApplication
{
public:
    AnonymousBitcoinComputing(const WEnvironment &env);
private:
    WStackedWidget *m_MainStack;
    Database m_UsernameDb;
    void buildGui();
    void handleInternalPathChanged(const std::string &newInternalPath);
    void showViewByInternalPath(const std::string &internalPath);
    WWidget *getViewOrAuthWidgetDependingOnInternalPathAndWhetherOrNotLoggedInAndIfItMattersForInternalPath(const std::string &internalPath);

    enum AbcViews
    {
        AbcHomeView,
        AbcIndexView,
        AbcBalanceSubMenuView,
        AbcBalanceAddFundsView,
        AbcBalanceRequestPayoutView,
        AbcAdvertisingSubMenuView,
        AbcAdvertisingSellAdSpaceView,
        AbcAdvertisingBuyAdSpaceView
        //todo: when adding more views, update createView
    };
    std::map<AbcViews,WContainerWidget*> m_AllViews;
    WContainerWidget *getView(AbcViews view);
    WContainerWidget *createView(AbcViews view);
};

#endif // ANONYMOUSBITCOINCOMPUTING_H
