#include "anonymousbitcoincomputingwtgui.h"

#include "../addcouchbasedocumentbykeyrequest.h"
#include "../getcouchbasedocumentbykeyrequest.h"

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

    WDateTime currentDateTime = WDateTime::currentDateTime();
    double currentPrice = (m*((double)currentDateTime.toTime_t()))+b;

    //TODOreq: check current not expired (use min)

    new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
    new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
    std::ostringstream currentPriceStream; //TODOreq: all other number->string conversions should use this method
    //currentPriceStream
    //why the fuck does setprecision(6) give me 8 decimal places and setprecision(8) gives me 10!?!?!? lol C++
    currentPriceStream << setprecision(6) << currentPrice; //TODOreq:rounding errors maybe? I need to make a decision on that, and I need to make sure that what I tell them it was purchased at is the same thing we have in our db
    std::string currentPriceString = currentPriceStream.str();
    new WText("Internal Price (should match above): " + currentPriceString, m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);

    std::ostringstream blah;
    blah << m_AllSlotFillersComboBox->currentIndex();
    std::string slotIndexString = blah.str();
    new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
    new WText("Index: " + slotIndexString, m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);


    //TODOreq: sanitize slot index (user could have forged), verify it in fact exists (was thinking i should try to pull the doc, BUT we already have the "allSlotFillers" doc and can just verify that it's in that instead (so we would be depending on earlier sanity checks when setting up the slotFiller instead)


    //TODOreq: make a query for the user's "account", to both cas-lock it and to see that the balance is high enough. hmm i don't need the cas value presented to me in Wt land, but i do need it to be held on to for continuing with the cas swap shit after i make sure balance is high enough (i suppose i can just pass the currentPrice to the backend and have him verify balance > currentPrice (but up till now, my backend has been app agnostic and therefore portable. i suppose i need an app-specific backend? perhaps isA agnostic db type? so much flexibility it hurts. KISS). really though, cas-swap(lock) is NOT abc-specific, so implementing that in my backend is good. maybe a generic "getCouchbaseDocumentByKeyAndHangOntoItBecauseIMightCasSwapIt" (TODOreq: account for when i DON'T cas-swap it (in this example, if the balance is too low, or if it's already 'locked')). Hmm the CAS value is just a uint64_t, so maybe it won't be so troublesome to bring it to wt side and then send it back to couchbase [in a new 'store' request])



    //TODOreq: failed to lock your account for the purchase (???), failed to buy/fill the slot (insufficient funds, or someone beat us to it (unlock in both cases))
}
void AnonymousBitcoinComputingWtGUI::getCouchbaseDocumentByKeyBegin(const std::string &keyToCouchbaseDocument)
{
    deferRendering();
    GetCouchbaseDocumentByKeyRequest couchbaseRequest(sessionId(), this, keyToCouchbaseDocument);
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
void AnonymousBitcoinComputingWtGUI::addCouchbaseDocumentByKeyBegin(const string &keyToCouchbaseDocument, const string &couchbaseDocument)
{
    deferRendering();
    AddCouchbaseDocumentByKeyRequest couchbaseRequest(sessionId(), this, keyToCouchbaseDocument, couchbaseDocument);
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
    time_t timeNow = time(0);
    struct tm timeStruct;
    char timeBuf[80];
    timeStruct = *localtime(&timeNow);
    strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d.%X", &timeStruct);
    std::string salt = sha1(username + uniqueId() + "saltplx739384sdfjghej9593859dffoiueoru584758958394fowuer732487587292" + timeBuf);
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

    addCouchbaseDocumentByKeyBegin("user" + username, jsonString);
    m_WhatTheAddWasFor = REGISTERATTEMPTADD;
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
        //changeSessionId();
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
