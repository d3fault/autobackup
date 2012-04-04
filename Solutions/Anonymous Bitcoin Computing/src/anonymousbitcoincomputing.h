#ifndef ANONYMOUSBITCOINCOMPUTING_H
#define ANONYMOUSBITCOINCOMPUTING_H

#define ANONYMOUS_BITCOIN_COMPUTING_BASE_TITLE "Anonymous Bitcoin Computing"

#define ANONYMOUS_BITCOIN_COMPUTING_MENU_SUB_ITEM_PREFIX "--"

#define ANONYMOUS_BITCOIN_COMPUTING_BALANCE_TEXT "Balance"
#define ANONYMOUS_BITCOIN_COMPUTING_BALANCE_ADD_FUNDS_TEXT "Add Funds"
#define ANONYMOUS_BITCOIN_COMPUTING_BALANCE_REQUEST_PAYOUT_TEXT "Request Payout"

#define ANONYMOUS_BITCOIN_COMPUTING_ADVERTISING_TEXT "Advertising"
#define ANONYMOUS_BITCOIN_COMPUTING_ADVERTISING_SELL_AD_SPACE_TEXT "Sell Ad Space"
#define ANONYMOUS_BITCOIN_COMPUTING_ADVERTISING_BUY_AD_SPACE_TEXT "Buy Ad Space"

#include <map>

#include <Wt/WApplication>
#include <Wt/WStackedWidget>
#include <Wt/WMenu>
#include <Wt/WMenuItem>
#include <Wt/WSubMenuItem>
#include <Wt/WWidget>
#include <Wt/WHBoxLayout>
#include <Wt/WAnimation>
#include <Wt/WText>
using namespace Wt;

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
    void buildMenu();
    WContainerWidget *getView(AbcViews view);
    WContainerWidget *createView(AbcViews view);
    WWidget *buildBalanceWWidget();
    WWidget *buildAdvertisingWWidget();
    void zeroOutEachViewPointer();
    void menuItemChanged(WMenuItem *menuItem);
    void setVisibilityForAllSubMenuItems(WSubMenuItem *subMenuItem, bool visible);
};

#endif // ANONYMOUSBITCOINCOMPUTING_H
