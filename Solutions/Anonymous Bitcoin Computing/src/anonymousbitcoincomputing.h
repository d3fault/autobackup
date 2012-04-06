#ifndef ANONYMOUSBITCOINCOMPUTING_H
#define ANONYMOUSBITCOINCOMPUTING_H

#define ANONYMOUS_BITCOIN_COMPUTING_BASE_TITLE "Anonymous Bitcoin Computing"

#define ANONYMOUS_BITCOIN_COMPUTING_MENU_SUB_ITEM_PREFIX "--"

#define ANONYMOUS_BITCOIN_COMPUTING_LOGOUT_INTERNAL_PATH "/logout"

#include <map>

#include <Wt/WApplication>
#include <Wt/WStackedWidget>
#include <Wt/WAnchor>
#include <Wt/WWidget>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WAnimation>
#include <Wt/WText>
#include <Wt/Auth/AuthWidget>
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
    Wt::Auth::AuthWidget *m_AuthWidget;
    Wt::Auth::AuthWidget *getAuthWidgetForStack();
    bool m_AuthWidgetIsInStack;
    WAnchor *m_LogoutAnchor;
    Database m_UsernameDb;
    void buildGui();
    void handleLoginChanged();
    void handleInternalPathChanged(const std::string &newInternalPath);
    void showViewByInternalPath(const std::string &internalPath);
    WWidget *getViewOrAuthWidgetDependingOnInternalPathAndWhetherOrNotLoggedInAndIfItMattersForInternalPath(const std::string &internalPath);
    WContainerWidget *pageNotFound();
    WContainerWidget *m_PageNotFound;

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
    typedef std::map<AbcViews,WContainerWidget*> AbcViewMap;
    AbcViewMap m_AllViews;
    WContainerWidget *getView(AbcViews view);
    WContainerWidget *createView(AbcViews view);
};

#endif // ANONYMOUSBITCOINCOMPUTING_H
