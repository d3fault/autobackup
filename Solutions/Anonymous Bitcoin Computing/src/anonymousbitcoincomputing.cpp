#include "anonymousbitcoincomputing.h"

AnonymousBitcoinComputing::AnonymousBitcoinComputing(const WEnvironment &env)
    : WApplication(env), m_AuthWidgetIsInStack(false), m_PageNotFound(0)
{
    m_UsernameDb.getLogin().changed().connect(this, &AnonymousBitcoinComputing::handleLoginChanged);

    m_AuthWidget = new Wt::Auth::AuthWidget(Database::getAuthService(), m_UsernameDb.getUserDatabase(), m_UsernameDb.getLogin());
    m_AuthWidget->addPasswordAuth(&Database::getPasswordService());
    m_AuthWidget->setRegistrationEnabled(true);

    buildGui();

    this->internalPathChanged().connect(this, &AnonymousBitcoinComputing::handleInternalPathChanged);

    handleInternalPathChanged(this->internalPath());

    m_AuthWidget->processEnvironment();

    //debug
    std::cout << "wt.sessionId(" << this->sessionId() << "), wapplication constructor just called" << std::endl;
}
void AnonymousBitcoinComputing::buildGui()
{
    //i want a WMenu that is site-wide that controls the contents of the m_ViewStack.

    //[Index], [Balance], [Advertising]
    //Index is just that, an index of all sites that a) advertise and b) contain advertisements. contrast with google's 'search engine' index...
    //Balance has sub-types "Add Funds", "Request Payout", "Payout Options"... and maybe a "Previous Funds Added Transactions" where they can refresh a previously used 'add funds key' in case they were stupid and paid to it again after a previous completion/confirmation
    //Advertising has sub-types "Advertise Your Page! - Get Known!" and "Sell Owned Ad Space! - Make Money!" or something. AdCaptcha would fall under the Advertising Category (but would also maybe have another reference to it under some other menu too ("Human Verification" maybe?)
    //TODO: other types include "Servers": "Sell Bandwidth/Space" and "Buy Bandwidth/Space". err, the bandwidth is purchased, but the space is rented. all just words anyways..

    setTitle(ANONYMOUS_BITCOIN_COMPUTING_BASE_TITLE);

    m_MainStack = new WStackedWidget();
    //m_MainStack->addWidget(m_AuthWidget);
    WAnimation slideInFromBottom(WAnimation::SlideInFromBottom, WAnimation::EaseOut, 250);
    m_MainStack->setTransitionAnimation(slideInFromBottom, true);

    WAnchor *homeAnchor = new WAnchor(WLink(WLink::InternalPath, AbcHome::PreferredInternalPath), AbcHome::ReadableText);
    WAnchor *indexAnchor = new WAnchor(WLink(WLink::InternalPath, AbcIndex::PreferredInternalPath), AbcIndex::ReadableText);

    WAnchor *balanceAnchor = new WAnchor(WLink(WLink::InternalPath, "/balance"), "Balance"); //TODO: should show their balance, and also show "Add Funds" and "Request Dispursement" below it
    WAnchor *balanceAddFundsAnchor = new WAnchor(WLink(WLink::InternalPath, AbcBalanceAddFunds::PreferredInternalPath), "--" + AbcBalanceAddFunds::ReadableText);
    WAnchor *balanceRequestDisbursementAnchor = new WAnchor(WLink(WLink::InternalPath, AbcBalanceRequestPayout::PreferredInternalPath), "--" + AbcBalanceRequestPayout::ReadableText);

    //TODO: "Set Up Auto-Dispursement" (daily) <-- where they add a bunch of keys ... or just "Options"

    WAnchor *advertisingAnchor = new WAnchor(WLink(WLink::InternalPath, "/advertising"), "Advertising");
    WAnchor *advertisingSellAdSpaceAnchor = new WAnchor(WLink(WLink::InternalPath, AbcAdvertisingSellAdSpace::PreferredInternalPath), "--" + AbcAdvertisingSellAdSpace::ReadableText);
    WAnchor *advertisingBuyAdSpaceAnchor = new WAnchor(WLink(WLink::InternalPath, AbcAdvertisingBuyAdSpace::PreferredInternalPath), "--" + AbcAdvertisingBuyAdSpace::ReadableText);

    //TODO: AdCaptcha (both buy and sell?)

    //TODO: Servers (both buy and sell?)


    WVBoxLayout *mainVLayout = new WVBoxLayout();

    WHBoxLayout *headerHLayout = new WHBoxLayout();
    //todo: title?
    m_LogoutAnchor = new WAnchor(WLink(WLink::InternalPath, ANONYMOUS_BITCOIN_COMPUTING_LOGOUT_INTERNAL_PATH), "Logout");
    headerHLayout->addWidget(m_LogoutAnchor, 0, Wt::AlignTop | Wt::AlignRight);
    m_LogoutAnchor->setHidden(true);
    //headerHLayout->addWidget(m_AuthWidget);

    WHBoxLayout *bodyHLayout = new WHBoxLayout();

    WVBoxLayout *linksVLayout = new WVBoxLayout();

    linksVLayout->addWidget(homeAnchor);
    linksVLayout->addWidget(indexAnchor);
    linksVLayout->addWidget(balanceAnchor);
    linksVLayout->addWidget(balanceAddFundsAnchor);
    linksVLayout->addWidget(balanceRequestDisbursementAnchor);
    linksVLayout->addWidget(advertisingAnchor);
    linksVLayout->addWidget(advertisingSellAdSpaceAnchor);
    linksVLayout->addWidget(advertisingBuyAdSpaceAnchor);

    bodyHLayout->addLayout(linksVLayout, 1, Wt::AlignLeft | Wt::AlignTop);
    bodyHLayout->addWidget(m_MainStack, 3, Wt::AlignLeft | Wt::AlignTop);

    mainVLayout->addLayout(headerHLayout);
    mainVLayout->addLayout(bodyHLayout);

    this->root()->setLayout(mainVLayout);
}
//TODOreq: after we tell Qt thread to not notify us of updates, our WContainerWidget that we use in the stack/menu is NOT set to zero again. we need to make sure that if we view it again, we not only re-use the same object, but we do want to ask the Qt thread to give us any updated values (that we stopped listening for). an error would be to pull the updated values in the constructor *only*... we should have a "getUpdatedValues()" or something which is called in the constructor AND when the object is re-shown. maybe it can be when we re-subscribe.. but that might also only give us FUTURE updates.. whereas we want updates that happened since we both a) switched to a different view (using the menu, for example) and b) told Qt thread to not WServer.post() us updates since we're no longer viewing whatever was updated
Wt::WContainerWidget * AnonymousBitcoinComputing::getView(AnonymousBitcoinComputing::AbcViews view)
{
    AbcViewMap::iterator it = m_AllViews.find(view);
    if(it != m_AllViews.end())
    {
        WContainerWidget *theView = it->second;
        if(theView)
        {
            std::cout << "returning already created" << std::endl;
            return theView; //already created, return it
        }
        std::cout << "returning re-created" << std::endl;
        m_AllViews[view] = createView(view); //already created but was zero'd out/deleted, re-create it. test this when doing a logout and then navigating around. really though, it should only delete/0 the pointers that required a login. still, i guess we can safely assume(!!!!!!!!!!) that they're done browsing if they log out.. right?
        return m_AllViews[view];
    }
    else
    {
        std::cout << "returning newly created" << std::endl;
        m_AllViews[view] = createView(view); //has not been created, is not in the list yet. create it
        //m_AllViews.insert(std::pair<AbcViews,WContainerWidget*>(view, theView));
        return m_AllViews[view];
    }
}
Wt::WContainerWidget * AnonymousBitcoinComputing::createView(AnonymousBitcoinComputing::AbcViews view)
{
    WContainerWidget *containerView;
    switch(view)
    {
    case AnonymousBitcoinComputing::AbcHomeView:
        containerView = new AbcHome(m_MainStack);
        return containerView;
        break;
    case AnonymousBitcoinComputing::AbcIndexView:
        containerView = new AbcIndex(m_MainStack);
        return containerView;
        break;
    case AnonymousBitcoinComputing::AbcBalanceSubMenuView:
    {
        containerView = new WContainerWidget(m_MainStack);
        containerView->addWidget(new WText("Pick A Balance Sub-Category"));
        return containerView;
    }
        break;
    case AnonymousBitcoinComputing::AbcBalanceAddFundsView:
        containerView = new AbcBalanceAddFunds(m_MainStack);
        return containerView;
        break;
    case AnonymousBitcoinComputing::AbcBalanceRequestPayoutView:
        containerView = new AbcBalanceRequestPayout(m_MainStack);
        return containerView;
        break;
    case AnonymousBitcoinComputing::AbcAdvertisingSubMenuView:
    {
        containerView = new WContainerWidget(m_MainStack);
        containerView->addWidget(new WText("Pick An Advertising Sub-Category"));
        return containerView;
    }
        break;
    case AnonymousBitcoinComputing::AbcAdvertisingSellAdSpaceView:
        containerView = new AbcAdvertisingSellAdSpace(m_MainStack);
        return containerView;
        break;
    case AnonymousBitcoinComputing::AbcAdvertisingBuyAdSpaceView:
        containerView = new AbcAdvertisingBuyAdSpace(m_MainStack);
        return containerView;
        break;
    default:
        containerView = pageNotFound();
        return containerView;
        break;
    }
}
void AnonymousBitcoinComputing::handleInternalPathChanged(const std::string &newInternalPath)
{
    //debug
    std::cout << "wt.sessionId(" << this->sessionId() << "), internal path changed to: " << newInternalPath << std::endl;

    if(newInternalPath == ANONYMOUS_BITCOIN_COMPUTING_LOGOUT_INTERNAL_PATH)
    {
        m_UsernameDb.getLogin().logout();
        //WApplication::instance()->redirect(AbcHome::PreferredInternalPath);
        WApplication::instance()->setInternalPath(AbcHome::PreferredInternalPath);
        return;
    }
    showViewByInternalPath(newInternalPath);
}
void AnonymousBitcoinComputing::showViewByInternalPath(const std::string &internalPath)
{
    WWidget *viewToShow = getViewOrAuthWidgetDependingOnInternalPathAndWhetherOrNotLoggedInAndIfItMattersForInternalPath(internalPath);
    m_MainStack->setCurrentWidget(viewToShow);
}
Wt::WWidget * AnonymousBitcoinComputing::getViewOrAuthWidgetDependingOnInternalPathAndWhetherOrNotLoggedInAndIfItMattersForInternalPath(const std::string &internalPath)
{
    if(AbcHome::isInternalPath(internalPath))
    {
        if(AbcHome::requiresLogin() && !m_UsernameDb.getLogin().loggedIn())
        {
            return getAuthWidgetForStack();
        }
        return getView(AbcHomeView);
    }
    if(AbcIndex::isInternalPath(internalPath))
    {
        if(AbcIndex::requiresLogin() && !m_UsernameDb.getLogin().loggedIn())
        {
            return getAuthWidgetForStack();
        }
        return getView(AbcIndexView);
    }
    //TODO: /balance
    if(AbcBalanceAddFunds::isInternalPath(internalPath))
    {
        if(AbcBalanceAddFunds::requiresLogin() && !m_UsernameDb.getLogin().loggedIn())
        {
            return getAuthWidgetForStack();
        }
        return getView(AbcBalanceAddFundsView);
    }
    if(AbcBalanceRequestPayout::isInternalPath(internalPath))
    {
        if(AbcBalanceRequestPayout::requiresLogin() && !m_UsernameDb.getLogin().loggedIn())
        {
            return getAuthWidgetForStack();
        }
        return getView(AbcBalanceRequestPayoutView);
    }
    //TODO: /advertising
    if(AbcAdvertisingBuyAdSpace::isInternalPath(internalPath))
    {
        if(AbcAdvertisingBuyAdSpace::requiresLogin() && !m_UsernameDb.getLogin().loggedIn())
        {
            return getAuthWidgetForStack();
        }
        return getView(AbcAdvertisingBuyAdSpaceView);
    }
    if(AbcAdvertisingSellAdSpace::isInternalPath(internalPath))
    {
        if(AbcAdvertisingSellAdSpace::requiresLogin() && !m_UsernameDb.getLogin().loggedIn())
        {
            return getAuthWidgetForStack();
        }
        return getView(AbcAdvertisingSellAdSpaceView);
    }
    return pageNotFound();
}
Wt::WContainerWidget * AnonymousBitcoinComputing::pageNotFound()
{
    if(!m_PageNotFound)
    {
        m_PageNotFound = new WContainerWidget(m_MainStack);
        m_PageNotFound->addWidget(new WText("Error 404, Page Not Found"));
    }
    return m_PageNotFound;
}
void AnonymousBitcoinComputing::handleLoginChanged()
{
    //debug
    std::cout << "wt.sessionId(" << this->sessionId() << "), login changed..." << std::endl;

    if(m_UsernameDb.getLogin().loggedIn())
    {
        //debug
        std::cout << "wt.sessionId(" << this->sessionId() << "), LOGGED IN" << std::endl;

        //TODO: since this seems to be the first point of entry after the user first registers... we can check our app bank cache db to see if we have set up a (non-cached) bank account yet on our remote server. if we haven't, now is where we do it and we store the user/id (whatever) in the app bank cache db too. we init it to zero on user creation, then check to see if it's still zero right here. if it is, we know they haven't set up a bank account yet. we want to delay this until after the registration CONFIRMATION process, so spam creating accounts (without verifying them) doesn't even send a message to our remote bank server. ya know, i kinda like that: RemoteBankServer and LocalAppBankCache. they are the same except remote manages multiple app bank accounts (for future projects)

        m_LogoutAnchor->setHidden(false);
        //continue to wherever the user requested
        handleInternalPathChanged(this->internalPath());
    }
    else
    {
        //debug
        std::cout << "wt.sessionId(" << this->sessionId() << "), LOGGED OUT" << std::endl;

        m_LogoutAnchor->setHidden(true);
        AbcViewMap::iterator it = m_AllViews.begin();
        //WContainerWidget *theView = 0;
        while(it != m_AllViews.end())
        {
            /*theView = it->second;
            if(theView) //if it's not 0, then we can safely __ASSUME__ that it's still in the stack. stack doesn't have a contains() so i don't know how else to detect this
            {
                m_MainStack->removeWidget(theView);
                delete theView; //the examples don't call delete, they just assign to zero.. so if shit's crashing... we'll know why...
                theView = 0; //hmm what the fuck? does this set my pointer theView to zero? or does it set the pointer that i'm pointing to to zero? i want to set second to zero!
            }*/
            if(it->second) //if the pointer isn't zero
            {
                if(it->first != AbcHomeView) //and if it isn't the home view (we always want to keep that instantiated because we redirect to it after logging out)
                {
                    m_MainStack->removeWidget(it->second);
                    delete it->second;
                    it->second = 0;
                }
                else
                {
                    m_MainStack->setCurrentWidget(it->second); //redirect to home
                }
            }
            ++it;
        }
        //TODO: iterate through  m_AllViews, set it->second to 0; (also 'delete' ?), and do m_MainStack.removeWidget(it->second)... we want to keep the authwidget in the stack... but right now i have it in the gui twice... idk where i want it yet :-/
        //m_MainStack->clear();

        /*m_HomeView = 0;
        m_CampaignEditorView = 0;
        m_AdEditorView = 0;*/
    }
}
Wt::Auth::AuthWidget * AnonymousBitcoinComputing::getAuthWidgetForStack()
{
    if(!m_AuthWidgetIsInStack)
    {
        m_MainStack->addWidget(m_AuthWidget);
        m_AuthWidgetIsInStack = true;
    }
    return m_AuthWidget;
}
