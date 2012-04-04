#include "anonymousbitcoincomputing.h"

AnonymousBitcoinComputing::AnonymousBitcoinComputing(const WEnvironment &env)
    : WApplication(env)
{
    setTitle(ANONYMOUS_BITCOIN_COMPUTING_BASE_TITLE);

    buildMenu();
}
void AnonymousBitcoinComputing::zeroOutEachViewPointer()
{
    //assign each of the pointers to views that are in the view stack to zero. there are a few times we zero them out, so putting this in a helper function just reduces lines. on init, we zero em out... because when instantiate them we check to see if they are equal to zero... and on user logout, for example.. we zero them out again

    //m_AdSlotPurchaseView = 0; //the page where you can purchase slots. a huge filterable/sortable list
    //m_ListOwnedSitesView = 0; //the page where you can list your own sites, which then show up for others on the m_AdSlotPurchaseView. it's just a simple list of sites, with an "add new" field/button at the bottom. you need to prove ownership of course (meta tags, file in root folder, etc)
    //m_AddFundsView = 0; //precisely what it says. gives you a new key, shows pending balance, ajax updates to confirmed when appropriate
}
void AnonymousBitcoinComputing::buildMenu()
{
    //i want a WMenu that is site-wide that controls the contents of the m_ViewStack.

    //[Index], [Balance], [Advertising]
    //Index is just that, an index of all sites that a) advertise and b) contain advertisements. contrast with google's 'search engine' index...
    //Balance has sub-types "Add Funds", "Request Payout", "Payout Options"... and maybe a "Previous Funds Added Transactions" where they can refresh a previously used 'add funds key' in case they were stupid and paid to it again after a previous completion/confirmation
    //Advertising has sub-types "Advertise Your Page! - Get Known!" and "Sell Owned Ad Space! - Make Money!" or something. AdCaptcha would fall under the Advertising Category (but would also maybe have another reference to it under some other menu too ("Human Verification" maybe?)
    //TODO: other types include "Servers": "Sell Bandwidth/Space" and "Buy Bandwidth/Space". err, the bandwidth is purchased, but the space is rented. all just words anyways..

    WStackedWidget *content = new WStackedWidget();
    WAnimation slideInFromBottom(WAnimation::SlideInFromBottom, WAnimation::EaseOut, 200);
    content->setTransitionAnimation(slideInFromBottom, true);

    WMenu *menu = new WMenu(content, Wt::Vertical);
    menu->setInternalPathEnabled("/");

    //Home
    menu->addItem("Home", getView(AbcHomeView))->setPathComponent("");
    //Index
    menu->addItem("Index", getView(AbcIndexView));

    //Balance
    WSubMenuItem *balanceSubMenu = new WSubMenuItem(ANONYMOUS_BITCOIN_COMPUTING_BALANCE_TEXT, getView(AbcBalanceSubMenuView));
    WMenu *balanceMenu = new WMenu(content, Wt::Vertical);
    balanceMenu->setInternalPathEnabled("/" + (std::string)ANONYMOUS_BITCOIN_COMPUTING_BALANCE_TEXT + "/");

    //Balance -> Add Funds
    WMenuItem *balanceAddFundsMenuItem = balanceMenu->addItem((std::string)ANONYMOUS_BITCOIN_COMPUTING_MENU_SUB_ITEM_PREFIX + (std::string)ANONYMOUS_BITCOIN_COMPUTING_BALANCE_ADD_FUNDS_TEXT, getView(AbcBalanceAddFundsView));
    balanceAddFundsMenuItem->setPathComponent(ANONYMOUS_BITCOIN_COMPUTING_BALANCE_ADD_FUNDS_TEXT);

    //Balance -> Request Dispursement
    WMenuItem *balanceRequestPayoutMenuItem = balanceMenu->addItem((std::string)ANONYMOUS_BITCOIN_COMPUTING_MENU_SUB_ITEM_PREFIX + (std::string)ANONYMOUS_BITCOIN_COMPUTING_BALANCE_REQUEST_PAYOUT_TEXT, getView(AbcBalanceRequestPayoutView));
    balanceRequestPayoutMenuItem->setPathComponent(ANONYMOUS_BITCOIN_COMPUTING_BALANCE_REQUEST_PAYOUT_TEXT);

    //Payout Options
    //todo

    //now add the Balance sub-menu
    balanceSubMenu->setSubMenu(balanceMenu);
    menu->addItem(balanceSubMenu);

    //Advertising
    WSubMenuItem *advertisingSubMenu = new WSubMenuItem(ANONYMOUS_BITCOIN_COMPUTING_ADVERTISING_TEXT, getView(AbcAdvertisingSubMenuView));
    WMenu *advertisingMenu = new WMenu(content, Wt::Vertical);
    advertisingMenu->setInternalPathEnabled("/" + (std::string)ANONYMOUS_BITCOIN_COMPUTING_ADVERTISING_TEXT + "/");

    //Advertising -> Sell Ad Space
    WMenuItem *advertisingSellAdSpaceMenuItem = advertisingMenu->addItem((std::string)ANONYMOUS_BITCOIN_COMPUTING_MENU_SUB_ITEM_PREFIX + (std::string)ANONYMOUS_BITCOIN_COMPUTING_ADVERTISING_SELL_AD_SPACE_TEXT, getView(AbcAdvertisingSellAdSpaceView));
    advertisingSellAdSpaceMenuItem->setPathComponent(ANONYMOUS_BITCOIN_COMPUTING_ADVERTISING_SELL_AD_SPACE_TEXT);

    //Advertising -> Buy Ad Space
    WMenuItem *advertisingBuyAdSpaceMenuItem = advertisingMenu->addItem((std::string)ANONYMOUS_BITCOIN_COMPUTING_MENU_SUB_ITEM_PREFIX + (std::string)ANONYMOUS_BITCOIN_COMPUTING_ADVERTISING_BUY_AD_SPACE_TEXT, getView(AbcAdvertisingBuyAdSpaceView));
    advertisingBuyAdSpaceMenuItem->setPathComponent(ANONYMOUS_BITCOIN_COMPUTING_ADVERTISING_BUY_AD_SPACE_TEXT);

    //AdCaptcha
    //todo
    //adcaptcha.com or whatever would point to this /advertising/captcha/ pathCompement (make them match)

    //now add the Advertising sub-menu
    advertisingSubMenu->setSubMenu(advertisingMenu);
    menu->addItem(advertisingSubMenu);

    //Servers
    //todo. buy/sell

    menu->itemSelected().connect(this, &AnonymousBitcoinComputing::menuItemChanged);

    WHBoxLayout *hlayout = new WHBoxLayout();
    hlayout->addWidget(menu, 0, Wt::AlignTop);
    hlayout->addWidget(content);

    this->root()->setLayout(hlayout);
}
void AnonymousBitcoinComputing::menuItemChanged(Wt::WMenuItem *menuItem)
{
    //TODO: we might want to use this to tell the Qt thread to stop watching for certain variables that it will WServer.post() to us on update... but a more logical place to do this would be in internalPathChanged()...
    //TODOreq: after we tell Qt thread to not notify us of updates, our WContainerWidget that we use in the stack/menu is NOT set to zero again. we need to make sure that if we view it again, we not only re-use the same object, but we do want to ask the Qt thread to give us any updated values (that we stopped listening for). an error would be to pull the updated values in the constructor *only*... we should have a "getUpdatedValues()" or something which is called in the constructor AND when the object is re-shown. maybe it can be when we re-subscribe.. but that might also only give us FUTURE updates.. whereas we want updates that happened since we both a) switched to a different view (using the menu, for example) and b) told Qt thread to not WServer.post() us updates since we're no longer viewing whatever was updated

    WMenu *parentMenu = menuItem->menu();
    const std::vector<WMenuItem *> &parentMenuItems = parentMenu->items();
    int parentMenuItemsCount = parentMenuItems.size();
    WSubMenuItem *subItem;

    for(int i = 0; i < parentMenuItemsCount; ++i)
    {
        subItem = 0;
        if(parentMenuItems.at(i) == menuItem) //is the one we just clicked
        {
            subItem = dynamic_cast<WSubMenuItem*>(menuItem);
            if(subItem) //slopppy cast-succeed detection
            {
                setVisibilityForAllSubMenuItems(subItem, true);
            }
            //else, it's just a regular WMenuItem... doesn't need to be hidden or shown
        }
        else //not the one we just clicked
        {
            subItem = dynamic_cast<WSubMenuItem*>(parentMenuItems.at(i));
            if(subItem)
            {
                setVisibilityForAllSubMenuItems(subItem, false);
            }
            //else, it's just a regular WMenuItem... doesn't need to be hidden or shown
        }
    }
}
void AnonymousBitcoinComputing::setVisibilityForAllSubMenuItems(Wt::WSubMenuItem *subMenuItem, bool visible)
{
    const std::vector<WMenuItem *> &allMenuItems = subMenuItem->subMenu()->items();
    int size = allMenuItems.size();
    for(int i = 0; i < size; ++i)
    {
        allMenuItems.at(i)->setHidden(!visible);
    }
}
Wt::WContainerWidget * AnonymousBitcoinComputing::getView(AnonymousBitcoinComputing::AbcViews view)
{
    std::map<AbcViews,WContainerWidget*>::iterator it = m_AllViews.find(view);
    if(it != m_AllViews.end())
    {
        WContainerWidget *theView = it->second;
        if(theView)
        {
            return theView; //already created, return it
        }
        m_AllViews[view] = createView(view); //already created but was zero'd out/deleted, re-create it
        return m_AllViews[view];
    }
    else
    {
        m_AllViews[view] = createView(view); //has not been created, is not in the list yet. create it
        //m_AllViews.insert(std::pair<AbcViews,WContainerWidget*>(view, theView));
        return m_AllViews[view];
    }

    /*
    try
    {
        theView = m_AllViews.at(view);
        if(theView)
        {
            return theView;
        }
        theView = createView(view);
        m_AllViews[view] = theView;
        return theView;
    }
    catch(std::out_of_range exc)
    {

    }*/
}
Wt::WContainerWidget * AnonymousBitcoinComputing::createView(AnonymousBitcoinComputing::AbcViews view)
{
    switch(view)
    {
    case AnonymousBitcoinComputing::AbcHomeView:
        return new AbcHome();
        break;
    case AnonymousBitcoinComputing::AbcIndexView:
        return new AbcIndex();
        break;
    case AnonymousBitcoinComputing::AbcBalanceSubMenuView:
    {
        WContainerWidget *balanceSub = new WContainerWidget();
        balanceSub->addWidget(new WText("Pick A Sub-Category"));
        return balanceSub;
    }
        break;
    case AnonymousBitcoinComputing::AbcBalanceAddFundsView:
        return new AbcBalanceAddFunds();
        break;
    case AnonymousBitcoinComputing::AbcBalanceRequestPayoutView:
        return new AbcBalanceRequestPayout();
        break;
    case AnonymousBitcoinComputing::AbcAdvertisingSubMenuView:
    {
        WContainerWidget *advertiseSub = new WContainerWidget();
        advertiseSub->addWidget(new WText("Pick A Sub-Category"));
        return advertiseSub;
    }
        break;
    case AnonymousBitcoinComputing::AbcAdvertisingSellAdSpaceView:
        return new AbcAdvertisingSellAdSpace();
        break;
    case AnonymousBitcoinComputing::AbcAdvertisingBuyAdSpaceView:
        return new AbcAdvertisingBuyAdSpace();
        break;
    default:
        //error
        return 0;
        break;
    }
}
