#include "anonymousbitcoincomputing.h"

AnonymousBitcoinComputing::AnonymousBitcoinComputing(const WEnvironment &env)
    : WApplication(env)
{
    buildGui();
    this->internalPathChanged().connect(this, &AnonymousBitcoinComputing::handleInternalPathChanged);
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
    WAnimation slideInFromBottom(WAnimation::SlideInFromBottom, WAnimation::EaseOut, 250);
    m_MainStack->setTransitionAnimation(slideInFromBottom, true);

    WAnchor *homeAnchor = new WAnchor(WLink(WLink::InternalPath,"/home"),"Home");
    WAnchor *indexAnchor = new WAnchor(WLink(WLink::InternalPath, "/index"), "Index");

    WAnchor *balanceAnchor = new WAnchor(WLink(WLink::InternalPath, "/balance"), "Balance"); //TODO: should show their balance, and also show "Add Funds" and "Request Dispursement" below it
    WAnchor *balanceAddFundsAnchor = new WAnchor(WLink(WLink::InternalPath, "/balance/add-funds"), "--Add Funds");
    WAnchor *balanceRequestDisbursementAnchor = new WAnchor(WLink(WLink::InternalPath, "/balance/request-disbursement"), "--Request Disbursement");

    //TODO: "Set Up Auto-Dispursement" (daily) <-- where they add a bunch of keys

    WAnchor *advertisingAnchor = new WAnchor(WLink(WLink::InternalPath, "/advertising"), "Advertising");
    WAnchor *advertisingSellAdSpaceAnchor = new WAnchor(WLink(WLink::InternalPath, "/advertising/sell-ad-space"), "--Sell Ad Space");
    WAnchor *advertisingBuyAdSpaceAnchor = new WAnchor(WLink(WLink::InternalPath, "/advertising/buy-ad-space"), "--Buy Ad Space");

    //TODO: AdCaptcha (both buy and sell?)

    //TODO: Servers (both buy and sell?)


    WVBoxLayout *mainVLayout = new WVBoxLayout();

    WHBoxLayout *headerHLayout = new WHBoxLayout(); //todo: title? authwidget?

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

    bodyHLayout->addLayout(linksVLayout);
    bodyHLayout->addWidget(m_MainStack);

    mainVLayout->addLayout(headerHLayout);
    mainVLayout->addLayout(bodyHLayout);

    this->root()->setLayout(mainVLayout);
}
//TODOreq: after we tell Qt thread to not notify us of updates, our WContainerWidget that we use in the stack/menu is NOT set to zero again. we need to make sure that if we view it again, we not only re-use the same object, but we do want to ask the Qt thread to give us any updated values (that we stopped listening for). an error would be to pull the updated values in the constructor *only*... we should have a "getUpdatedValues()" or something which is called in the constructor AND when the object is re-shown. maybe it can be when we re-subscribe.. but that might also only give us FUTURE updates.. whereas we want updates that happened since we both a) switched to a different view (using the menu, for example) and b) told Qt thread to not WServer.post() us updates since we're no longer viewing whatever was updated
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
void AnonymousBitcoinComputing::handleInternalPathChanged(const std::string &newInternalPath)
{
    showViewByInternalPath(newInternalPath);
    /*if(m_UsernameDb.getLogin().loggedIn())
    {
        if(newInternalPath == HOME_PATH)
        {
            showHome();
        }
        else if(newInternalPath == OWNER_PATH)
        {
            showOwner();
        }
        else if(newInternalPath == PUBLISHER_PATH) //could also catch cases like AdvertiserPath (just pure semantics, fuck it)
        {
            showPublisher();
        }
        else //catch-all. could report an error here if we wanted
        {
            WApplication::instance()->setInternalPath(HOME_PATH, true);
        }
    }
    //else: TODO: i could put certain widgets that don't require login in here, and the others up above...
    */
}
void AnonymousBitcoinComputing::showViewByInternalPath(const std::string &internalPath)
{
    WContainerWidget *viewToShow = getViewOrAuthWidgetDependingOnInternalPathAndWhetherOrNotLoggedInAndIfItMattersForInternalPath(internalPath);
    m_MainStack->setCurrentWidget(viewToShow);
}
Wt::WWidget * AnonymousBitcoinComputing::getViewOrAuthWidgetDependingOnInternalPathAndWhetherOrNotLoggedInAndIfItMattersForInternalPath(const std::string &internalPath)
{
    if(internalPath == AbcHome::getInternalPath())
    {
        if(AbcHome::requiresLogin())
        {
            return getAuthWidget();
        }
        return getView(AbcHomeView);
    }
    //if(...)

    //there's gotta be a way i can do this more betterer
    //a read-only singleton could be shared by each Wt WApplication (user session)
    //only to be used as a "list of views" to iterate. but how can i reference the class and not an instantiation of it? further, how can i instantiate it later when i only have a reference to it? i'm suuuuuure it's possible... i'm just not good enough at C++ to do it :(
    //one thing i do know, is each view should implement an interface: IAbcView with virtual methods getInternalPath, requiresLogin, and maybe newInstantiation?? maybe i can code it so that newInstantiation... or i guess we could call it sessionInstance()... basically a per-WApplication-session singleton self-contained. idfk. it would be awesome if i could get the abstract (interface) code of sessionInstance() to call it's child constructor... so i'd only have to write the code once... but man at this point i'm really just dicking around with "prettiness" and "elegance" and not improving functionality...
    //so a read-only REAL singleton that has a list of IAbcView class types... for iterating
    //and each IAbcView is a per-WApplication-singleton self-contained. sounds fun to code lol.. although i'll admit in advance that i'm taking a pointless tangent. fuck it. coding is supposed to be fun. when it isn't fun, BONER
}
