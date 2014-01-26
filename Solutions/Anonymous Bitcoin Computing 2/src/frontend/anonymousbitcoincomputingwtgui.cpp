#include "anonymousbitcoincomputingwtgui.h"

//internal paths

#define ABC_INTERNAL_PATH_REGISTER "/register"
#define ABC_ANCHOR_TEXTS_REGISTER "Register"

#define ABC_INTERNAL_PATH_HOME "/home" //if home has any "sub" paths, then our hack to handle clean urls logic will break (should use internalPathMatches instead)
#define ABC_INTERNAL_PATH_ADS "/advertising"
#define ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE ABC_INTERNAL_PATH_ADS \
                                    "/buy-ad-space"
//hardcoded internal paths, would be dynamic ideally
#define ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE_D3FAULT ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE \
                                        "/d3fault"
#define ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE_D3FAULT_CAMPAIGN_0 ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE_D3FAULT \
                                            "/0"

#define ABC_ANCHOR_TEXTS_PATH_HOME "Home"
#define ABC_ANCHOR_TEXTS_PATH_ADS "Advertising"
#define ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE "Buy Ad Space"
#define ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE_D3FAULT "d3fault"
#define ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE_D3FAULT_CAMPAIGN_0 "d3fault's Ad Campaign #0"

//TODOreq: "Forgot Your Password?" --> "Tough shit, I hope you learned your lesson"
//TODOreq: timezones fuck shit up? if so, we can send them the 'current timestamp' to use (but then there'd be a bit of latency delay)... or mb we can find out proper solution in js (toTime() gives us msecs since epoch... in greenwhich right? not local? maybe this isn't a problem)

//TODOoptimization: lots of stuff needs to be moved into it's own object instead of just being a member in this class. it will reduce the memory-per-connection by...  maybe-a-significant... amount. for example the HackedInD3faultCampaign0 shit should be on it's own widget, but ON TOP OF THAT each "step" in the HackedInD3faultCampaign0 thing (buy step 1, buy step 2) can/should be it's own object (member of of HackedInD3faultCampaign0 object). We just have too many unused and unneeded-most-of-the-time member variables in this monster class... but KISS so ima continue for now, despite cringing at how ugly/hacky it's becoming :)

//TODOreq: can't remember if i wrote this anywhere or only thought of it, but i need to add underscores between certain things in my couchbase docs. for example a user named JimboKnives0 would have a conflict with a user named JimboKnives. adSpaceSlotFillersJimboKnives0 would now point to a specific ad slot filler, _AND_ the "profile view" (last 10 slot fillers set up) for JimboKnives0 -- hence, conflict. Could maybe solve this another way by perhaps always having the username be the very last part of the key???

//TODOoptimization: walking the internal path via "next sub path" (including sanitization at each step) is a good way to organize the website and additionally fill in json key pieces. example: /ads/buy-ad-space/d3fault/0 could be organized in Wt as: class Ads { class BuyAdSpace { m_SellerUsername = d3fault; m_SellerCampaign = 0; } }.... and organized in json as ads_buy_d3fault_0.  sanitization errors would be simple 404s, and it goes without saying that neither can/should include underscores or slashes

//TODOreq: vulnerable to session fixation attacks xD... but my login shit doesn't work when i changeSessionId xD. ok wtf now changeSessionId _does_ work [and solves it]... fuck it. Nvm, even when using changeSessionId I can still 'jump into' the session by copy/pasting the url (not current, but any of the links (or current after i've clicked any of the links (so that the change session id has taken effect))) into a new tab... but maybe this is fine since the session is still not known to an adversary who may have given a victim their own. HOWEVER it is still vulnerable to copy/pasting of of the URLs to irc/forums/wherever.... i think (not sure (i saw wt has user agent identification protection, but bah easily forged anyways)). i only think and am not sure because i wonder if that's the intended functionality? Perhaps my IP is doing the protection at this point and I'd only be vulnerable to others on my LAN? Fucking http/www is a piece of shit. Cookies aren't secure, get/post isn't secure... WHAT THE FUCK _IS_ secure!?!?!? "Yea just don't run an http server and you're set"
//TODOreq: so long as Wt detects/thwarts two IPs (with matching user agents) trying to use the same session id, I think I'm happy enough with that... but gah still an office building network admin could snoop http[s?] requests and then login as any of his (O WAIT HE COULD KEYLOGGER THEM ANYWAYS LOLOL)
//NOPE: Maybe I need to set a cookie and then depend on the combination of the session id and the cookie [which needs it's own id of course (they can't match and the cookie can't be derived from session id (else attacker could derive same cookie))]

AnonymousBitcoinComputingWtGUI::AnonymousBitcoinComputingWtGUI(const WEnvironment &myEnv)
    : WApplication(myEnv), m_HeaderHlayout(new WHBoxLayout()), m_MainVLayout(new WVBoxLayout(root())), m_LoginLogoutStackWidget(new WStackedWidget()), m_LoginWidget(new WContainerWidget(m_LoginLogoutStackWidget)), m_LoginUsernameLineEdit(0), m_LoginPasswordLineEdit(0), m_LogoutWidget(0), m_MainStack(new WStackedWidget()), m_HomeWidget(0), m_AdvertisingWidget(0), m_AdvertisingBuyAdSpaceWidget(0), m_RegisterWidget(0), /*m_RegisterSuccessfulWidget(0),*/ m_AdvertisingBuyAdSpaceD3faultWidget(0), m_AdvertisingBuyAdSpaceD3faultCampaign0Widget(0), m_AllSlotFillersComboBox(0), m_AddMessageQueuesRandomIntDistribution(0, NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES - 1), m_GetMessageQueuesRandomIntDistribution(0, NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES - 1), m_WhatTheGetWasFor(INITIALINVALIDNULLGET), m_LoggedIn(false)
{
    m_RandomNumberGenerator.seed((int)rawUniqueId());
    m_CurrentAddMessageQueueIndex = m_AddMessageQueuesRandomIntDistribution(m_RandomNumberGenerator); //TODOoptimization: these don't need to be members if i just use them once in constructor
    m_CurrentGetMessageQueueIndex = m_GetMessageQueuesRandomIntDistribution(m_RandomNumberGenerator);

    buildGui(); //everything that is NOT dependent on the internal path

    internalPathChanged().connect(this, &AnonymousBitcoinComputingWtGUI::handleInternalPathChanged);

    //might use enableUpdates/triggerUpdates for "post"/"add"/"store" shit, since durability tests can take a while.... but meh fuck it for now..
    //if(myEnv.ajax())
    //{
    //    enableUpdates(true);
    //}

    //hack to handle clean urls when [clean] url is typed in directly (no session). idk why wt doesn't do this for us...
    const std::string &cleanUrlInternalPath = internalPath();
    if(isHomePath(cleanUrlInternalPath))
    {
        showHomeWidget();
        return;
    }
    handleInternalPathChanged(cleanUrlInternalPath);
    //if adding more code here, take "return;" out of isHomePath and put handleInternalPathChanged into an "else" (the optimization isn't even worth this comment)
}
void AnonymousBitcoinComputingWtGUI::buildGui()
{
    setTitle("Anonymous Bitcoin Computing");
    //WAnimation slideInFromBottom(WAnimation::SlideInFromBottom, WAnimation::EaseOut, 250); //If this took any longer than 2 lines of code (<3 Wt), I wouldn't do it
    //m_MainStack->setTransitionAnimation(slideInFromBottom, true);

    //Login Widget
    new WText("Username:", m_LoginWidget);
    m_LoginUsernameLineEdit = new WLineEdit(m_LoginWidget);
    m_LoginUsernameLineEdit->enterPressed().connect(this, &AnonymousBitcoinComputingWtGUI::handleLoginButtonClicked);
    new WText("Password:", m_LoginWidget);
    m_LoginPasswordLineEdit = new WLineEdit(m_LoginWidget);
    m_LoginPasswordLineEdit->setEchoMode(WLineEdit::Password);
    m_LoginPasswordLineEdit->enterPressed().connect(this, &AnonymousBitcoinComputingWtGUI::handleLoginButtonClicked);
    WPushButton *loginButton = new WPushButton("Log In", m_LoginWidget);
    loginButton->clicked().connect(this, &AnonymousBitcoinComputingWtGUI::handleLoginButtonClicked);
    new WBreak(m_LoginWidget);
    new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_REGISTER), ABC_ANCHOR_TEXTS_REGISTER, m_LoginWidget);
    m_LoginLogoutStackWidget->setCurrentWidget(m_LoginWidget); //might not be necessary, since it's the only one added at this point (comment is not worth...)
    m_HeaderHlayout->addWidget(m_LoginLogoutStackWidget, 0, Wt::AlignTop | Wt::AlignRight);
    m_MainVLayout->addLayout(m_HeaderHlayout, 0, Wt::AlignTop | Wt::AlignRight);
    m_MainVLayout->addWidget(m_MainStack, 1, Wt::AlignTop | Wt::AlignCenter);
}
void AnonymousBitcoinComputingWtGUI::showHomeWidget()
{
    if(!m_HomeWidget)
    {
        m_HomeWidget = new WContainerWidget(m_MainStack);
        new WText("Welcome To Anonymous Bitcoin Computing", m_HomeWidget);
        new WBreak(m_HomeWidget);
        new WBreak(m_HomeWidget);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS), ABC_ANCHOR_TEXTS_PATH_ADS, m_HomeWidget);
    }
    m_MainStack->setCurrentWidget(m_HomeWidget);
}
void AnonymousBitcoinComputingWtGUI::showAdvertisingWidget()
{
    if(!m_AdvertisingWidget)
    {
        m_AdvertisingWidget = new WContainerWidget(m_MainStack);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_HOME), ABC_ANCHOR_TEXTS_PATH_HOME, m_AdvertisingWidget);
        new WBreak(m_AdvertisingWidget);
        new WBreak(m_AdvertisingWidget);
        new WText("You Are Here: " + WString(ABC_ANCHOR_TEXTS_PATH_ADS), m_AdvertisingWidget);
        new WBreak(m_AdvertisingWidget);
        new WBreak(m_AdvertisingWidget);
        new WBreak(m_AdvertisingWidget);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE), ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE, m_AdvertisingWidget);
    }
    m_MainStack->setCurrentWidget(m_AdvertisingWidget);
}
void AnonymousBitcoinComputingWtGUI::showAdvertisingBuyAdSpaceWidget()
{
    if(!m_AdvertisingBuyAdSpaceWidget)
    {
        m_AdvertisingBuyAdSpaceWidget = new WContainerWidget(m_MainStack);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_HOME), ABC_ANCHOR_TEXTS_PATH_HOME, m_AdvertisingBuyAdSpaceWidget);
        new WBreak(m_AdvertisingBuyAdSpaceWidget);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS), ABC_ANCHOR_TEXTS_PATH_ADS, m_AdvertisingBuyAdSpaceWidget);
        new WBreak(m_AdvertisingBuyAdSpaceWidget);
        new WBreak(m_AdvertisingBuyAdSpaceWidget);
        new WText("You Are Here: " + WString(ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE), m_AdvertisingBuyAdSpaceWidget);
        new WBreak(m_AdvertisingBuyAdSpaceWidget);
        new WBreak(m_AdvertisingBuyAdSpaceWidget);
        new WBreak(m_AdvertisingBuyAdSpaceWidget);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE_D3FAULT), ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE_D3FAULT, m_AdvertisingBuyAdSpaceWidget);
    }
    m_MainStack->setCurrentWidget(m_AdvertisingBuyAdSpaceWidget);
}
void AnonymousBitcoinComputingWtGUI::showRegisterWidget()
{
    if(!m_RegisterWidget)
    {
        m_RegisterWidget = new WContainerWidget(m_MainStack);
        new WText("Username:", m_RegisterWidget);
        m_RegisterUsername = new WLineEdit(m_RegisterWidget);
        m_RegisterUsername->enterPressed().connect(this, &AnonymousBitcoinComputingWtGUI::handleRegisterButtonClicked); //was tempted to not put this here because if they press enter in username then they probably aren't done, BUT that 'implicit form submission' bullshit would submit it anyways. might as well make sure it's pointing at the right form...
        new WBreak(m_RegisterWidget);
        new WText("Password:", m_RegisterWidget);
        m_RegisterPassword = new WLineEdit(m_RegisterWidget);
        m_RegisterPassword->setEchoMode(WLineEdit::Password);
        m_RegisterPassword->enterPressed().connect(this, &AnonymousBitcoinComputingWtGUI::handleRegisterButtonClicked);

        new WBreak(m_RegisterWidget);
        new WBreak(m_RegisterWidget);

        WPushButton *registerButton = new WPushButton("Register", m_RegisterWidget);
        registerButton->clicked().connect(this, &AnonymousBitcoinComputingWtGUI::handleRegisterButtonClicked);

        new WBreak(m_RegisterWidget);
        new WBreak(m_RegisterWidget);

        new WText("Optional:", m_RegisterWidget);
        new WBreak(m_RegisterWidget);
        new WText("Sexual Preference:", m_RegisterWidget);
        new WLineEdit(m_RegisterWidget);
        new WBreak(m_RegisterWidget);
        new WText("Social Security Number:", m_RegisterWidget);
        new WLineEdit(m_RegisterWidget);
        new WBreak(m_RegisterWidget);
        new WText("Religion:", m_RegisterWidget);
        new WLineEdit(m_RegisterWidget);
        new WBreak(m_RegisterWidget);
        new WText("Political Beliefs:", m_RegisterWidget);
        new WLineEdit(m_RegisterWidget);
        new WBreak(m_RegisterWidget);
        new WText("Time of day your wife is home alone:", m_RegisterWidget);
        new WLineEdit(m_RegisterWidget);
        new WBreak(m_RegisterWidget);
        new WText("Where you keep your gun:", m_RegisterWidget);
        new WLineEdit(m_RegisterWidget);
        new WBreak(m_RegisterWidget);
        new WText("Your wife's cycle:", m_RegisterWidget);
        new WLineEdit(m_RegisterWidget);
        new WBreak(m_RegisterWidget);
        new WText("Your mum's cycle:", m_RegisterWidget);
        new WLineEdit(m_RegisterWidget);
        new WBreak(m_RegisterWidget);
        new WText("Credit Card #:", m_RegisterWidget);
        new WLineEdit(m_RegisterWidget);
        new WBreak(m_RegisterWidget);
        new WText("Credit Card Pin #:", m_RegisterWidget);
        new WLineEdit(m_RegisterWidget);
        new WBreak(m_RegisterWidget);
        new WText("The most embarrassing thing you've put up your anus in order to climax:", m_RegisterWidget);
        new WLineEdit(m_RegisterWidget);

        new WBreak(m_RegisterWidget);
        new WBreak(m_RegisterWidget);

        //;-) and then secretly ;-) on the deployed/binary version ;-) i actually save these values ;-) and then sell them to hollywood ;-) to make movies off of them ;-) and then use the funds from that to take over the world ;-) and solve us of our corporate cancers (of which hollywood is an item) ;-) muaahahahahhahaha

        WPushButton *registerButton2 = new WPushButton("Register", m_RegisterWidget);
        registerButton2->clicked().connect(this, &AnonymousBitcoinComputingWtGUI::handleRegisterButtonClicked);
    }
    m_MainStack->setCurrentWidget(m_RegisterWidget);
}
void AnonymousBitcoinComputingWtGUI::showAdvertisingBuyAdSpaceD3faultWidget()
{
    if(!m_AdvertisingBuyAdSpaceD3faultWidget)
    {
        m_AdvertisingBuyAdSpaceD3faultWidget = new WContainerWidget(m_MainStack);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_HOME), ABC_ANCHOR_TEXTS_PATH_HOME, m_AdvertisingBuyAdSpaceD3faultWidget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultWidget);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS), ABC_ANCHOR_TEXTS_PATH_ADS, m_AdvertisingBuyAdSpaceD3faultWidget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultWidget);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE), ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE, m_AdvertisingBuyAdSpaceD3faultWidget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultWidget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultWidget);
        new WText("You Are Here: " + WString(ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE_D3FAULT), m_AdvertisingBuyAdSpaceD3faultWidget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultWidget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultWidget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultWidget);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE_D3FAULT_CAMPAIGN_0), ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE_D3FAULT_CAMPAIGN_0, m_AdvertisingBuyAdSpaceD3faultWidget);
    }
    m_MainStack->setCurrentWidget(m_AdvertisingBuyAdSpaceD3faultWidget);
}
void AnonymousBitcoinComputingWtGUI::beginShowingAdvertisingBuyAdSpaceD3faultCampaign0Widget()
{
    //TODOreqoptimization: this is going to be my most expensive document (the home link might be too, but it doesn't hit the db). I need SOME sort of caching solution [in order to make this horizontally scalable (so in other words, it isn't an absolute must pre-launch task], even if it's hacked-in/hardcoded who cares. getAndSubscribe comes to mind (but sounds complicated). a fucking mutex locked when new'ing the WContainerWidget below would be easy and would scale horizontally. Hell it MIGHT even be faster than a db hit (and there's always a 'randomly selected mutex in array of mutexes' hack xD)

    if(!m_AdvertisingBuyAdSpaceD3faultCampaign0Widget)
    {
        m_AdvertisingBuyAdSpaceD3faultCampaign0Widget = new WContainerWidget(m_MainStack);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_HOME), ABC_ANCHOR_TEXTS_PATH_HOME, m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS), ABC_ANCHOR_TEXTS_PATH_ADS, m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE), ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE, m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE_D3FAULT), ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE_D3FAULT, m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WText("You Are Here: " + WString(ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE_D3FAULT_CAMPAIGN_0), m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);

        //still dunno if i should block or do it async? instincts tell me async, but "SUB MILLISECOND LATENCY" tells me async might actually be wasteful/slower??? The obvious answer is "benchmark it xD" (FUCK FUCK FUCK FUCK FUCK THAT, async it is (because even if slower, it still allows us to scale bettarer))
        //^To clarify, I have 3 options: wait on a wait condition right here that is notified by couchbase thread and we serve up result viola, deferRendering/resumeRendering, or enableUpdates/TriggerUpdates. The last one requires ajax. Both the last 2 are async (despite defer "blocking" (read:disabling) the GUI)
        getCouchbaseDocumentByKeyBegin("adSpaceSlotsd3fault0");
        m_WhatTheGetWasFor = HACKEDIND3FAULTCAMPAIGN0GET;
    }
    m_MainStack->setCurrentWidget(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
}
void AnonymousBitcoinComputingWtGUI::finishShowingAdvertisingBuyAdSpaceD3faultCampaign0Widget(const string &couchbaseDocument)
{
    m_HackedInD3faultCampaign0JsonDocForUpdatingLaterAfterSuccessfulPurchase = couchbaseDocument;
    //TODOreq: this is the javascript impl of the countdown shit. we obviously need to still verify that the math is correct when a buy attempt happens (we'll be using C++ doubles as well, so will be more precise). we should detect when a value lower than 'current' is attempted, and then laugh at their silly hack attempt. i should make the software laugh at me to test that i have coded it properly (a temporary "submit at price [x]" line edit just for testing), but then leave it (the laughing when verification fails, NOT the line edit for testing) in for fun.
    //TODOreq: decided not to be a dick. "buy at current" sends the currentSlotIdForSale and the 'priceUserSawWhenTheyClicked', BUT we use our own internal and calculated-on-the-spot 'current price' and go ahead with the buy using that. We only use currentSlotIdForSale and 'priceUserSawWhenTheyClicked' to make sure they're getting the right slot [and not paying too much]. The two checks are redundant of one another, but that's ok

    if(environment().ajax())
    {
        ptree pt;
        std::istringstream is(couchbaseDocument);
        read_json(is, pt);

        m_HackedInD3faultCampaign0_MinPrice = pt.get<std::string>("minPrice");
        m_HackedInD3faultCampaign0_SlotLengthHours = pt.get<std::string>("slotLengthHours");
        boost::optional<ptree&> lastSlotFilledAkaPurchased = pt.get_child_optional("lastSlotFilledAkaPurchased");
        boost::optional<ptree&> currentSlotOnDisplay = pt.get_child_optional("currentSlotOnDisplay");
        boost::optional<ptree&> nextSlotOnDisplay = pt.get_child_optional("nextSlotOnDisplay");

        if(!lastSlotFilledAkaPurchased.is_initialized())
        {
            //no purchases yet, mostly empty json doc
        }
        else
        {
            if(!nextSlotOnDisplay.is_initialized())
            {
                //partial json doc: no 'next' purchase [yet] -- no big deal, but still must be accounted for (happens after very first purchase, and also when the 2nd to last slot on display expires)

                m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedSlotIndex = lastSlotFilledAkaPurchased.get().get<std::string>("slotIndex");
                m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedPurchaseTimestamp = lastSlotFilledAkaPurchased.get().get<std::string>("purchaseTimestamp");
                m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedStartTimestamp = lastSlotFilledAkaPurchased.get().get<std::string>("startTimestamp");
                m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedPurchasePrice = lastSlotFilledAkaPurchased.get().get<std::string>("purchasePrice");

#ifdef LOL_I_HAVE_PLENTY_OF_TIME_TO_KILL_AND_AM_BORED_AND_DONT_WANT_TO_LAUNCH_ASEP //Wt's chart seemed good enough for visualization, but didn't appear at a glance to be very javascript-interaction-friendly. Hoving your mouse over the line and seeing 'price at that time' would be nifty, as would the "dot"/ball aka "now" slowly going down to minprice. A 3rd party javascript lib might be better/easier, but then it's a matter of making it play nice with Wt (i'd just put the entire lib in "doJavascript" i'd imagine xD?) and then remembering I have bigger problems than this
                //TODOreq: populate model for chart first
                WCartesianChart *priceChart = new WCartesianChart(ScatterPlot, m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
                priceChart->setModel();
                priceChart->setXSeriesColumn(0);
                priceChart->axis(XAxis).setScale(DateTimeScale);
                priceChart->axis(YAxis).setScale(LinearScale);
                //TODOreq: x range = (last purchase datetime -> last purchase expire datetime)
                //TODOreq: PointSeries/CircleMarker for 'now', LineSeries/NoMarker for everything else
#endif

                new WText("Price in BTC: ", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
                WText *placeholderElement = new WText(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
                placeholderElement->doJavaScript
                        (
                            "var lastSlotFilledAkaPurchasedExpireDateTime = new Date((" + m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedStartTimestamp + "*1000)+((" + m_HackedInD3faultCampaign0_SlotLengthHours + "*3600)*1000));" +
                            "var lastSlotFilledAkaPurchasedPurchasePrice = " + m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedPurchasePrice + ";" + //made a var here so it can be updated later (on buy event) without stopping/restarting timer
                            "var lastSlotFilledAkaPurchasedPurchaseTimestamp = " + m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedPurchaseTimestamp + ";" + //ditto as above
                            "var lastSlotFilledAkaPurchasedExpireDateTimeMSecs = lastSlotFilledAkaPurchasedExpireDateTime.getTime();" + //ditto as above two
                            "var m = ((" + m_HackedInD3faultCampaign0_MinPrice + "-(lastSlotFilledAkaPurchasedPurchasePrice*2))/((lastSlotFilledAkaPurchasedExpireDateTimeMSecs/1000)-lastSlotFilledAkaPurchasedPurchaseTimestamp));" +
                            "var b = (" + m_HackedInD3faultCampaign0_MinPrice + " - (m * (lastSlotFilledAkaPurchasedExpireDateTimeMSecs/1000)));" +
                            "var tehIntervalz = setInterval(" +
                                "function()" +
                                "{" +
                                    "var currentDateTimeMSecs = new Date().getTime();" +
                                    "if(currentDateTimeMSecs >= lastSlotFilledAkaPurchasedExpireDateTimeMSecs)" +
                                    "{" +
                                        "var minPrice = " + m_HackedInD3faultCampaign0_MinPrice + ";" +
                                        placeholderElement->jsRef() + ".innerHTML = minPrice.toFixed(8);" + //TODOreq: stop interval (start it again on buy event)
                                        "clearInterval(tehIntervalz);" +
                                    "}" +
                                    "else" +
                                    "{" +
                                        "var currentPrice = ((m*(currentDateTimeMSecs/1000))+b);" +  //y = mx+b
                                        placeholderElement->jsRef() + ".innerHTML = currentPrice.toFixed(8);" +
                                    "}" +
                                "},100);" //INTERVAL AT 100ms
                        );
            }
            else
            {
                //traditonal full json doc ('next' already purchased)
            }
        }
    }
    else
    {
        new WText("enable javascript nob", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        //TODOreq: "current price is XXX and it will be back at YYYYY at ZZZZZZZ (should nobody buy any further slots). Click here to refresh this information to see if it's still valid, as someone may have purchased a slot since you loaded this page (enable javascript if you want to see it count down automatically)"
    }
    new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
    new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
    WPushButton *buySlotFillerStep1Button = new WPushButton("Buy At This Price (Step 1/2)", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
    buySlotFillerStep1Button->clicked().connect(this, &AnonymousBitcoinComputingWtGUI::buySlotStep1d3faultCampaign0ButtonClicked);
}
void AnonymousBitcoinComputingWtGUI::buySlotStep1d3faultCampaign0ButtonClicked()
{
    if(!m_LoggedIn)
    {
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WText("Log In First", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        return;
    }
    getCouchbaseDocumentByKeyBegin("adSpaceAllSlotFillers" + m_Username.toUTF8()); //TODOreq: obviously we'd have sanitized the username by here...
    m_WhatTheGetWasFor = HACKEDIND3FAULTCAMPAIGN0BUYSTEP1GET;
}
void AnonymousBitcoinComputingWtGUI::buySlotPopulateStep2d3faultCampaign0(const std::string &allSlotFillersJsonDoc)
{
    if(!m_AllSlotFillersComboBox)
    {
        //TODOreq: give error if no slotFillers have been set up, and instruct them to do so (with link to page to do it)

        ptree pt;
        std::istringstream is(allSlotFillersJsonDoc);
        read_json(is, pt);

        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WText("Select which advertisement you want to use (you cannot change this later):", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);

        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        m_AllSlotFillersComboBox = new WComboBox(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        BOOST_FOREACH(const ptree::value_type &child, pt.get_child("allSlotFillers"))
        {
            const std::string &slotFillerNickname = child.second.get<std::string>("nickname");
            const std::string &slotFillerIndex = child.second.get<std::string>("slotFillerIndex");
            m_AllSlotFillersComboBox->insertItem(atoi(slotFillerIndex.c_str()), slotFillerNickname); //TODOreq: Wt probably/should already do[es] this, but the nicknames definitely need to be sanitized (when they're created). TODOreq: make sure atoi is working how i think it should
        }

        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WText("ARE YOU SURE? THERE'S NO TURNING BACK AFTER THIS", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        WPushButton *buySlotFillerStep2Button = new WPushButton("Buy At This Price (Step 2/2)", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        buySlotFillerStep2Button->clicked().connect(this, &AnonymousBitcoinComputingWtGUI::buySlotStep2d3faultCampaign0ButtonClicked);
    }
}
void AnonymousBitcoinComputingWtGUI::buySlotStep2d3faultCampaign0ButtonClicked()
{
    deferRendering(); //haha it hit me while laying in bed that you can call this multiple times. never thought i'd use that feature but blamo here i am using it and loving Wt :). TODOreq: make sure the 'second' deferRendering is called in all error cases from here
    //TO DOnereqopt: idk if required or optional or optimization, but it makes sense that we consolidate the defer/resume renderings here. The result of that button clicked (the signal that brought us here) makes us do TWO couchbase round trips: the first one to verify balance and lock account, the second to do the actual slot-filling/buying. It makes sense to only defer rendering once HERE, and to resume rendering only once LATER (error, or slot filled, etc). It might not matter at all.

    //DO ACTUAL BUY (associate slot with slot filler, aka fill the slot)

   //TODOreq: this doesn't belong here, but the combo box probably depends on the user to send in the slotfiller index, so we need to sanitize that input (also applies to nickname if it is used (i don't currently plan to))

    new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
    new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
    new WText("You're attempting to buy slot with filler: " + m_AllSlotFillersComboBox->currentText(), m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
    new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);

    //TODOreq: somehow we need the 'currentprice' (what they see) from the javascript/timer shit. might need a JSignal in that case...

    //TODOreq: calculate INTERNAL current price, make sure it's lower than the currentPrice they submitted/saw (JSignal prolly), then go ahead with buy. The accuracy of this is going to be stupid hard to attain. Since the javascript uses the end-user's system time... and 'sending the time' to them will have latency issues, there is no proper solution (sending the time has the benefit of not depending on the user's system time to be correct (we wouldn't want to give them the false impression that they're getting a low as fuck price (and this is the reason they should send in the price THEY SEE)))
    //TODOreq: change to 'sending them the time' [for use in js], because it not only solves the timezones/incorrect-system-time problem, but it also has the added benefit of GUARANTEEING that the price they see will be above the internal price (because of latency)... so much so that we don't need them to send it in anymore. the slot index would be enough. Also worth noting that the js should use an accurate timer to calculate the 'current time' (from the time we sent them and they saved). I doubt setInterval is a high precision timer. I was originally thinking they could delta the time we sent them against their system time, but then there'd be the problem of if they changed their system time while running then it'd fuck shit up (not a big deal though since it's a rare case. still if js has a high precision timer object ("time elapsed since x"), use that).
    //^that guarantee depends on all wt nodes being time sync'd like fuck, but that goes without saying

    //TODOreq: handle cases where no puchases yet (use min), no next, no balls, current expired (use min),etc

    //had calculate current price here, but moved it to a few sub-milliseconds later

    //TODOreq: sanitize slot index (user could have forged), verify it in fact exists (was thinking i should try to pull the doc, BUT we already have the "allSlotFillers" doc and can just verify that it's in that instead (so we would be depending on earlier sanity checks when setting up the slotFiller instead)

    std::ostringstream slotIndexStream;
    slotIndexStream << m_AllSlotFillersComboBox->currentIndex();
    m_SlotFillerToUseInBuy = "adSpaceSlotFillers" + m_Username.toUTF8() + slotIndexStream.str();

    //TODOreq: make a query for the user's "account", to both cas-lock it and to see that the balance is high enough. hmm i don't need the cas value presented to me in Wt land, but i do need it to be held on to for continuing with the cas swap shit after i make sure balance is high enough (i suppose i can just pass the currentPrice to the backend and have him verify balance > currentPrice (but up till now, my backend has been app agnostic and therefore portable. i suppose i need an app-specific backend? perhaps isA agnostic db type? so much flexibility it hurts. KISS). really though, cas-swap(lock) is NOT abc-specific, so implementing that in my backend is good. maybe a generic "getCouchbaseDocumentByKeyAndHangOntoItBecauseIMightCasSwapIt" (TODOreq: account for when i DON'T cas-swap it (in this example, if the balance is too low, or if it's already 'locked')). Hmm the CAS value is just a uint64_t, so maybe it won't be so troublesome to bring it to wt side and then send it back to couchbase [in a new 'store' request])
    getCouchbaseDocumentByKeySavingCasBegin("user" + m_Username.toUTF8());
    m_WhatTheGetSavingCasWasFor = HACKEDIND3FAULTCAMPAIGN0BUYSTEP2aVERIFYBALANCEANDGETCASFORSWAPLOCKGET;

    //TODOreq: the above get shouldn't fail since they need to already be logged in to get this far, but what the fuck do i know? it probably CAN (db overload etc), so i need to account for that
    //^everything can DEFINITELY fail, it is only success that can ever at most PROBABLY happen

    //TODOreq: failed to lock your account for the purchase (???), failed to buy/fill the slot (insufficient funds, or someone beat us to it (unlock in both cases))
}
void AnonymousBitcoinComputingWtGUI::verifyUserHasSufficientFundsAndThatTheirAccountIsntAlreadyLockedAndThenStartTryingToLockItIfItIsntAlreadyLocked(const string &userAccountJsonDoc, u_int64_t cas)
{
    m_UserAccountLockedDuringBuyJson = userAccountJsonDoc; //our starting point for when we debit the user account during unlock (after the slot fill later on)
    ptree pt;
    std::istringstream is(userAccountJsonDoc);
    read_json(is, pt);

    std::string ALREADY_LOCKED_CHECK_slotToAttemptToFillAkaPurchase = pt.get<std::string>("slotToAttemptToFillAkaPurchase", "n");
    if(ALREADY_LOCKED_CHECK_slotToAttemptToFillAkaPurchase == "n")
    {
        //not already locked
        std::string userBalanceString = pt.get<std::string>("balance");
        double userBalance = strtod(userBalanceString.c_str(), 0);

        //TO DOnereq: we should probably calculate balance HERE/now instead of in buySlotStep2d3faultCampaign0ButtonClicked (where it isn't even needed). Those extra [SUB ;-P]-milliseconds will get me millions and millions of satoshis over time muahhahaha superman 3

        //calculate internal price
        //y2
        double minPriceDouble = strtod(m_HackedInD3faultCampaign0_MinPrice.c_str(), 0);
        //y1
        double lastSlotFilledAkaPurchasedPurchasePrice_Doubled = (strtod(m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedPurchasePrice.c_str(), 0)*2.0);
        //x2
        double lastSlotFilledAkaPurchasedExpireDateTime = (strtod(m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedStartTimestamp.c_str(), 0)+((double)(strtod(m_HackedInD3faultCampaign0_SlotLengthHours.c_str(),0)*(3600.0))));
        //x1
        double lastSlotFilledAkaPurchasedPurchaseDateTime = strtod(m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedPurchaseTimestamp.c_str(), 0);
        //m = (y2-y1)/(x2-x1);
        double m = (minPriceDouble-lastSlotFilledAkaPurchasedPurchasePrice_Doubled)/(lastSlotFilledAkaPurchasedExpireDateTime-lastSlotFilledAkaPurchasedPurchaseDateTime);
        //b = y-(m*x)
        double b = (minPriceDouble - (m * lastSlotFilledAkaPurchasedExpireDateTime));

        m_CurrentPriceToUseForBuying = (m*((double)WDateTime::currentDateTime().toTime_t()))+b;

        std::ostringstream lastSlotFilledAkaPurchasedExpireDateTimeBuffer;
        lastSlotFilledAkaPurchasedExpireDateTimeBuffer << lastSlotFilledAkaPurchasedExpireDateTime;
        m_LastSlotFilledAkaPurchasedExpireDateTime_ToBeUsedAsStartDateTimeIfTheBuySucceeds = lastSlotFilledAkaPurchasedExpireDateTimeBuffer.str();

        //TODOreq: check current not expired or no purchases etc (use min)

        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        std::ostringstream currentPriceStream; //TODOreq: all other number->string conversions should use this method
        //TODOreq: why the fuck does setprecision(6) give me 8 decimal places and setprecision(8) gives me 10!?!?!? lol C++. BUT SERIOUSLY THOUGH I need to make sure that this doesn't fuck up shit and money get leaked/lost/whatever. Maybe rounding errors in this method of converting double -> string, and since I'm using it as the actual value in the json doc, I need it to be accurate. The very fact that I have to use 6 instead of 8 just makes me wonder...
        //TODOreq: ^ok wtf now i got 7 decimal places, so maybe it's dependent on the value........... maybe i should just store/utilize as many decimal places as possible (maybe trimming to 8 _ONLY_ when the user sees the value).... and then force the bitcoin client to do the rounding shizzle :-P
        currentPriceStream /*<< setprecision(6)*/ << m_CurrentPriceToUseForBuying; //TODOreq:rounding errors maybe? I need to make a decision on that, and I need to make sure that what I tell them it was purchased at is the same thing we have in our db
        m_CurrentPriceToUseForBuyingString = currentPriceStream.str();
        new WText("Internal Price Calculated At (should match above): " + m_CurrentPriceToUseForBuyingString, m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);


        if(userBalance >= m_CurrentPriceToUseForBuying) //idk why but this makes me cringe. suspicion of rounding errors and/or other hackability...
        {
            //proceed with trying to lock account

            //make 'account locked' json doc [by just appending to the one we already have]
            int oldSlotIndex = atoi(m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedSlotIndex.c_str()); //TODOoptimization: and what happens after 4294967296 days (which is 11767033 years (ok nvm (but still, if I make it public then people could change the slot length to one hour etc)))!?!?
            ++oldSlotIndex; //now new slot index :)
            std::ostringstream slotIndexIntToStringBuffer;
            slotIndexIntToStringBuffer << oldSlotIndex;
            m_AdSlotIndexToUseInPurchaseAndInUpdateCampaignDocAfterPurchase = slotIndexIntToStringBuffer.str();
            m_AdSlotAboutToBeFilledIfLockIsSuccessful = "adSpaceSlotsd3fault0Slot" + m_AdSlotIndexToUseInPurchaseAndInUpdateCampaignDocAfterPurchase;
            pt.put("slotToAttemptToFillAkaPurchase", m_AdSlotAboutToBeFilledIfLockIsSuccessful);
            //TODOreq (read next TODOreq first): seriously it appears as though i'm still not getting the user's input to verify the slot index. we could have a 'buy event' that could update m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedSlotIndex and even push it to their client just milliseconds before they hit 'buy' step 2.... and if they have sufficient funds they'd now buy at near-exactly twice what they wanted to [and be pissed]. i need some boolean guards in javascript surrounding a "are you sure" prompt thingo (except i can't/shouldn't depend on js so gah) -> jsignal-emit-with-that-value -> unlock the boolean guards (to allow buy events to update that slot index and/or price). an amateur wouldn't see this HUGE bug
            //TODOreq: ^bleh, i should 'lock in the slot index' when the user clicks buy step 1, DUH (fuck js) (if they receive a buy event during that time, we undo step 1, requiring them to click it again [at a now double price]. BUT it's _VITAL_ that i don't allow the internal code to modify their locked in slot index and allow them to proceed forward with the buy)
            pt.put("priceToAttemptToBuyItFor", m_CurrentPriceToUseForBuyingString);
            pt.put("slotToAttemptToFillAkaPurchaseItWith", m_SlotFillerToUseInBuy);
            std::ostringstream jsonDocBuffer;
            write_json(jsonDocBuffer, pt, false);
            std::string accountLockedForBuyJsonDoc = jsonDocBuffer.str();

            setCouchbaseDocumentByKeyWithInputCasBegin("user" + m_Username.toUTF8(), accountLockedForBuyJsonDoc, cas, AddCouchbaseDocumentByKeyRequest::SaveOutputCasMode);
            m_WhatTheSetWithInputCasSavingOutputCasWasFor = HACKEDIND3FAULTCAMPAIGN0BUYSTEP2bLOCKACCOUNTFORBUYINGSETWITHCASSAVINGCAS;
        }
        else
        {
            //TODOreq: give insufficient funds message, instruct them to go to page to add funds (TODOreq: doesn't belong here but i probalby shouldn't let them do ANY purchase activity when they have 'pending'/unconfirmed (bitcoin) funds. It just complicates the logic too much and I'm probably going to use account locking for that too)
        }
    }
    else
    {
        //TODOreq: account already locked so error out of this buy. they're logged in elsewhere and trying to buy two things at once? etc. It jumps at me that it might be a place to do 'recovery' code, but I think for now I'm only going to do that during 'login' (TODOreq: maybe doing recovery at login isn't such a good idea (at least, the PROCEED WITH BUY kind isn't a good idea (rollback is DEFINITELY good idea (once we verify that they didn't get it(OMG RACE CONDITION HACKABLE TODOreq: they are on two machines they log in on a different one just after clicking "buy" and get lucky so that their account IS locked, but the separate-login-machine checks to see if they got the slot. When it sees they didn't, it rolls back their account. Meanwhile the original machine they clicked "buy" on is still trying to buy the slot (by LCB_ADD'ing the slot). The hack depends on the machine they press "buy" on being slower (more load, etc) than the alternate one they log in to (AND NOTE, BY MACHINE I MEAN WT SERVER, not end user machine). So wtf rolling back is dangerous? Wat do. It would also depend on the buying machine crashing immediately after the slot is purchased, because otherwise it would be all like 'hey wtf who unlocked that account motherfucker, I was still working on it' and at least error out TODOreq))))
        //^very real race condition vuln aside, what i was getting at originally is that maybe it's not a good idea to do "recover->proceed-with-buy" because who the fuck knows how much later they'd log in... and like maybe currentPrice would be waaaaay less than when they originally locked/tried-and-failed. It makes sense to at least ask them: "do you want to try this buy again" and then to also recalculate the price (which means re-locking the account (which probably has security considerations to boot)) on login
    }
}
void AnonymousBitcoinComputingWtGUI::nowThatTheUserAccountIsLockedDoTheActualSlotFillAdd(u_int64_t casFromLockSoWeCanSafelyUnlockLater)
{
    m_CasFromUserAccountLockSoWeCanSafelyUnlockLater = casFromLockSoWeCanSafelyUnlockLater;
    //TODOreq: unlock user account after successful add
    //TODOreq: handle beat to punch error case (unlock without deducting)
    //TODOreq: handle lock failed error case

    //do the LCB_ADD for the slot!
    ptree pt;
    std::ostringstream timestampStream;
    timestampStream << WDateTime::currentDateTime().toTime_t();
    m_PurchaseTimestampForUseInSlotItselfAndAlsoUpdatingCampaignDocAfterPurchase = timestampStream.str();
    pt.put("purchaseTimestamp", m_PurchaseTimestampForUseInSlotItselfAndAlsoUpdatingCampaignDocAfterPurchase);
    pt.put("purchasePrice", m_CurrentPriceToUseForBuyingString);
    pt.put("slotFilledWith", m_SlotFillerToUseInBuy);
    std::ostringstream jsonDocBuffer;
    write_json(jsonDocBuffer, pt, false);
    storeCouchbaseDocumentByKeyBegin(m_AdSlotAboutToBeFilledIfLockIsSuccessful, jsonDocBuffer.str());
    m_WhatTheAddWasFor = BUYAKAFILLSLOTWITHSLOTFILLERADD;
}
void AnonymousBitcoinComputingWtGUI::successfulSlotFillAkaPurchaseAddIsFinishedSoNowDoUnlockUserAccountWhileSubtractingAmount()
{
    //TODOreq: error cases, such as getting beat to the punch
    new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
    new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
    new WText("Gratz brah, you bought a slot for BTC: " + m_CurrentPriceToUseForBuyingString, m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);

    //TODOreq: user account unlock -- i worry that the user would be able to fuck with the state from the time they hit buy2 -> now (and keeping the rendering deferred until now might be the solution)... like i can't quite put my finger on it, but something to do with "when we repopulate the json doc for unlocking the account, they've modified something so that now something unintentional happens" (a big one being balance not being deducted properly, but it could be something else subtler)

    //user account debiting + unlock
    //first parse the json doc that we saved before doing the lock
    ptree pt;
    std::istringstream is(m_UserAccountLockedDuringBuyJson);
    read_json(is, pt);
    //now do the debit of the balance and put it back in the json doc
    double balancePrePurchase = strtod(pt.get<std::string>("balance").c_str(), 0);
    balancePrePurchase -= m_CurrentPriceToUseForBuying; //TODOreq: again, just scurred of rounding errors etc. I think as long as I  use 'more precision than needed' (as much as a double provides), I should be ok...
    std::ostringstream balancePostPurchaseBuffer;
    balancePostPurchaseBuffer << balancePrePurchase;
    pt.put("balance", balancePostPurchaseBuffer.str());
    //now convert the json doc back to string for couchbase
    std::ostringstream jsonDocWithBalanceDeducted;
    write_json(jsonDocWithBalanceDeducted, pt, false);

    setCouchbaseDocumentByKeyWithInputCasBegin("user" + m_Username.toUTF8(), jsonDocWithBalanceDeducted.str(), m_CasFromUserAccountLockSoWeCanSafelyUnlockLater);
    m_WhatTheSetWithInputCasWasFor = HACKEDIND3FAULTCAMPAIGN0BUYPURCHASSUCCESSFULSOUNLOCKUSERACCOUNTSAFELYUSINGCAS; //TODOreq: handle cas swap UNLOCK failure, should never happen but can

    //TO DOnereq(set without cas): should we have done a multi-get to update the campaign doc also? i actually think not since we don't have the cas for that doc (but we could have gotten it~). more importantly i don't know whether or not i need the cas for it when i do the swap. seems safer no doubt.. but really would there be any contesting?? maybe only subsequent buys milliseconds later (probably in error, but maybe the campaign is just liek popular ya know ;-P)... so yea TODOreq do a cas-swap of the campaign doc, makes sure we're only n+1 the last purchased.... and i think maybe do an exponential backoff trying... because it's the same code that runs for when subsequent ones are purchased seconds later? or no... maybe we just do a set without cas because we know that all attempts to buy will fail until that set is complete (because they try to buy n+1 (which is what we just bought (so it will fail safely as beat-to-thepunch)), not n+2 (until the set we're about to do, is done))
    //TODOreq:^^Does the order of user-account-unlock and setting-the-campaign-doc-with-new-last-purchase matter? can we do them asynchronously (AKA HERE IS WHERE WE'D START 'UPDATING'/LCB_SET'ing CAMPAIGN), or do we need to do one before the other and then wait for whichever goes first to complete?


    //TODOreq: need to update the adSpaceSlotsd3fault0 to make the filler that just succeded the most recent purchased
    //TODOreq: dispatch the buy event to our neighbor wt nodes so they can post the buy event to their end users etc (how da fuq? rpc? get-and-subscribe polling model? guh). polling is easiest and really having N[ode-count] hits ever i[N]terval [m]seconds is still not that expensive (get and subscribe also acts as caching layer so that we don't get 10-freaking-thousand hits per second to that same document)
    //TODOreq: definitely doesn't belong here, but a get-and-subscribe / polling method could do a "poll" for the current value and when the poll finishes, the last end-user could have disconnected in that time and we MIGHT think it would be an error but we'd be wrong. in that case we'd just discard/disregard that last polled value (and of course, not do any more polling until at least one end-user connected)
    //TODOreq: ^it goes without saying that 'buy' events do not use the cached value. (NVM:) Hell, even hitting 'buy step 1' should maybe even do a proper get for the actual value (or just nudge cache to do it for him).... BUT then it becomes a DDOS point. since I'm thinking the polling intervals will be like 100-500ms, doing a non-cached get like that on "buy step 1" is probably not necessary (/NVM). But we definitely absolutely need to do it for buy step 2 (duh), and that is NOT a DDOS point because noobs would be giving ya monay each time so i mean yea ddos all ya fuggan want nobs <3

    //TODOreq: even though it seemed like a small optimization to do both the user-account-unlock and updating of campaign doc with a new 'last purchased' slot, that pattern of programming will lead to disaster so i shouldn't do it just in principle. Yes with this one case it wouldn't have led to disaster, but actually it MIGHT have given extreme circumstances. it opens up a race condition: say the 'user account unlock' happens really fast and the 'update campaign doc' happens to take a while. if we gave control back to the user after the 'user account unlock' was finished, they could do some action that would then conflict with the m_WhatThe[blahblahblah]WasFor related to the 'update campaign doc' store. As in, when the 'update campaign doc' finishes and gets posted back to the WApplication, the user's actions could have started something else and then we wouldn't have handled the finishing of the 'update campaign doc' properly (which would mean that we never notify our neighbors of buy event? actually not a problem if we do 'get-and-subscribe-polling' like i think i'm going to do (BUT LIKE I SAID THIS ONE GETS LUCKY, BUT AN OVERALL PATTERN TO AVOID)). So I'm going to do the successive sets synchronously, despite seeing a clear opportunity for optimization
}
void AnonymousBitcoinComputingWtGUI::doneUnlockingUserAccountAfterSuccessfulPurchaseSoNowUpdateCampaignDocSettingOurPurchaseAsLastPurchase()
{
    //TODOreq: need a recovery path that both SAFELY (see vuln above) debits user account and also updates campaign doc.

    //we already have the campaign doc because we were looking at it when we hit buy step 1!
    ptree pt;
    std::istringstream is(m_HackedInD3faultCampaign0JsonDocForUpdatingLaterAfterSuccessfulPurchase);
    read_json(is, pt);

    //boost::optional<ptree&> lastSlotFilledAkaPurchased = pt.get_child_optional("lastSlotFilledAkaPurchased");
    //boost::optional<ptree&> currentSlotOnDisplay = pt.get_child_optional("currentSlotOnDisplay");
    //boost::optional<ptree&> nextSlotOnDisplay = pt.get_child_optional("nextSlotOnDisplay");
#if 0
    if(lastSlotFilledAkaPurchased.is_initialized())
    {

    }
    else
    {
        //first purchase, so basically the same thing but we don't modify the existing...
    }
#endif
    ptree lastPurchasedPt;
    lastPurchasedPt.put("slotIndex", m_AdSlotIndexToUseInPurchaseAndInUpdateCampaignDocAfterPurchase);
    lastPurchasedPt.put("purchaseTimestamp", m_PurchaseTimestampForUseInSlotItselfAndAlsoUpdatingCampaignDocAfterPurchase);
    lastPurchasedPt.put("startTimestamp", m_LastSlotFilledAkaPurchasedExpireDateTime_ToBeUsedAsStartDateTimeIfTheBuySucceeds);
    lastPurchasedPt.put("purchasePrice", m_CurrentPriceToUseForBuyingString);

    pt.put_child("lastSlotFilledAkaPurchased", lastPurchasedPt);

    //TODOreq: we also need to MAYBE update 'current' and/or 'next', depending on if it's appropriate. the only one we DEFINITELY update is 'last purchased'. if current is expired and there is a next -- holy race condition batman TODOreq (maybe solution is to not list "next" in the doc... and shit i'm actually starting to think maybe i shouldn't even list current. maybe whether or not there is a current is detected on the fly and when needed... by seeing if the (slotIndex-of-the-just-expired-current)+1 exists.. and if it doesn't then shit we don't have a current :-P... yea actually i'm liking that more and more :). simplifies managing the campaign doc too, only two states: no purchase and last purchase (whether it's expired is not that relevant to us, except maybe later when doing a buy announce event (but... polling...?). STILL, I do feel as though I need to keep track of 'current' SOMEWHERE. My thoughts are drifting and although it does make sense to do the 'get n+1' and that's our next if it exists method, who and when exactly would that be done? If it's the "client" of abc (so like the page where i'm fucking dancing naked coding tripping balls on video), who initiates the "ok expired now gimmeh next" and then abc does the n+1 shit,... err i guess what i mean is who has the final say of what n is to begin with? should for example the client die and come back and forget n, how would it figure out it's state again? I GUESS we could just keep doing n+1 or n-1 until we found it, and hell we could even use hella accurate jumping to the middle of all of our slots based on SlotLength, slot0 start time (polled), and the current date time. Would be accurate as fuck list jump with maybe an extra get to correct off by one. SO maybe the abc client does keep track of "n" and then gives it to abc as a parameter a la "hint". Client(dance-vid-page): abc.getNextSlot(n_justExpired /*or n+1, but it doesn't matter who does the incrementing*/), and if it's 0 (state lost somehow) then abc does the list jump mechanism to find it asap. but guh, i haven't worked out any of the details of the abc client (dance-vid-page), so idfk what it's state or code or ANYTHING even looks like on that side of things...
    //^regardless of what i choose, the fact that we can figure out the current slot by doing math with near 100% accuracy (3 gets tops i'd estimate (first for slot 0, second for our guess based on math (so yea 4 gets if you include campaign doc get), third as potential recovery from mistake in math (timezones? idfk...) -- assumes slotLengthHours never changes of course...
    //^^i think i'll cross that bridge when i get there (buy event and 'gimmeh ad plx client request'). dats how da hackas doeet (that being said, i should now consider current and next to NOT be in the campaign json doc (will re-add later if determined i should))

    //if current is expired and there is no next, we fill our latest purchase in as the current
    //if

    std::ostringstream updatedCampaignJsonDocBuffer;
    write_json(updatedCampaignJsonDocBuffer, pt, false);

    //you might think this should use CAS input, but the only way (orly? TODOreq race condition 'recovery process' (lol i'm starting to think a journal is the answer to all this nonsense, but meh now[/then] i['d] have two problems)) it could get updated is if there was another purchase... but future purchases DEPEND on this being set first... so...
    storeCouchbaseDocumentByKeyBegin("adSpaceSlotsd3fault0", updatedCampaignJsonDocBuffer, AddCouchbaseDocumentByKeyRequest::StoreNoCasMode);
    m_WhatTheSetWasFor = ;

    resumeRendering(); //TODOreq: move this to be after the very last thing we do (confirm that campaign doc is updated)
}
void AnonymousBitcoinComputingWtGUI::getCouchbaseDocumentByKeyBegin(const std::string &keyToCouchbaseDocument)
{
    deferRendering();
    GetCouchbaseDocumentByKeyRequest couchbaseRequest(sessionId(), this, keyToCouchbaseDocument, GetCouchbaseDocumentByKeyRequest::DiscardCASMode);
    SERIALIZE_COUCHBASE_REQUEST_AND_SEND_TO_COUCHBASE_ON_RANDOM_MUTEX_PROTECTED_MESSAGE_QUEUE(Get, GET)
}
void AnonymousBitcoinComputingWtGUI::getCouchbaseDocumentByKeySavingCasBegin(const string &keyToCouchbaseDocument)
{
    deferRendering();
    GetCouchbaseDocumentByKeyRequest couchbaseRequest(sessionId(), this, keyToCouchbaseDocument, GetCouchbaseDocumentByKeyRequest::SaveCASMode);
    SERIALIZE_COUCHBASE_REQUEST_AND_SEND_TO_COUCHBASE_ON_RANDOM_MUTEX_PROTECTED_MESSAGE_QUEUE(Get, GET)
}
//original code with comments
//S3RIALIZE_COUCHBASE_REQUEST_AND_SEND_TO_COUCHBASE_ON_RANDOM_MUTEX_PROTECTED_MESSAGE_QUEUE
#if 0
    std::ostringstream couchbaseRequestSerialized;

    {
        boost::archive::text_oarchive serializer(couchbaseRequestSerialized);
        serializer << couchbaseRequest;
    }

    std::string couchbaseRequesSerializedString = couchbaseRequestSerialized.str();

    size_t couchbaseRequesSerializedStringLength = couchbaseRequesSerializedString.length();
    if(couchbaseRequesSerializedStringLength > WT_TO_COUCHBASE_GET_MAX_MESSAGE_SIZE) //assumes 1 character is 1 byte... i think this is right because it worked down below for our c_str() passed in, which i'm pretty damn sure is 1ch=1by
    {
        //TODOreq: handle serialized buffer over maximum message size properly
        return;
    }

    ++m_CurrentGetMessageQueueIndex;
    if(m_CurrentGetMessageQueueIndex == NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES)
    {
        m_CurrentGetMessageQueueIndex = 0;
    }
    int lockedMutexIndex = m_CurrentGetMessageQueueIndex;
    int veryLastMutexIndexToBlockLock = (m_CurrentGetMessageQueueIndex == 0 ? (NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES-1) : (m_CurrentGetMessageQueueIndex-1));
    while(true)
    {
        if(lockedMutexIndex == veryLastMutexIndexToBlockLock)
        {
            m_GetMutexArray[lockedMutexIndex].lock();
            break;
        }
        if(m_GetMutexArray[lockedMutexIndex].try_lock())
        {
            break;
        }
        ++lockedMutexIndex;

        //loop around
        if(lockedMutexIndex == NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES)
        {
            lockedMutexIndex = 0;
        }
    }

    m_GetWtMessageQueues[lockedMutexIndex]->send(couchbaseRequesSerializedString.c_str(), couchbaseRequesSerializedStringLength, 0);

    event_active(m_GetEventCallbacksForWt[lockedMutexIndex], EV_READ|EV_WRITE, 0);
    m_GetMutexArray[lockedMutexIndex].unlock();
#endif
//despite one of the parameters mentioning CAS input, you shouldn't use this method if you want to use CAS input. You probably want one of the set* variants
void AnonymousBitcoinComputingWtGUI::storeCouchbaseDocumentByKeyBegin(const std::string &keyToCouchbaseDocument, const std::string &couchbaseDocument, AddCouchbaseDocumentByKeyRequest::LcbStoreMode_AndWhetherOrNotThereIsInputCasEnum storeMode = AddCouchbaseDocumentByKeyRequest::AddMode)
{
    deferRendering();
    AddCouchbaseDocumentByKeyRequest couchbaseRequest(sessionId(), this, keyToCouchbaseDocument, couchbaseDocument, storeMode);
    SERIALIZE_COUCHBASE_REQUEST_AND_SEND_TO_COUCHBASE_ON_RANDOM_MUTEX_PROTECTED_MESSAGE_QUEUE(Add, ADD)
}
void AnonymousBitcoinComputingWtGUI::setCouchbaseDocumentByKeyWithInputCasBegin(const string &keyToCouchbaseDocument, const string &couchbaseDocument, u_int64_t cas, AddCouchbaseDocumentByKeyRequest::WhatToDoWithOutputCasEnum whatToDoWithOutputCasEnum = AddCouchbaseDocumentByKeyRequest::DiscardOuputCasMode)
{
    deferRendering();
    AddCouchbaseDocumentByKeyRequest couchbaseRequest(sessionId(), this, keyToCouchbaseDocument, couchbaseDocument, AddCouchbaseDocumentByKeyRequest::StoreWithCasMode, cas, whatToDoWithOutputCasEnum);
    SERIALIZE_COUCHBASE_REQUEST_AND_SEND_TO_COUCHBASE_ON_RANDOM_MUTEX_PROTECTED_MESSAGE_QUEUE(Add, ADD)
}
void AnonymousBitcoinComputingWtGUI::getCouchbaseDocumentByKeyFinished(const std::string &keyToCouchbaseDocument, const std::string &couchbaseDocument)
{
    resumeRendering();
    //this hack STILL makes me giggle like a little school girl, tee hee
    switch(m_WhatTheGetWasFor)
    {
    case HACKEDIND3FAULTCAMPAIGN0GET:
        {
            finishShowingAdvertisingBuyAdSpaceD3faultCampaign0Widget(couchbaseDocument);
        }
        break;
    case LOGINATTEMPTGET:
        {
            loginIfInputHashedEqualsDbInfo(couchbaseDocument);
        }
        break;
    case HACKEDIND3FAULTCAMPAIGN0BUYSTEP1GET:
        {
            buySlotPopulateStep2d3faultCampaign0(couchbaseDocument);
        }
        break;
    case INITIALINVALIDNULLGET:
    default:
        cerr << "got a couchbase 'get' response we weren't expecting:" << endl << "unexpected key: " << keyToCouchbaseDocument << endl << "unexpected value: " << couchbaseDocument << endl;
        break;
    }
    //if(environment().ajax())
    //{
    //    triggerUpdate(); //this and enableUpdates are probably not needed if i'm always defer/resuming
    //}
}
void AnonymousBitcoinComputingWtGUI::getCouchbaseDocumentByKeySavingCasFinished(const string &keyToCouchbaseDocument, const string &couchbaseDocument, u_int64_t cas)
{
    resumeRendering();
    switch(m_WhatTheGetSavingCasWasFor)
    {
    case HACKEDIND3FAULTCAMPAIGN0BUYSTEP2aVERIFYBALANCEANDGETCASFORSWAPLOCKGET:
        {
            verifyUserHasSufficientFundsAndThatTheirAccountIsntAlreadyLockedAndThenStartTryingToLockItIfItIsntAlreadyLocked(couchbaseDocument, cas);
        }
        break;
    case INITIALINVALIDNULLGETSAVINGCAS:
    default:
        cerr << "got a couchbase 'get' (saving cas) response we weren't expecting:" << endl << "unexpected key: " << keyToCouchbaseDocument << endl << "unexpected value: " << couchbaseDocument << endl << "unexpected cas: " << cas << endl;
        break;
    }
    //if(environment().ajax())
    //{
    //    triggerUpdate(); //this and enableUpdates are probably not needed if i'm always defer/resuming
    //}
}
void AnonymousBitcoinComputingWtGUI::addCouchbaseDocumentByKeyFinished(const string &keyToCouchbaseDocument)
{
    //no need to pass in the value, and we probably don't even need the key here... but might in the future. i do know that if i do the enableUpdates/triggerUpdate async design that i would probably very much need the key at least here. imagine they dispatch two "add" requests before the first one can return. but meh that's a pretty big design overhaul as it is so not worrying about it right now (if only there was a generator that could...)
    resumeRendering();
    switch(m_WhatTheAddWasFor)
    {
    case REGISTERATTEMPTADD:
    {
        //TODOreq: error cases (user already exists, time outs, etc...)

        WContainerWidget *registerSuccessfulWidget = new WContainerWidget(m_MainStack);
        new WText("Welcome to Anonymous Bitcoin Computing, " + m_RegisterUsername->text() + ". You can now log in.", registerSuccessfulWidget);

        new WBreak(registerSuccessfulWidget);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_HOME), ABC_ANCHOR_TEXTS_PATH_HOME, registerSuccessfulWidget);

        //in case they browse back to it. TODOreq: probably should do this in other places as well
        m_RegisterUsername->setText("");
        m_RegisterPassword->setText("");

        m_MainStack->setCurrentWidget(registerSuccessfulWidget);

        //TODOoptimization: if the user keeps registering, logging in, logging out, registering new, etc etc.. and never navigates away.. then they still have the same "session" (even though yes, session id changes on login) and we'll be adding a bunch of registerSuccessFulWidgets which never get deleted until they navigate away and the session is destroyed. BUT REALLY at that point it's probably a larger concern all the database they're wasting xD. I had the below ifdef'd out code, but it doesn't account for logout->register new user. The old logged out username would still be shown. Lots of ways to deal with this problem, fuck it for now. Not technically a memory leak...
#if 0
        if(!m_RegisterSuccessfulWidget)
        {
            m_RegisterSuccessfulWidget = new WContainerWidget(m_MainStack);
            new WText("Welcome to Anonymous Bitcoin Computing, " + m_RegisterUsername->text() + ". You can now log in.", m_RegisterSuccessfulWidget);

            //in case they browse back to it. TODOreq: probably should do this in other places as well
            m_RegisterUsername->setText("");
            m_RegisterPassword->setText("");
        }
        m_MainStack(m_RegisterSuccessfulWidget);
#endif
    }
        break;
    case BUYAKAFILLSLOTWITHSLOTFILLERADD:
    {
        successfulSlotFillAkaPurchaseAddIsFinishedSoNowDoUnlockUserAccountWhileSubtractingAmount();
    }
        break;
    case INITIALINVALIDNULLADD:
    default:
        cerr << "got a couchbase 'add' response we weren't expecting:" << endl << "unexpected key: " << keyToCouchbaseDocument << endl;
        break;
    }
    //if(environment().ajax())
    //{
    //    triggerUpdate();
    //}
}
void AnonymousBitcoinComputingWtGUI::setCouchbaseDocumentByKeyWithInputCasFinished(const string &keyToCouchbaseDocument)
{
    resumeRendering();
    switch(m_WhatTheSetWithInputCasWasFor)
    {
    case HACKEDIND3FAULTCAMPAIGN0BUYPURCHASSUCCESSFULSOUNLOCKUSERACCOUNTSAFELYUSINGCAS:
    {
            doneUnlockingUserAccountAfterSuccessfulPurchaseSoNowUpdateCampaignDocSettingOurPurchaseAsLastPurchase();
    }
        break;
    case INITIALINVALIDNULLSETWITHCAS:
    default:
        cerr << "got a couchbase 'set' with input cas response we weren't expecting:" << endl << "unexpected key: " << keyToCouchbaseDocument << endl;
        break;
    }
    //if(environment().ajax())
    //{
    //    triggerUpdate();
    //}
}
void AnonymousBitcoinComputingWtGUI::setCouchbaseDocumentByKeyWithInputCasSavingOutputCasFinished(const string &keyToCouchbaseDocument, u_int64_t outputCas)
{
    resumeRendering();
    switch(m_WhatTheSetWithInputCasSavingOutputCasWasFor)
    {
    case HACKEDIND3FAULTCAMPAIGN0BUYSTEP2bLOCKACCOUNTFORBUYINGSETWITHCASSAVINGCAS:
    {
        nowThatTheUserAccountIsLockedDoTheActualSlotFillAdd(outputCas);
    }
        break;
    case INITIALINVALIDNULLSETWITHCASSAVINGCAS:
    default:
        cerr << "got a couchbase 'set' with input cas saving output cas response we weren't expecting:" << endl << "unexpected key: " << keyToCouchbaseDocument << endl;
        break;
    }
    //if(environment().ajax())
    //{
    //    triggerUpdate();
    //}
}
bool AnonymousBitcoinComputingWtGUI::isHomePath(const std::string &pathToCheck)
{
    if(pathToCheck == "" || pathToCheck == "/" || pathToCheck == ABC_INTERNAL_PATH_HOME)
    {
        return true;
    }
    return false;
}
void AnonymousBitcoinComputingWtGUI::handleInternalPathChanged(const std::string &newInternalPath)
{
    if(isHomePath(newInternalPath)) //why do we have this both here and in the constructor? because setInternalPath() does not go to/through the constructor, so showHome() would never be called if they click a link etc that does setInternalPath("/home"). They'd only be able to get there by navigating directly to the site/home without a session (which is the common case but yea~)
    {
        showHomeWidget();
        return;
    }
    if(internalPathMatches(ABC_INTERNAL_PATH_ADS))
    {
        if(internalPathMatches(ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE))
        {
            if(internalPathMatches(ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE_D3FAULT))
            {
                if(internalPathMatches(ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE_D3FAULT_CAMPAIGN_0))
                {
                    beginShowingAdvertisingBuyAdSpaceD3faultCampaign0Widget();
                    return;
                }
                showAdvertisingBuyAdSpaceD3faultWidget();
                return;
            }
            showAdvertisingBuyAdSpaceWidget();
            return;
        }
        showAdvertisingWidget();
        return;
    }
    if(newInternalPath == ABC_INTERNAL_PATH_REGISTER)
    {
        showRegisterWidget();
        return;
    }
    //TODOreq: 404
}
void AnonymousBitcoinComputingWtGUI::handleRegisterButtonClicked()
{
    //TODOreq: sanitize username at least (my json encoder doesn't like periods, for starters). since password is being b64 encoded we can probably skip sanitization (famous last words?)
    //TODOreq: do i need to base64 the passwordHash if i want to store it as json? methinks yes, because the raw byte array might contain a quote or a colon etc...

    std::string username = m_RegisterUsername->text().toUTF8();
    std::string passwordPlainText = m_RegisterPassword->text().toUTF8();

    //make salt
    std::string salt = sha1(username + uniqueId() + "saltplx739384sdfjghej9593859dffoiueoru584758958394fowuer732487587292" + WDateTime::currentDateTime().toString().toUTF8());
    //base64 salt for storage in json/couchbase
    std::string base64Salt = base64Encode(salt);
    //hash password using base64'd salt
    std::string passwordSaltHashed = sha1(passwordPlainText + base64Salt);
    //base64 hash for storage in json/couchbase
    std::string base64PasswordSaltHashed = base64Encode(passwordSaltHashed);
    //json'ify
    ptree jsonDoc;
    jsonDoc.put("passwordHash", base64PasswordSaltHashed);
    jsonDoc.put("passwordSalt", base64Salt);
    jsonDoc.put("balance", "0.0");
    //string'ify json
    std::ostringstream jsonDocBuffer;
    write_json(jsonDocBuffer, jsonDoc, false);
    std::string jsonString = jsonDocBuffer.str();

    storeCouchbaseDocumentByKeyBegin("user" + username, jsonString);
    m_WhatTheAddWasFor = REGISTERATTEMPTADD;
    //TODOreq: username already exists, invalid characters in username errors
}
void AnonymousBitcoinComputingWtGUI::handleLoginButtonClicked()
{
    //TODOreq: sanitize. keep in mind username is part of lots of keys, so maybe make it like 20 characters and keep in mind during key/doc design to make sure you don't make a key that is > 230 characters long (250 is max key length in couchbase)

    //TODOreq: it might make sense to clear the line edit's in the login widget, and to save the login credentials as member variables, and to also clear the password member variable (memset) to zero after it has been compared with the db hash (or even just after it has been hashed (so just make the hash a member xD))
    std::string username = m_LoginUsernameLineEdit->text().toUTF8();
    getCouchbaseDocumentByKeyBegin("user" + username);
    m_WhatTheGetWasFor = LOGINATTEMPTGET;
}
void AnonymousBitcoinComputingWtGUI::loginIfInputHashedEqualsDbInfo(const std::string &userProfileCouchbaseDocAsJson)
{
    ptree pt;
    std::istringstream is(userProfileCouchbaseDocAsJson);
    read_json(is, pt);

    std::string passwordHasBase64hFromDb = pt.get<std::string>("passwordHash");
    std::string passwordSaltInBase64FromDb = pt.get<std::string>("passwordSalt");

    std::string passwordFromUserInput = m_LoginPasswordLineEdit->text().toUTF8();
    std::string passwordHashFromUserInput = sha1(passwordFromUserInput + passwordSaltInBase64FromDb);

    //hmm i COULD use base64decode but it doesn't matter which of the two converts to the other's format
    std::string passwordHashBase64FromUserInput = base64Encode(passwordHashFromUserInput);

    if(passwordHashBase64FromUserInput == passwordHasBase64hFromDb)
    {
        //login
        changeSessionId();
        m_LoggedIn = true;
        m_Username = m_LoginUsernameLineEdit->text();

        //TODOreq: logout -> login as different user will show old username in logout widget guh (same probablem with register successful widget)...
        if(!m_LogoutWidget)
        {
            m_LogoutWidget = new WContainerWidget(m_LoginLogoutStackWidget);
            new WText("Hello, ", m_LogoutWidget);
            new WText(m_Username, m_LogoutWidget);
            WPushButton *logoutButton = new WPushButton("Log Out", m_LogoutWidget);
            logoutButton->clicked().connect(this, &AnonymousBitcoinComputingWtGUI::handleLogoutButtonClicked);
        }
        m_LoginLogoutStackWidget->setCurrentWidget(m_LogoutWidget);
    }
    else
    {
        //login fail. TODOreq: also do this login fail if the requested key isn't found (user doesn't exist)
    }

    //TODOreq: do this on key not found fail (and all other relevant errors) also (if only there was a generator that let you see if there was any kind of error and then let you drill down)
    m_LoginUsernameLineEdit->setText("");
    m_LoginPasswordLineEdit->setText("");
}
void AnonymousBitcoinComputingWtGUI::handleLogoutButtonClicked()
{
    if(!m_LoggedIn)
        return;

    m_LoggedIn = false;
    m_LoginLogoutStackWidget->setCurrentWidget(m_LoginWidget);
}
void AnonymousBitcoinComputingWtGUI::newAndOpenAllWtMessageQueues()
{
    //m_AddWtMessageQueue[0] = new message_queue(open_only, "BlahAdd0");
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES, WT_NEW_AND_OPEN_ADD_MESSAGE_QUEUE_MACRO, ~)
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES, WT_NEW_AND_OPEN_GET_MESSAGE_QUEUE_MACRO, ~)
}
void AnonymousBitcoinComputingWtGUI::deleteAllWtMessageQueues()
{
    //the couchbase half does the message_queue::remove -- we just delete (implicitly 'closing') our pointers

    //delete m_AddWtMessageQueue[0]; m_AddWtMessageQueue[0] = NULL;
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES, WT_DELETE_MESSAGE_QUEUE_MACRO, Add)
    BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES, WT_DELETE_MESSAGE_QUEUE_MACRO, Get)
}

message_queue *AnonymousBitcoinComputingWtGUI::m_AddWtMessageQueues[NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES];
message_queue *AnonymousBitcoinComputingWtGUI::m_GetWtMessageQueues[NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES];

event *AnonymousBitcoinComputingWtGUI::m_AddEventCallbacksForWt[NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES];
event *AnonymousBitcoinComputingWtGUI::m_GetEventCallbacksForWt[NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES];

boost::mutex AnonymousBitcoinComputingWtGUI::m_AddMutexArray[NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES];
boost::mutex AnonymousBitcoinComputingWtGUI::m_GetMutexArray[NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES];


#if 0
//message_queue *AnonymousBitcoinComputingWtGUI::m_AddWtMessageQueues[0] = NULL;
BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES, WT_MESSAGE_QUEUE_DEFINITIONS_MACRO, Add)
BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES, WT_MESSAGE_QUEUE_DEFINITIONS_MACRO, Get)

//event *AnonymousBitcoinComputingWtGUI::m_AddEventCallbackForWt[0] = NULL;
BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES, WT_LIBEVENTS_MEMBER_DEFINITIONS_MACRO, Add)
BOOST_PP_REPEAT(NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES, WT_LIBEVENTS_MEMBER_DEFINITIONS_MACRO, Get)
#endif
