#include "anonymousbitcoincomputing.h"

AnonymousBitcoinComputing::AnonymousBitcoinComputing(const WEnvironment &env)
    : WApplication(env), m_AuthWidgetIsInStack(false), m_PageNotFound(0), m_OurBankAccountIsSetUp(false)
{
    m_UsernameDb.getLogin().changed().connect(this, &AnonymousBitcoinComputing::handleLoginChanged);

    m_AuthWidget = new Wt::Auth::AuthWidget(Database::getAuthService(), m_UsernameDb.getUserDatabase(), m_UsernameDb.getLogin());
    m_AuthWidget->addPasswordAuth(&Database::getPasswordService());
    m_AuthWidget->setRegistrationEnabled(true);

    buildGui();

    this->internalPathChanged().connect(this, &AnonymousBitcoinComputing::handleInternalPathChanged);

    handleInternalPathChanged(this->internalPath());

    m_AuthWidget->processEnvironment();
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
    //TODOopt: re-arrange these based on usage statistics as well.

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
        //todo: links to them. fuck actually showing the balance
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
    case AnonymousBitcoinComputing::AbcFirstLoginView:
        containerView = new AbcFirstLogin(m_MainStack);
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
    //TODOreq: here, or somewhere, tell the AppDbHelper to NOT update us for the internal path that we just LEFT. i can use my WApplication identifier (AppDbHelperClient* or sessionId) to see if we're already watching for a set of values... and if we are, remove it right before we set the new notification. what i'm saying is that we can have this logic performed in AppDbHelper when a PageChange or whatever happens ('notify us of any values on the page we are looking at'). i like having this todo right here though because it relates to internal path changes

    //TODOopt: make newInternalPath lowercase before passing it to anything below. it will simply catch more <3. i need to verify that all my internal paths for each view are written (by me) in lowercase... i'm pretty sure they are

    if(newInternalPath == ANONYMOUS_BITCOIN_COMPUTING_LOGOUT_INTERNAL_PATH)
    {
        if(m_UsernameDb.getLogin().loggedIn())
        {
            m_UsernameDb.getLogin().logout();
        }
        //if the user navigated here while not being logged in, idfk what to do.. i guess redirect them home? which is what we do after we log out anyways. NOTE, we __NEED__ to have emitChanged = true as per the comment just below in order for erroneous navigations to /logout while not logged in to redirect to home. as of right now it will just show a blank page (return;) after changing the url in the browser to /home??? weird.... but not all that important
        this->setInternalPath(AbcHome::PreferredInternalPath); //TODOreq: maybe set emitChanged to true here and i won't need the "m_MainStack->setCurrentWidget(it->second); //redirect to home" line in handleLoginChanged. i'm relatively sure, but i don't want to test it right now because i'm doing way too many other changes at the same time. as it stands all this line does here (setInternalPath) is makes it so the url in the browser matches the widget that we show after responding to the logout() call a few lines up. if we emit the change, we'll come right back to this handleInternalPathChanged, skip this if statement, and go straight into showViewByInter.... and the home widget will be set regularly in there and we'll have the url set. just semantics but less sloppy code. this explaination isn't worth it
        return;
    }

    //catch attempts to go to any page after logging in without completing the first-login step... but let us through if newInternalPath _IS_ first-login of course... otherwise we'd have infinite loop here
    if(!m_OurBankAccountIsSetUp && (!AbcFirstLogin::isInternalPath(newInternalPath)) && m_UsernameDb.getLogin().loggedIn())
    {
        this->setInternalPath(AbcFirstLogin::PreferredInternalPath, true);
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
    //TODOopt: based on usage statistics, i can re-arrange these if/else-if statements accordingly

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
    if(AbcFirstLogin::isInternalPath(internalPath))
    {
        if(AbcFirstLogin::requiresLogin() && !m_UsernameDb.getLogin().loggedIn())
        {
            return getAuthWidgetForStack();
        }
        return getView(AbcFirstLoginView);
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
    if(m_UsernameDb.getLogin().loggedIn())
    {
        //TODO: since this seems to be the first point of entry after the user first registers... we can check our app bank cache db to see if we have set up a (non-cached) bank account yet on our remote server. if we haven't, now is where we do it and we store the user/id (whatever) in the app bank cache db too. we init it to zero on user creation, then check to see if it's still zero right here. if it is, we know they haven't set up a bank account yet. we want to delay this until after the registration CONFIRMATION process, so spam creating accounts (without verifying them) doesn't even send a message to our remote bank server. ya know, i kinda like that: RemoteBankServer and LocalAppBankCache. they are the same except remote manages multiple app bank accounts (for future projects)

        //^^^^OUTDATED

        //GetAndOrCreateBankAccountAsyncOrBlocking
        //or something. maybe a QHash<QString=username,bool=accountIsSetup> on in our AppDbHelper cache. if it's not in there, we try to set one up by sending a request to the AppDb. This also can/should/but-doesn't-have-to send a request to the Bitcoin Server setting up an account on that (associated with our AppDb's AppId)... we could delay it until they request an add funds key... but that might complicate shit unnecessarily. it is a QHash and not a QCache because we want to know right away whether or not they have an account set up... but we don't necessarily need any of the values from it. Damn dude this is getting complicated especially since you don't have a defined goal here. do you want to show the balance on every page??? if that were the case then you WOULD want it to be in this hash. i don't fucking know anymore i want to kill myself. i want relief.
        //a reason for NOT delaying the creation of this bank account until they do an add funds request is if they are using the site solely for listing their page to advertise on. in this case they will only be receiving money and will never click 'Add Funds'. there's also a lot of other functionality that i plan to add which might conflict the same way.
        //we could use the boolean in that hash to also trigger a FirstLoginExtraSteps widget thingo... that asks them for their public signing key if they want to use the Secure part of OptionallySecureAsFuckBitcoinServer
        //ok seriously though, it would be stupid to put this in AppDbHelper.. it should be in a shared_lock protected std::map... since we'r e in boost land
        //then again logins don't happen ALL THAT OFTEN so maybe not, especially since i can't figure out how to safely write to that std::map from a qt thread. or maybe this is a different problem and there is no problem with it. maybe that's just my cached objects shit and this is easy. there's too much caching and maps and hashes and qcache and fuck man my head REALLY hurts, SHOTGUN IS SO CLOSE. but i won't do it. now i'm blogging in my code...
        //the race condition only applies to values that will be updated in the future. once this boolean is set to true it will always remain so. in fact what the fuck is the point of having a boolean if once it's there, it's always true. just have a std::list of usernames that already have bank accounts set up

        m_Username = m_UsernameDb.getLogin().user().identity(Auth::Identity::LoginName).toUTF8(); //TODOreq: show this somewhere, maybe by the logout buttan just like hangman does. but i think they just use a built in authwidget logout button thingo idfk

        m_OurBankAccountIsSetUp = AppDbHelper::Instance()->isBankAccountSetUpForUser(m_Username); //we need to keep this bool too in case they try to navigate to any other pages [that require authentication? or just any after first login? the choice is mine but idk wtf to choose] without completing the first-login stuff. anyways, we'll check this somewhere in the flow of handleInternalPathChanged
        //i don't know how i'm going to later set this to true... since it will have to be through a push/notify... but idfk, i'll figure something out... (famous last words)

        if(!m_OurBankAccountIsSetUp)
        {
            this->setInternalPath(AbcFirstLogin::PreferredInternalPath, false); //don't emit the change because we process it just below.
            //TODOreq: the first-login page widget shit should deferRendering until the bank account has been set up. i guess we also need a variable for 'setting up bank account' so they don't log out, log back in, and try to create another one. man so many millions of ways of breaking shit. shotgun. or 2 logins at once from multiple computers (on different wt-cache's). we'd just have to (on AppDb) process the first one and then return an error for the second. or i guess just say it went alright since i mean fuck it, it did. they have a bank account so who gives a shit. we shouldn't punish them for requesting a second one though, it's a bug on our end that we're able to handle without crashing. JESUS FUCKING KILL ME NOW. Dear God, If You Exist, Please Remove Me From Existence Right This Very Instance. oh right i'm still here >_>

            //we could send the request to create the bank account right here... maybe blocking until it's creation? but that's ugly. we could do it async, but then the user hasn't had their rendering deferred and this is a terrible spot to do so (where would i resume?)
            //the first-login widget could even just say 'Please Wait While Your Account Is Being Set Up'... defer itself, dispatch the request.. then viola. done when we get a callback... but i still have to set m_OurBankAccountIsSetUp to true; lol. WORST CASE i pass in this as an AnonymousBitcoinComputing* and then call a public function on it hahaha (but we STILL NEED CONTEXT FIRST!!!). it would be the same as calling WApplication::Instance(), except our additional method would be exposed.
            //i think my incomplete add funds request code is going to get in the way of me testing this initial login functionality, which obviously needs to be coded first. god i'm so dumb.
        }


        m_LogoutAnchor->setHidden(false);
        //continue to wherever the user requested
        handleInternalPathChanged(this->internalPath());
    }
    else
    {
        m_Username = "";

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
