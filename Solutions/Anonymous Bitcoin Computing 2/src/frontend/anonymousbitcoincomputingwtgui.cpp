#include "anonymousbitcoincomputingwtgui.h"


#include "validatorsandinputfilters/lettersnumbersonlyregexpvalidatorandinputfilter.h"
#include "registersuccessfulwidget.h"
#include "accounttabs/addfundsaccounttabbody.h"
#include "accounttabs/newadslotfilleraccounttabbody.h"
#include "accounttabs/viewallexistingadslotfillersaccounttabbody.h"

#include "abc2couchbaseandjsonkeydefines.h"


//internal paths

#define ABC_INTERNAL_PATH_REGISTER "/register"
#define ABC_ANCHOR_TEXTS_REGISTER "Register"

#define ABC_INTERNAL_PATH_ACCOUNT "/account"
#define ABC_ANCHOR_TEXTS_ACCOUNT "Account"

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

#define ABC_MAXIMUM_USERNAME_AND_PASSWORD_LENGTH 64
#define ABC_MAXIMUM_USERNAME_AND_PASSWORD_LENGTH_STRING "64"

//TODOreq: FOREVER: any feature that modifies user-account doc must do so with a CAS-swap, _AND_ the 'get' for that CAS-swap must ALWAYS check that "slotToAttemptToFillAkaPurchase" isn't set on the user-account. If "slotToAttemptToFillAkaPurchase" is set, that feature must not work until "slotToAttemptToFillAkaPurchase" is gone. It should error out saying "don't use multiple tabs" or "please try again in a few seconds".
//^just worth mentioning, but not worth fixing yet: when the buy slot aka slot fill fails just after using account locking (but before slot fill) and they log in and basically can't do shit with their account, it effectively makes the bitcoin stuff off limits too since it relies on the user account not being locked. As of right now they have to wait until the slot is purchased, but once I let them proceed/retry with the buy, then they can get themselves out of it (if they still want to do the purchase)
//TODOreq: "Forgot Your Password?" --> "Tough shit, I hope you learned your lesson"
//TODOreq: no-js get-from-subscription-cache-but-dont-subscribe
//TODOreq: find out when a signal connected to a dangerous slot can NOT be invoked by a user sifting through the html/js: signal disconnect should do it, but what about setVisible(false) and setEnabled(false), or changing a stackedwidget's current item (slot connected to button on stack item underneath). The easiest way to find this out is to ask; fuck trying to dig through Wt (and definitely fuck trying to dig through the html/js). Another related question: is there a race condition between resumeRendering at the beginning of a function and accessing a [editable-via-gui] member variable later in that same method (hacky solution to that is to always resumeRendering at the end of the method, and a hacky way to do that with lots of scope exit points in the function is using the stack struct destructor hack). I'm guessing the resumeRendering thing isn't a problem because the input events aren't (read:PROBABLY-AREN't(idk)) processed until the method ends and wt gets control again. hell, i don't even know for sure that deferRendering is even giving me the protection i'm assuming it is. More: does connecting a button's clicked signal to disable and/or calling deferRendering in slot connected to it solve the 'two clicks really fast' race condition, OR do i need to use a boolean flag checked/set at the very beginning of the slot (and unset at the end of the operation [i don't want to happen twice concurrently]). Does WComboBox sanitize it's currentIndex? Does LineEdit server-side-enforce maxLength?
//TODOreq: litter if(!m_LoggedIn) lots of places. It's a cheap but effective safeguard
//TODOreq: to double or to int64? that is the question. boost::lexical_cast threw an exception when i tried to convert it to u_int64_t, maybe int64_t will have better luck. there's a "proper money handling" page that tells you to use int64.... but i'm trying to figure out what the fuck the point of that even is if i have to go "through" double anyways. maybe i should just string replace the decimal place for an empty string and then viola it is an int64 (still needs lexical casting for maths :-/). re: "through double" = double is usually more than enough on most platforms, but on some it isn't <- rationale for using int64. but if you're going string -> double -> int64 anyways, won't you have already lost precision? or maybe it's only when the math is being performed. I still am leaning towards int64 (if i can fucking convert my strings to it) because it does appear to be the right way to do rounding (deal with single Satoshis = no rounding needed)
//venting/OT: fucking bitcoin testnet-box-3 has tons of bitcoins, but they're all immature/orphaned so unspendable until i mine 120 fucking blocks. i'm averaging 1 block an hour -_-. box 2 didn't have this problem when i was fucking around with it years ago. i have the code written to test bitcoin interactions, but no test bitcoins available hahaha (i guess i can write the bulk bitcoins generator -> hugeBitcoinList/bitcoinKeySetN setter upper thingo first... zzz...

//TODOoptimization: lots of stuff needs to be moved into it's own object instead of just being a member in this class. it will reduce the memory-per-connection by...  maybe-a-significant... amount. for example the HackedInD3faultCampaign0 shit should be on it's own widget, but ON TOP OF THAT each "step" in the HackedInD3faultCampaign0 thing (buy step 1, buy step 2) can/should be it's own object (member of of HackedInD3faultCampaign0 object). We just have too many unused and unneeded-most-of-the-time member variables in this monster class... but KISS so ima continue for now, despite cringing at how ugly/hacky it's becoming :)
//^for string values that aren't needed most of the time but still need to be members because accessed in various async methods etc, i can put them in a ptree (that is itself a member). this way they only take up any memory when they are needed (this monster class has a lot of corner cases and even those corner cases declare members). ptree isn't just for json (i lol'd when i was thinking to myself "hmm i need some sort of... property... tree..."). a hash with string key/value would also work and might be faster. i've never done memory management in such a way but hey i think it'll work...

//TODOreq: can't remember if i wrote this anywhere or only thought of it, but i need to add underscores between certain things in my couchbase docs. for example a user named JimboKnives0 would have a conflict with a user named JimboKnives. adSpaceSlotFillersJimboKnives0 would now point to a specific ad slot filler, _AND_ the "profile view" (last 10 slot fillers set up) for JimboKnives0 -- hence, conflict. Could maybe solve this another way by perhaps always having the username be the very last part of the key???

//TODOoptimization: walking the internal path via "next sub path" (including sanitization at each step) is a good way to organize the website and additionally fill in json key pieces. example: /ads/buy-ad-space/d3fault/0 could be organized in Wt as: class Ads { class BuyAdSpace { m_SellerUsername = d3fault; m_SellerCampaign = 0; } }.... and organized in json as ads_buy_d3fault_0.  sanitization errors would be simple 404s, and it goes without saying that neither can/should include underscores or slashes
//^sanitize underscores(well,maybe.. if used as key 'split' identifier (isn't currently)), slashes(ditto if done for internal path 'walking'), and spaces (couchbase keys cannot contain spaces) from all user input AS A MINIMUM. really i'm probably going to go overkill and just allow a-z and 0-9 in a whitelist :-D. fuggit

AnonymousBitcoinComputingWtGUI::AnonymousBitcoinComputingWtGUI(const WEnvironment &myEnv)
    : WApplication(myEnv),
      m_BodyHLayout(new WHBoxLayout()),
      m_LinksVLayout(new WVBoxLayout()),
      m_MainVLayout(new WVBoxLayout(root())),
      m_LoginLogoutStackWidget(new WStackedWidget()),
      m_LoginWidget(new WContainerWidget(m_LoginLogoutStackWidget)),
      m_LoginUsernameLineEdit(0),
      m_LoginPasswordLineEdit(0),
      m_LogoutWidget(0),
      m_LinkToAccount(0),
      m_MainStack(new WStackedWidget()),
      m_404NotFoundWidget(0),
      m_HomeWidget(0),
      m_AdvertisingWidget(0),
      m_AdvertisingBuyAdSpaceWidget(0),
      m_AccountTabWidget(0),
      m_NewAdSlotFillerAccountTab(0),
      m_AuthenticationRequiredWidget(0),
      m_RegisterWidget(0),
      m_RegisterSuccessfulWidget(0),
      m_BuyInProgress(false),
      m_AdvertisingBuyAdSpaceD3faultWidget(0),
      m_AdvertisingBuyAdSpaceD3faultCampaign0Widget(0),
      m_FirstPopulate(false),
      m_BuySlotFillerStep1Button(0),
      m_HackedInD3faultCampaign0_NoPreviousSlotPurchases(true),
      m_BuyStep2placeholder(0),
      m_WhatTheStoreWithoutInputCasWasFor(INITIALINVALIDNULLSTOREWITHOUTINPUTCAS),
      m_WhatTheStoreWithInputCasWasFor(INITIALINVALIDNULLSTOREWITHCAS),
      m_WhatTheStoreWithInputCasSavingOutputCasWasFor(INITIALINVALIDNULLSTOREWITHCASSAVINGCAS),
      m_WhatTheGetWasFor(INITIALINVALIDNULLGET),
      m_WhatTheGetSavingCasWasFor(INITIALINVALIDNULLGETSAVINGCAS),
      m_CurrentlySubscribedTo(INITIALINVALIDNULLNOTSUBSCRIBEDTOANYTHING),
      m_LoggedIn(false)
{
    //constructor body, in case you're confused...
    mt19937 mersenneTwisterRandomNumberGenerator;
    mersenneTwisterRandomNumberGenerator.seed(static_cast<int>(WDateTime::currentDateTime().toTime_t())); //had taus88, but when used with bitcoin key dispersement set selection, i was getting the same number over and over for the entire app run. a TODOoptimization here might be to just do "time_t % num_queues" (or use unique id again), because i'd imagine these RNGs are expensive ish

    //uniform_int_distribution<> l_StoreMessageQueuesRandomIntDistribution(0, ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_Store - 1); m_CurrentStoreMessageQueueIndex = l_StoreMessageQueuesRandomIntDistribution(mersenneTwisterRandomNumberGenerator);
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_MACRO, ABC_WT_PER_QUEUE_SET_UNIFORM_INT_DISTRIBUTION_CONSTRUCTOR_INITIALIZATION_MACRO)

    buildGui(); //everything that is NOT dependent on the internal path

    internalPathChanged().connect(this, &AnonymousBitcoinComputingWtGUI::handleInternalPathChanged);

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
void AnonymousBitcoinComputingWtGUI::finalize()
{
    if(m_CurrentlySubscribedTo != INITIALINVALIDNULLNOTSUBSCRIBEDTOANYTHING)
    {
        //we're subscribed to something, so unsubscribe
        getAndSubscribeCouchbaseDocumentByKeySavingCas(adSpaceCampaignKey("d3fault", "0"), GetCouchbaseDocumentByKeyRequest::GetAndSubscribeUnsubscribeMode); //TODOreq: see handleInternalPath's comment about making key dynamic (ez)
        //no need to reset m_CurrentlySubscribedTo to because 'this' is about to be deleted...
    }
    WApplication::finalize();
}
void AnonymousBitcoinComputingWtGUI::buildGui()
{
    setTitle("Anonymous Bitcoin Computing");
    WDefaultLoadingIndicator *theLoadingIndicator = new WDefaultLoadingIndicator();
    theLoadingIndicator->setMessage("Computing..."); //'Computing' just barely beat out 'Raping yer mum'
    setLoadingIndicator(theLoadingIndicator);
    //WAnimation slideInFromBottom(WAnimation::SlideInFromBottom, WAnimation::EaseOut, 250); //If this took any longer than 2 lines of code (<3 Wt), I wouldn't do it
    //m_MainStack->setTransitionAnimation(slideInFromBottom, true);

    //Login Widget
    m_LoginWidget->setContentAlignment(Wt::AlignTop | Wt::AlignRight);
    new WText("Username:", m_LoginWidget);
    m_LoginUsernameLineEdit = new WLineEdit(m_LoginWidget);
    m_LoginUsernameLineEdit->enterPressed().connect(this, &AnonymousBitcoinComputingWtGUI::handleLoginButtonClicked);
    new WText("Password:", m_LoginWidget);
    m_LoginPasswordLineEdit = new WLineEdit(m_LoginWidget);
    m_LoginPasswordLineEdit->setEchoMode(WLineEdit::Password);
    m_LoginPasswordLineEdit->enterPressed().connect(this, &AnonymousBitcoinComputingWtGUI::handleLoginButtonClicked);
    m_LettersNumbersOnlyValidatorAndInputFilter = new LettersNumbersOnlyRegExpValidatorAndInputFilter(ABC_MAXIMUM_USERNAME_AND_PASSWORD_LENGTH_STRING, m_LoginWidget); //TODOoptimization: thread safe? "service" class? maybe making this static would lessen per-connection-memory
    m_LoginUsernameLineEdit->setMaxLength(ABC_MAXIMUM_USERNAME_AND_PASSWORD_LENGTH);
    m_LoginPasswordLineEdit->setMaxLength(ABC_MAXIMUM_USERNAME_AND_PASSWORD_LENGTH);
    m_LoginUsernameLineEdit->setValidator(m_LettersNumbersOnlyValidatorAndInputFilter);
    m_LoginPasswordLineEdit->setValidator(m_LettersNumbersOnlyValidatorAndInputFilter);
    WPushButton *loginButton = new WPushButton("Log In", m_LoginWidget);
    loginButton->clicked().connect(this, &AnonymousBitcoinComputingWtGUI::handleLoginButtonClicked);
    new WBreak(m_LoginWidget);
    m_LoginStatusMessagesPlaceholder = new WText(m_LoginWidget);
    new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_REGISTER), ABC_ANCHOR_TEXTS_REGISTER, m_LoginWidget);
    m_LoginLogoutStackWidget->setCurrentWidget(m_LoginWidget); //might not be necessary, since it's the only one added at this point (comment is not worth...)

    WHBoxLayout *titleHeaderHlayout = new WHBoxLayout();
    titleHeaderHlayout->addWidget(new WText("<h2>Anonymous Bitcoin Computing</h2>"), 0, Wt::AlignTop | Wt::AlignCenter);
    m_MainVLayout->addLayout(titleHeaderHlayout, 0, Wt::AlignTop | Wt::AlignCenter);

    WHBoxLayout *loginLogoutHeaderHLayout = new WHBoxLayout();
    loginLogoutHeaderHLayout->addWidget(m_LoginLogoutStackWidget, 0, Wt::AlignTop | Wt::AlignRight);
    m_MainVLayout->addLayout(loginLogoutHeaderHLayout, 0, Wt::AlignTop | Wt::AlignRight);

    //LINKS -- TODOoptional: disable clickability of current location
    m_LinksVLayout->addWidget(new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_HOME), ABC_ANCHOR_TEXTS_PATH_HOME), 0, Wt::AlignTop | Wt::AlignLeft);
    m_LinksVLayout->addWidget(new WBreak(), 0, Wt::AlignTop | Wt::AlignLeft);
    m_LinksVLayout->addWidget(new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS), ABC_ANCHOR_TEXTS_PATH_ADS), 0, Wt::AlignTop | Wt::AlignLeft);
    m_LinksVLayout->addWidget(new WBreak(), 0, Wt::AlignTop | Wt::AlignLeft);
    m_LinksVLayout->addWidget(new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE), "-" ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE), 0, Wt::AlignTop | Wt::AlignLeft);
    m_LinksVLayout->addWidget(new WBreak(), 0, Wt::AlignTop | Wt::AlignLeft);

    m_BodyHLayout->addLayout(m_LinksVLayout, 0, Wt::AlignTop | Wt::AlignLeft);
    m_MainStack->setOverflow(WContainerWidget::OverflowAuto);
    m_BodyHLayout->addWidget(m_MainStack, 1, Wt::AlignTop | Wt::AlignLeft);

    m_MainVLayout->addLayout(m_BodyHLayout, 1, Wt::AlignTop | Wt::AlignLeft);
}
void AnonymousBitcoinComputingWtGUI::showHomeWidget()
{
    if(!m_HomeWidget)
    {
        m_HomeWidget = new WContainerWidget(m_MainStack);
        new WText("Mostly empty site for now, you probably want this: ", m_HomeWidget);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE_D3FAULT_CAMPAIGN_0), ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE_D3FAULT_CAMPAIGN_0, m_HomeWidget);
    }
    m_MainStack->setCurrentWidget(m_HomeWidget);
}
void AnonymousBitcoinComputingWtGUI::showAdvertisingWidget()
{
    if(!m_AdvertisingWidget)
    {
        m_AdvertisingWidget = new WContainerWidget(m_MainStack);
        new WText(ABC_ANCHOR_TEXTS_PATH_ADS " - Sub-categories:", m_AdvertisingWidget);
        new WBreak(m_AdvertisingWidget);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE), "-" ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE, m_AdvertisingWidget);
    }
    m_MainStack->setCurrentWidget(m_AdvertisingWidget);
}
void AnonymousBitcoinComputingWtGUI::showAdvertisingBuyAdSpaceWidget()
{
    if(!m_AdvertisingBuyAdSpaceWidget)
    {
        m_AdvertisingBuyAdSpaceWidget = new WContainerWidget(m_MainStack);
        new WText("Users with ad space for sale:", m_AdvertisingBuyAdSpaceWidget);
        new WBreak(m_AdvertisingBuyAdSpaceWidget);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE_D3FAULT), ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE_D3FAULT, m_AdvertisingBuyAdSpaceWidget);
    }
    m_MainStack->setCurrentWidget(m_AdvertisingBuyAdSpaceWidget);
}
void AnonymousBitcoinComputingWtGUI::showAccountWidget()
{
    if(!m_LoggedIn)
    {
        if(m_AccountTabWidget)
        {
            delete m_AccountTabWidget;
            m_AccountTabWidget = 0;
        }
        if(!m_AuthenticationRequiredWidget)
        {
            m_AuthenticationRequiredWidget = new WContainerWidget(m_MainStack);
            new WText("You need to be logged in to view this page", m_AuthenticationRequiredWidget);
        }
        m_MainStack->setCurrentWidget(m_AuthenticationRequiredWidget);
        return;
    }

    //logged in
    if(!m_AccountTabWidget)
    {
        m_AccountTabWidget = new ActualLazyLoadedTabWidget(m_MainStack);
        //I'm pretty sure LazyLoading doesn't quite mean what I think it does/should. I think it means that the html etc isn't sent until request... whereas it real lazy loading would leave the object pointer set to zero until the tab is switched. The docs/api don't appear to work that way, but I could be wrong. Also feel like I've fumbled around with this before... but adding my own lazy loading by listening to currentTabChanged signal won't be hard...
        m_AccountTabWidget->myAddTab(m_NewAdSlotFillerAccountTab = new NewAdSlotFillerAccountTabBody(this), "New Advertisement");
        m_AccountTabWidget->myAddTab(m_ViewAllExistingAdSlotFillersAccountTab = new ViewAllExistingAdSlotFillersAccountTabBody(this), "Existing Advertisements");
        m_AccountTabWidget->myAddTab(m_AddFundsAccountTab = new AddFundsAccountTabBody(this), "Add Funds"); //was going to have this one be first tab, but i don't want a db hit unless they request it


    }
    m_MainStack->setCurrentWidget(m_AccountTabWidget);
}
void AnonymousBitcoinComputingWtGUI::showRegisterWidget()
{
    if(!m_RegisterWidget)
    {
        m_RegisterWidget = new WContainerWidget(m_MainStack);
        WGridLayout *registerGridLayout = new WGridLayout(m_RegisterWidget);
        int rowIndex = -1;

        registerGridLayout->addWidget(new WText("Username/Password: " LETTERS_NUMBERS_ONLY_REGEXPVALIDATOR_AND_INPUT_FILTER_ACCEPTABLE_RANGE_STRING), ++rowIndex, 0, 1, 2, Wt::AlignTop | Wt::AlignLeft);

        registerGridLayout->addWidget(new WText("Username:"), ++rowIndex, 0, Wt::AlignTop | Wt::AlignLeft);
        m_RegisterUsernameLineEdit = new WLineEdit();
        registerGridLayout->addWidget(m_RegisterUsernameLineEdit, rowIndex, 1, Wt::AlignTop | Wt::AlignLeft);
        m_RegisterUsernameLineEdit->enterPressed().connect(this, &AnonymousBitcoinComputingWtGUI::handleRegisterButtonClicked); //was tempted to not put this here because if they press enter in username then they probably aren't done, BUT that 'implicit form submission' bullshit would submit it anyways. might as well make sure it's pointing at the right form...
        m_RegisterUsernameLineEdit->setMaxLength(ABC_MAXIMUM_USERNAME_AND_PASSWORD_LENGTH);
        m_RegisterUsernameLineEdit->setValidator(m_LettersNumbersOnlyValidatorAndInputFilter);

        registerGridLayout->addWidget(new WText("Password:"), ++rowIndex, 0, Wt::AlignTop | Wt::AlignLeft);
        m_RegisterPasswordLineEdit = new WLineEdit();
        registerGridLayout->addWidget(m_RegisterPasswordLineEdit, rowIndex, 1, Wt::AlignTop | Wt::AlignLeft);
        m_RegisterPasswordLineEdit->setEchoMode(WLineEdit::Password);
        m_RegisterPasswordLineEdit->enterPressed().connect(this, &AnonymousBitcoinComputingWtGUI::handleRegisterButtonClicked);
        m_RegisterPasswordLineEdit->setMaxLength(ABC_MAXIMUM_USERNAME_AND_PASSWORD_LENGTH);
        m_RegisterPasswordLineEdit->setValidator(m_LettersNumbersOnlyValidatorAndInputFilter);

        registerGridLayout->addWidget(new WText("WARNING: DO NOT LOSE/FORGET YOUR PASSWORD! THERE IS NO PASSWORD RESET FEATURE!!!"), ++rowIndex, 0, 1, 2, Wt::AlignTop | Wt::AlignLeft);

        WPushButton *registerButton = new WPushButton("Register");
        registerGridLayout->addWidget(registerButton, ++rowIndex, 1, Wt::AlignTop | Wt::AlignLeft);
        registerButton->clicked().connect(this, &AnonymousBitcoinComputingWtGUI::handleRegisterButtonClicked);

        m_RegisterStatusMessagesPlaceholder = new WText();
        registerGridLayout->addWidget(m_RegisterStatusMessagesPlaceholder, ++rowIndex, 0, 1, 2, Wt::AlignTop | Wt::AlignLeft);

        registerGridLayout->addWidget(new WText("Optional:"), ++rowIndex, 0, Wt::AlignTop | Wt::AlignLeft);
        registerGridLayout->addWidget(new WText("Sexual Preference:"), ++rowIndex, 0, Wt::AlignTop | Wt::AlignLeft);
        registerGridLayout->addWidget(new WLineEdit(), rowIndex, 1, Wt::AlignTop | Wt::AlignLeft);
        registerGridLayout->addWidget(new WText("Social Security Number:"), ++rowIndex, 0, Wt::AlignTop | Wt::AlignLeft);
        registerGridLayout->addWidget(new WLineEdit(), rowIndex, 1, Wt::AlignTop | Wt::AlignLeft);
        registerGridLayout->addWidget(new WText("Religion:"), ++rowIndex, 0, Wt::AlignTop | Wt::AlignLeft);
        registerGridLayout->addWidget(new WLineEdit(), rowIndex, 1, Wt::AlignTop | Wt::AlignLeft);
        registerGridLayout->addWidget(new WText("Political Beliefs:"), ++rowIndex, 0, Wt::AlignTop | Wt::AlignLeft);
        registerGridLayout->addWidget(new WLineEdit(), rowIndex, 1, Wt::AlignTop | Wt::AlignLeft);
        registerGridLayout->addWidget(new WText("Time of day your wife is home alone:"), ++rowIndex, 0, Wt::AlignTop | Wt::AlignLeft);
        registerGridLayout->addWidget(new WLineEdit(), rowIndex, 1, Wt::AlignTop | Wt::AlignLeft);
        registerGridLayout->addWidget(new WText("Where you keep your gun:"), ++rowIndex, 0, Wt::AlignTop | Wt::AlignLeft);
        registerGridLayout->addWidget(new WLineEdit(), rowIndex, 1, Wt::AlignTop | Wt::AlignLeft);
        registerGridLayout->addWidget(new WText("Your wife's cycle:"), ++rowIndex, 0, Wt::AlignTop | Wt::AlignLeft);
        registerGridLayout->addWidget(new WLineEdit(), rowIndex, 1, Wt::AlignTop | Wt::AlignLeft);
        registerGridLayout->addWidget(new WText("Your mum's cycle:"), ++rowIndex, 0, Wt::AlignTop | Wt::AlignLeft);
        registerGridLayout->addWidget(new WLineEdit(), rowIndex, 1, Wt::AlignTop | Wt::AlignLeft);
        registerGridLayout->addWidget(new WText("Credit Card #:"), ++rowIndex, 0, Wt::AlignTop | Wt::AlignLeft);
        registerGridLayout->addWidget(new WLineEdit(), rowIndex, 1, Wt::AlignTop | Wt::AlignLeft);
        registerGridLayout->addWidget(new WText("Credit Card Pin #:"), ++rowIndex, 0, Wt::AlignTop | Wt::AlignLeft);
        registerGridLayout->addWidget(new WLineEdit(), rowIndex, 1, Wt::AlignTop | Wt::AlignLeft);

        //;-) and then secretly ;-) on the deployed/binary version ;-) i actually save these values ;-) and then sell them to hollywood ;-) to make movies off of them ;-) and then use the funds from that to take over the world ;-) and solve us of our corporate cancers (of which hollywood is an item) ;-) muaahahahahhahaha

        registerGridLayout->addWidget(new WText("WARNING: DO NOT LOSE/FORGET YOUR PASSWORD! THERE IS NO PASSWORD RESET FEATURE!!!"), ++rowIndex, 0, 1, 2, Wt::AlignTop | Wt::AlignLeft);

        WPushButton *registerButton2 = new WPushButton("Register");
        registerGridLayout->addWidget(registerButton2, ++rowIndex, 1, Wt::AlignTop | Wt::AlignLeft);
        registerButton2->clicked().connect(this, &AnonymousBitcoinComputingWtGUI::handleRegisterButtonClicked);
    }
    m_MainStack->setCurrentWidget(m_RegisterWidget);
}
void AnonymousBitcoinComputingWtGUI::registerAttemptFinished(bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        m_RegisterStatusMessagesPlaceholder->setText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE);
        return;
    }
    if(!lcbOpSuccess)
    {
        m_RegisterStatusMessagesPlaceholder->setText("That username is already taken, please try another");
        return;
    }

    //if we get here, the registration doc add was successful

    if(!m_RegisterSuccessfulWidget)
    {
        m_RegisterSuccessfulWidget = new RegisterSuccessfulWidget(m_RegisterUsernameLineEdit->text().toUTF8(), m_MainStack);
    }
    else
    {
        m_RegisterSuccessfulWidget->setUsername(m_RegisterUsernameLineEdit->text().toUTF8());
    }
    m_MainStack->setCurrentWidget(m_RegisterSuccessfulWidget);

    m_RegisterStatusMessagesPlaceholder->setText("");
    m_RegisterUsernameLineEdit->setText("");
    m_RegisterPasswordLineEdit->setText("");
}
void AnonymousBitcoinComputingWtGUI::showAdvertisingBuyAdSpaceD3faultWidget()
{
    if(!m_AdvertisingBuyAdSpaceD3faultWidget)
    {
        m_AdvertisingBuyAdSpaceD3faultWidget = new WContainerWidget(m_MainStack);
        new WText("d3fault's ad space campaigns:", m_AdvertisingBuyAdSpaceD3faultWidget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultWidget);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE_D3FAULT_CAMPAIGN_0), ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE_D3FAULT_CAMPAIGN_0, m_AdvertisingBuyAdSpaceD3faultWidget);
    }
    m_MainStack->setCurrentWidget(m_AdvertisingBuyAdSpaceD3faultWidget);
}
void AnonymousBitcoinComputingWtGUI::beginShowingAdvertisingBuyAdSpaceD3faultCampaign0Widget()
{
    //TO DOnereqoptimization(but comments mentions other designs): this is going to be my most expensive document (the home link might be too, but it doesn't hit the db). I need SOME sort of caching solution [in order to make this horizontally scalable (so in other words, it isn't an absolute must pre-launch task], even if it's hacked-in/hardcoded who cares. getAndSubscribe comes to mind (but sounds complicated). a fucking mutex locked when new'ing the WContainerWidget below would be easy and would scale horizontally. Hell it MIGHT even be faster than a db hit (and there's always a 'randomly selected mutex in array of mutexes' hack xD)

    if(!m_AdvertisingBuyAdSpaceD3faultCampaign0Widget) //TODOreq: this object, once created, should never be deleted until the WApplication is deleted. The reason is that get and subscribe updates might be sent to it (even if the user has navigated away, there is a race condition where they did not 'unsubscribe' yet so they'd still get the update (Wt handles this just fine. you can setText on something not being shown without crashing (but if I were to delete it, THEN we'd be fucked)))
    {
        m_AdvertisingBuyAdSpaceD3faultCampaign0Widget = new WContainerWidget(m_MainStack);
        m_AdvertisingBuyAdSpaceD3faultCampaign0Widget->setOverflow(WContainerWidget::OverflowAuto);
        new WText(ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE_D3FAULT_CAMPAIGN_0, m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);

        //still dunno if i should block or do it async? instincts tell me async, but "SUB MILLISECOND LATENCY" tells me async might actually be wasteful/slower??? The obvious answer is "benchmark it xD" (FUCK FUCK FUCK FUCK FUCK THAT, async it is (because even if slower, it still allows us to scale bettarer))
        //^To clarify, I have 3 options: wait on a wait condition right here that is notified by couchbase thread and we serve up result viola, deferRendering/resumeRendering, or enableUpdates/TriggerUpdates. The last one requires ajax. Both the last 2 are async (despite defer "blocking" (read:disabling) the GUI)

        new WText("Buy ad space for ", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        m_CampaignLengthHoursLabel = new WText(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WText(" hours, starting: ", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        m_CampaignSlotCurrentlyForSaleStartDateTimeLabel = new WText(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);

        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WText("Price: BTC ", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        m_CurrentPriceLabel = new WText(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        m_CurrentPriceDomPath = m_CurrentPriceLabel->jsRef();

        if(!environment().ajax())
        {
            //TODOreq: get, don't subscribe (but still use subsciption cache!). when you impl that, change it for the no-js successful slot buy as well, since it is currently hackily a copy/paste job from this code block :)
            doHackyOneTimeBuyEventUpdateThingoForNoJavascriptUser();
            //getCouchbaseDocumentByKeySavingCasBegin(adSpaceCampaignKey("d3fault", "0"));
            //m_WhatTheGetSavingCasWasFor = HACKEDIND3FAULTCAMPAIGN0GET; //TODOreq: why the fuck do i save the cas? surely i should re-fetch the doc- OH BUT IT NEVER CHANGES UNLESS THERE'S A BUY EVENT!!!
        }
        else
        {
            enableUpdates(true); //TODOreq: putting here doesn't future proof us for other get-and-subscribe, but no need to put it in constructor because most other pages don't use it
            //TODOoptional: maybe enableUpdates(false) if/when they navigate away, which would mean i set back to true in the 're-subscribe' below (for when this widget is already created)

            m_FirstPopulate = true; //so we know to call resumeRendering on first populate/update, but not populates/updates thereafter
            deferRendering();

            getAndSubscribeCouchbaseDocumentByKeySavingCas(adSpaceCampaignKey("d3fault", "0"), GetCouchbaseDocumentByKeyRequest::GetAndSubscribeMode);
            m_CurrentlySubscribedTo = HACKEDIND3FAULTCAMPAIGN0GETANDSUBSCRIBESAVINGCAS;

            //TODOoptimization: shorten js variable names (LOL JS). use defines to retain sanity

            //placeholder/initialization javascript, not populated or active yet            
            m_CurrentPriceLabel->doJavaScript
            (
                m_CurrentPriceDomPath + ".z0bj = new Object();" +
                m_CurrentPriceDomPath + ".z0bj.minPrice = 0;" +
                m_CurrentPriceDomPath + ".z0bj.serverTimestamp = 0;" +
                m_CurrentPriceDomPath + ".z0bj.clientServerTimeOffsetMSecs = 0;" + //fuck timezones or just plain wrong times, now the only inaccuracy will be 'latency'
                m_CurrentPriceDomPath + ".z0bj.lastSlotFilledAkaPurchasedExpireDateTime = 0;" +
                m_CurrentPriceDomPath + ".z0bj.lastSlotFilledAkaPurchasedPurchasePrice = 0;" +
                m_CurrentPriceDomPath + ".z0bj.lastSlotFilledAkaPurchasedPurchaseTimestamp = 0;" +
                m_CurrentPriceDomPath + ".z0bj.lastSlotFilledAkaPurchasedExpireDateTimeMSecs = 0;" +
                m_CurrentPriceDomPath + ".z0bj.m = 0;" +
                m_CurrentPriceDomPath + ".z0bj.b = 0;" +
                m_CurrentPriceDomPath + ".z0bj.tehIntervalz = 0;" +
                m_CurrentPriceDomPath + ".z0bj.tehIntervalIsRunnan = false;" +
                m_CurrentPriceDomPath + ".z0bj.tehIntervalzCallback = function()"
                "{"
                    "var currentDateTimeMSecs = new Date().getTime();"
                    "currentDateTimeMSecs += " + m_CurrentPriceDomPath + ".z0bj.clientServerTimeOffsetMSecs;"
                    "if(currentDateTimeMSecs >= " + m_CurrentPriceDomPath + ".z0bj.lastSlotFilledAkaPurchasedExpireDateTimeMSecs)"
                    "{" +
                        m_CurrentPriceDomPath + ".innerHTML = " + m_CurrentPriceDomPath + ".z0bj.minPrice.toFixed(8);" //TODOreq: rounding?
                        "clearInterval(" + m_CurrentPriceDomPath + ".z0bj.tehIntervalz);"
                        + m_CurrentPriceDomPath + ".z0bj.tehIntervalIsRunnan = false;"
                    "}"
                    "else"
                    "{"
                        "var currentPrice = ((" + m_CurrentPriceDomPath + ".z0bj.m*(currentDateTimeMSecs/1000))+" + m_CurrentPriceDomPath + ".z0bj.b);" + //y = mx+b
                        m_CurrentPriceDomPath + ".innerHTML = currentPrice.toFixed(8);"
                    "}"
                "}"
            );
        }
    }
    else //widget already created, so...
    {
        if(!environment().ajax())
        {
            //get current/fresh values
            doHackyOneTimeBuyEventUpdateThingoForNoJavascriptUser();
        }
        else
        {
            //just re-subscribe
            getAndSubscribeCouchbaseDocumentByKeySavingCas(adSpaceCampaignKey("d3fault", "0"), GetCouchbaseDocumentByKeyRequest::GetAndSubscribeMode);
            m_CurrentlySubscribedTo = HACKEDIND3FAULTCAMPAIGN0GETANDSUBSCRIBESAVINGCAS;
        }
        //TODOreq: the data will still be populated, but the data may be stale (maybe i shouldn't ever show such stale info (could be really really old, BUT if everything is working it will get updated really fast ([probably] not even a couchbase hit, so SUB-SUB-MILLISECONDS!?!?)))
        //^still, should i disable gui or something?
    }
    m_MainStack->setCurrentWidget(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
}
double AnonymousBitcoinComputingWtGUI::calculateCurrentPrice(double currentTime_x, double minPrice_y2, double lastSlotFilledAkaPurchasedPurchasePriceDoubled_y1, double lastSlotFilledAkaPurchasedExpireDateTime_x2, double lastSlotFilledAkaPurchasedPurchaseDateTime_x1)
{
    //m = (y2-y1)/(x2-x1);
    double m = (minPrice_y2-lastSlotFilledAkaPurchasedPurchasePriceDoubled_y1)/(lastSlotFilledAkaPurchasedExpireDateTime_x2-lastSlotFilledAkaPurchasedPurchaseDateTime_x1);
    //b = y-(m*x)
    double b = (minPrice_y2 - (m * lastSlotFilledAkaPurchasedExpireDateTime_x2));
    //y = m(x)+b
    return ((m*currentTime_x)+b);
}
void AnonymousBitcoinComputingWtGUI::finishShowingAdvertisingBuyAdSpaceD3faultCampaign0Widget(const string &advertisingBuyAdSpaceD3faultCampaign0JsonDocument, u_int64_t casForSafelyUpdatingCampaignDocAfterSuccesfulPurchase)
{
    //This is ALSO (but not always (first get)) what I have referred to as a "buy event", so if they have clicked "buy step 1" we need to roll back the GUI so they have to click buy step 1 again (various GUI object organizational changes to support this)
    //Goes without saying that the new 'slot index' that they will try to buy (which is locked in after clicking buy step 1) should be set in this method
    //I'm thinking it might be easiest to do all the "new" ing in the beginShowing() method and then to merely modify/populate those objects/variables via setText in this one
    //TODOoptional: unsubscribing + resubscribing would make the resubscriber a NewSubscriber, and he'd get an update even if it's the same value (this probably won't matter (and isn't even worth coding for))

    //if they're passed step 1 when a buy event is received, roll back
    rollBackToBeforeBuyStep1ifNeeded();

    m_HackedInD3faultCampaign0JsonDocForUpdatingLaterAfterSuccessfulPurchase = advertisingBuyAdSpaceD3faultCampaign0JsonDocument;
    m_HackedInD3faultCampaign0CasForSafelyUpdatingCampaignDocLaterAfterSuccessfulPurchase = casForSafelyUpdatingCampaignDocAfterSuccesfulPurchase;

    //TODOreq: this is the javascript impl of the countdown shit. we obviously need to still verify that the math is correct when a buy attempt happens (we'll be using C++ doubles as well, so will be more precise). we should detect when a value lower than 'current' is attempted, and then laugh at their silly hack attempt. i should make the software laugh at me to test that i have coded it properly (a temporary "submit at price [x]" line edit just for testing), but then leave it (the laughing when verification fails, NOT the line edit for testing) in for fun.
    //TODOreq: if we do the new-ing in 'begin' a populating in 'finish' (here), we need to deferRendering to make things such as 'buy step 1' not clickable until the first 'finish' (which is more appropriately 'update'/populate now)... but we shouldn't do resumeRendering every time, because buy events don't need to do it. I guess we could force it to be invisible until 'finish', but I'm not certain that makes it unclickable (so a hacker could segfault me still). I'm 50/50 on this, I think Wt might make invisible objects 'unclickable' in that sense. setDisabled should do the same, but again unsure if that makes the inner js -> c++ slot mechanism not usable/hackable(hackable-because-segfault)

    ptree pt;
    std::istringstream is(advertisingBuyAdSpaceD3faultCampaign0JsonDocument);
    read_json(is, pt);

    m_HackedInD3faultCampaign0_MinPrice = pt.get<std::string>(JSON_AD_SPACE_CAMPAIGN_MIN_PRICE);
    m_HackedInD3faultCampaign0_SlotLengthHours = pt.get<std::string>(JSON_AD_SPACE_CAMPAIGN_SLOT_LENGTH_HOURS);
    boost::optional<ptree&> lastSlotFilledAkaPurchased = pt.get_child_optional(JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED);
    m_HackedInD3faultCampaign0_NoPreviousSlotPurchases = !lastSlotFilledAkaPurchased.is_initialized();


    //TODOoptional: would be nifty to have over to the right side of the graph, the last 10 purchases (with last at the bottom and momentarily highlighted on a buy event)

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

    if(m_HackedInD3faultCampaign0_NoPreviousSlotPurchases)
    {
        //TODOreq (done i think, but test this): no purchases yet, use static min price (but still be able to transform to javascript countdown on buy event)
        m_CurrentPriceLabel->setText(m_HackedInD3faultCampaign0_MinPrice);
        m_CampaignSlotCurrentlyForSaleStartDateTimeLabel->setText("immediately after purchase");
    }
    else
    {
        m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedSlotIndex = lastSlotFilledAkaPurchased.get().get<std::string>(JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED_INDEX);
        m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedPurchaseTimestamp = lastSlotFilledAkaPurchased.get().get<std::string>(JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED_PURCHASE_TIMESTAMP);
        m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedStartTimestamp = lastSlotFilledAkaPurchased.get().get<std::string>(JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED_START_TIMESTAMP);
        m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedPurchasePrice = lastSlotFilledAkaPurchased.get().get<std::string>(JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED_PURCHASE_PRICE);

        if(environment().ajax())
        {
            //timer either already running, about to start running (first populate), so give these variables real values
            m_CurrentPriceLabel->doJavaScript
            (
                m_CurrentPriceDomPath + ".z0bj.lastSlotFilledAkaPurchasedExpireDateTime = new Date((" + m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedStartTimestamp + "*1000)+((" + m_HackedInD3faultCampaign0_SlotLengthHours + "*3600)*1000));" +
                m_CurrentPriceDomPath + ".z0bj.lastSlotFilledAkaPurchasedPurchasePrice = " + m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedPurchasePrice + ";" +
                m_CurrentPriceDomPath + ".z0bj.lastSlotFilledAkaPurchasedPurchaseTimestamp = " + m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedPurchaseTimestamp + ";" +
                m_CurrentPriceDomPath + ".z0bj.lastSlotFilledAkaPurchasedExpireDateTimeMSecs = " + m_CurrentPriceDomPath + ".z0bj.lastSlotFilledAkaPurchasedExpireDateTime.getTime();" +
                m_CurrentPriceDomPath + ".z0bj.m = ((" + m_HackedInD3faultCampaign0_MinPrice + "-(" + m_CurrentPriceDomPath + ".z0bj.lastSlotFilledAkaPurchasedPurchasePrice*2))/((" + m_CurrentPriceDomPath + ".z0bj.lastSlotFilledAkaPurchasedExpireDateTimeMSecs/1000)-" + m_CurrentPriceDomPath + ".z0bj.lastSlotFilledAkaPurchasedPurchaseTimestamp));" +
                m_CurrentPriceDomPath + ".z0bj.b = (" + m_HackedInD3faultCampaign0_MinPrice + " - (" + m_CurrentPriceDomPath + ".z0bj.m * (" + m_CurrentPriceDomPath + ".z0bj.lastSlotFilledAkaPurchasedExpireDateTimeMSecs/1000)));" +

                //Price at minPrice -> buy-event (Re-enable timer if it's not on and this isn't the first populate (if it's first populate, we enable it later after setting minPrice))
                (m_FirstPopulate ? "" : ("if(" + m_CurrentPriceDomPath + ".z0bj.tehIntervalIsRunnan == false)"
                "{" +
                        ABC_START_JS_INTERVAL_SNIPPET
                "}"))
            );
        }

        //check expired. js checks expired itself, BUT we still follow this code path just to get the starting 'datetime' of purchaseable slot accurately set [for both js and no-js]
        double lastSlotFilledAkaPurchasedExpireDateTime = (boost::lexical_cast<double>(m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedStartTimestamp)+((double)(boost::lexical_cast<double>(m_HackedInD3faultCampaign0_SlotLengthHours)*(3600.0))));
        double currentDateTime = static_cast<double>(WDateTime::currentDateTime().toTime_t());
        if(currentDateTime >= lastSlotFilledAkaPurchasedExpireDateTime)
        {
            //expired, so use min price
            if(!environment().ajax()) //js sets it to min price on it's own (after first timeout)
            {
                m_CurrentPriceLabel->setText(m_HackedInD3faultCampaign0_MinPrice);
            }
            m_CampaignSlotCurrentlyForSaleStartDateTimeLabel->setText("immediately after purchase");
        }
        else
        {
            //not expired, so calculate. despite determining it's not expired here, we are not going to set "m_HackedInD3faultCampaign0_LastSlotPurchasesIsExpired = false;", because it may have expired by the time they hit buy step 2. We have to check when/after they click buy step 2. Could be minutes/hours/days after the page has been rendered (this code)

            m_CampaignSlotCurrentlyForSaleStartDateTimeLabel->setText(WDateTime::fromTime_t(lastSlotFilledAkaPurchasedExpireDateTime).toString());

            if(!environment().ajax()) //since not expired, no-js now calculates current price
            {
                //TODOreq: javascript-less UI should update price after buy step 1 is clicked
                //TODOreq: maybe a 60 second timer to refresh js-less page, or a refresh button

                double currentPrice = calculateCurrentPrice(currentDateTime, boost::lexical_cast<double>(m_HackedInD3faultCampaign0_MinPrice), (boost::lexical_cast<double>(m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedPurchasePrice)*2.0), (boost::lexical_cast<double>(m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedStartTimestamp)+((double)(boost::lexical_cast<double>(m_HackedInD3faultCampaign0_SlotLengthHours)*(3600.0)))), boost::lexical_cast<double>(m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedPurchaseTimestamp));

                m_CurrentPriceLabel->setText(boost::lexical_cast<std::string>(currentPrice)); //TODOreq: 8 decimal places, since it's presented to user
                //TODOoptional: if viewing campaign/countdown with js on and you then disallow js, the price is empty/blank (not even zero). weirdly though, coming directly to the page (no session) with javascript already disabled works fine. I think it has something to do with noscript not re-fetching the source during the "reload"... because on the noscript-reload tab i see "Scripts currently forbidden" (with options to allow them), and on "navigated directly to campaign with js already off" tab I don't see that message (no js was served). I don't even think I CAN fix this problem heh :-P... but it might be a Wt bug... not gracefully degrading when session becomes js-less?

                //TODOreq: "current price is XXX and it will be back at YYYYY at ZZZZZZZ (should nobody buy any further slots). Click here to refresh this information to see if it's still valid, as someone may have purchased a slot since you loaded this page (enable javascript if you want to see it count down automatically)"
            }
        }
    }

    if(environment().ajax()) //derp i hate when my k-maps (which i don't even actually formally make) don't come out nicely and i have to check the same bool twice
    {
        if(m_FirstPopulate)
        {
            m_FirstPopulate = false;
            resumeRendering();
            m_CampaignLengthHoursLabel->setText(m_HackedInD3faultCampaign0_SlotLengthHours);
            m_CurrentPriceLabel->doJavaScript
            (
                m_CurrentPriceDomPath + ".z0bj.minPrice = " + m_HackedInD3faultCampaign0_MinPrice + ";" +
                m_CurrentPriceDomPath + ".z0bj.serverTimestamp = " + boost::lexical_cast<std::string>(static_cast<double>(WDateTime::currentDateTime().toTime_t())) + "*1000;" +
                m_CurrentPriceDomPath + ".z0bj.clientServerTimeOffsetMSecs = (" + m_CurrentPriceDomPath + ".z0bj.serverTimestamp -" + "(new Date().getTime()));" +
                ABC_START_JS_INTERVAL_SNIPPET
                //TODOreq: does re-subscribe re-enable the timer? i would think the timer would still be in whatever state it was in when we left/unsubscribed, SO THAT MEANS: if we unsubscribe -> the timer 'stops' (minprice) _AND_ there is a buy event before we re-subscribe = the timer is still sitting at stopped/min-price and does not get re-enabled (despite us having accurate 'values' for calculating).
                //^it appears that the timer only gets turned on at first populate. a buy event after the timer stops (min-price) probably won't enable it either, which is related to the directly above req
            );
        }
        triggerUpdate();
    }
    else //no-js
    {
        m_CampaignLengthHoursLabel->setText(m_HackedInD3faultCampaign0_SlotLengthHours);
    }

    if(!m_BuySlotFillerStep1Button)
    {
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        m_BuySlotFillerStep1Button = new WPushButton("Buy At This Price (Step 1 of 2)", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        m_BuySlotFillerStep1Button->clicked().connect(this, &AnonymousBitcoinComputingWtGUI::buySlotStep1d3faultCampaign0ButtonClicked);
        m_BuySlotFillerStep1Button->clicked().connect(m_BuySlotFillerStep1Button, &WPushButton::disable);
    }
    //TODOreq: get and subscribe testing so far will not include rolling back of anything after step 1 of 2 clicked, so i gotta do the rest of that process later
}
void AnonymousBitcoinComputingWtGUI::buySlotStep1d3faultCampaign0ButtonClicked()
{
    //TODOreq: anything between step 1 being clicked and the slot being successfully purchased/filled needs to be able to roll back to a pre-step-1-clicked state: buy events, insufficient funds, user-account-lock fails, etc (there are probably more)
    //TODOreq: semi-related to ^. since the user is already logged in (actually that's checked a few lines down from here), we can do ANOTHER ahead-of-time sufficient funds check (DO NOT DEPEND ON IT THOUGH) right when the user clicks buy step 1

    if(!m_LoggedIn)
    {
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget); //TODOoptimization: a DDOS to make server run out of ram: click 'buy step 1' a billion fucking times (easy since it doesn't move on screen). each one is a heap allocate. solution = only one "log in first"
        new WText("Log In First", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        m_BuySlotFillerStep1Button->enable();
        return;
    }

    //TODOreq: we should record the current slot index here and make sure that after buy step 2 is pressed, the slot index when they pressed it matches the one we recorded here. that's common sense, HOWEVER the current slot index might have even changed by the time the below db-get returns (before we create teh buy step 2 button), so it makes sense to check it in two places (just before creating buy step 2 button + in buy step 2's clicked slot). ADDITIONALLY, we should use the slot index recorded here to do the buy attempt, so that even if our checks fail and they somehow get passed those two checks, we don't use 'current slot index' but the slot index recorded here/now. i think that about covers it, aside from of course the visual rollback stuff
    //^^actually buy event rolling back to pre-step-1 means i don't need to check it's valid after the db-get below, WHICH MEANS buy event itself needs to check the recorded value's accuracy. wait no, it always rolls back durr ;-P. still, isn't there a race condition where they hit buy step 1 right around/at/before/after (WHICH?) the buy event that would leave the buy step 2 button visible and [HOPEFULLY] pointing to the wrong slot index?
    //^^^race condition(?): buy event doesn't see step 2 enabled, so doesn't disable it. buy step 1 click event is still traveling to server and when it gets here,.... err then we'd have the value from the most recent buy event, which although accurate-ish isn't what it was when they pressed buy step 1. it would be up to them to look at the gui (BUT WHAT IF MAH buy step 2 enabling beats --- no nvm it won't beat the buy event that CAME BEFORE IT (tcp guarantees this)). TODOreq: it is mandatory that buy step 1 updates the price when in no-js mode, otherwise the user wouldn't see that and well duh this is obvious and has nothing to do with a race condition
    //^^^^^the race condition where buy event happens before teh below db-get finishes DOES matter, because the buy event will not see step 2 created yet (it isn't). it won't end up making the user buy a slot for twice what they want (it will fail), BUT their gui won't roll back for the 'buy event', so we DO need to check just before setting up buy step 2 xD
    //^^^^^^^^we also want to 'roll back' after the buy is successful, but certain messages should remain
    m_SlotIndexImmediatelyAfterBuyStep1wasPressed_aka_PreviousSlotIndexToTheOneTheyWantToBuy = m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedSlotIndex;

    getCouchbaseDocumentByKeyBegin(adSpaceAllSlotFillersKey(m_CurrentlyLoggedInUsername));
    m_WhatTheGetWasFor = HACKEDIND3FAULTCAMPAIGN0BUYSTEP1GET;
    deferRendering();
}
void AnonymousBitcoinComputingWtGUI::buySlotPopulateStep2d3faultCampaign0(const std::string &allSlotFillersJsonDoc, bool lcbOpSuccess, bool dbError)
{
    resumeRendering();
    if(dbError)
    {
        //TODOreq: 500 internal server error

        //temp:
        cerr << "buySlotPopulateStep2d3faultCampaign0 db error" << endl;

        //"roll back" to pre-step 1, but since we haven't set up anything yet we just re-enable step 1 buttan
        m_BuySlotFillerStep1Button->enable();
        return;
    }
    if(!lcbOpSuccess) //allAds doc doesn't exist
    {
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WText("You need to set up some advertisements before you can buy ad space: ", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ACCOUNT), ABC_ANCHOR_TEXTS_ACCOUNT, m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        //"roll back" to pre-step 1, but since we haven't set up anything yet we just re-enable step 1 buttan
        m_BuySlotFillerStep1Button->enable();
        return;
    }

    //if we get here, allAds doc exists (which means it has at least one ad)

    string currentPriceStringForConfirmingOnly = m_HackedInD3faultCampaign0_NoPreviousSlotPurchases ? m_HackedInD3faultCampaign0_MinPrice : (boost::lexical_cast<std::string>(calculateCurrentPrice(static_cast<double>(WDateTime::currentDateTime().toTime_t()), boost::lexical_cast<double>(m_HackedInD3faultCampaign0_MinPrice), (boost::lexical_cast<double>(m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedPurchasePrice)*2.0), (boost::lexical_cast<double>(m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedStartTimestamp)+((double)(boost::lexical_cast<double>(m_HackedInD3faultCampaign0_SlotLengthHours)*(3600.0)))), boost::lexical_cast<double>(m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedPurchaseTimestamp)))); //TODOreq: rounding/etc

    if(!environment().ajax())
    {
        //update the regular one too if no js, because who knows where their eyes will go (and also the buy event race condition needs to still update the price for no-js peeps) :-P
        m_CurrentPriceLabel->setText(currentPriceStringForConfirmingOnly);
    }

    //we may have received a buy event while the get we just got was going on, so check that now
    if(m_SlotIndexImmediatelyAfterBuyStep1wasPressed_aka_PreviousSlotIndexToTheOneTheyWantToBuy != m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedSlotIndex)
    {
        //buy event race condition

        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WText("Someone just bought that slot, so you have to click step 1 again to buy the NEXT slot", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);

        //"roll back" to pre-step 1, but since we haven't set up anything yet we just re-enable step 1 buttan
        m_BuySlotFillerStep1Button->enable();
        return;
    }


    //TODOreq: unsubscribing (navigating away) and resubscribing (coming back) might have missed a buy event, but actually i think re-subscribe should give us an updated value right away (well, within 100ms xD) and when we receive it we'll then roll back to pre-step 1. test this

    //setting this bool to false here instead of at the buy event makes the user have to click buy step 1 again
    m_BuyInProgress = false;

    if(!m_BuyStep2placeholder)
    {
        m_BuyStep2placeholder = new WContainerWidget(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        m_BuyStep2placeholder->setContentAlignment(Wt::AlignTop | Wt::AlignLeft);

        ptree pt;
        std::istringstream is(allSlotFillersJsonDoc);
        read_json(is, pt);

        new WBreak(m_BuyStep2placeholder);
        new WBreak(m_BuyStep2placeholder);
        new WText("Select which advertisement you want to use (you cannot change this later):", m_BuyStep2placeholder);

        new WBreak(m_BuyStep2placeholder);
        m_AllSlotFillersComboBox = new WComboBox(m_BuyStep2placeholder);
        m_AllSlotFillersAdsCount = boost::lexical_cast<int>(pt.get<std::string>(JSON_ALL_SLOT_FILLERS_ADS_COUNT));
        for(int i = 0; i < m_AllSlotFillersAdsCount; ++i)
        {
            //json looks like this: "0" : "<nickname>"
            m_AllSlotFillersComboBox->insertItem(i, base64Decode(pt.get<std::string>(boost::lexical_cast<std::string>(i))));
        }

        new WBreak(m_BuyStep2placeholder);
        new WBreak(m_BuyStep2placeholder);

        //show them the price again, just in case they pressed buy step 1 right around a buy event, hopefully forcing them to notice before continuing (because if they have sufficient funds, owned)

        new WText("You will get the ad space for no more than: BTC " + currentPriceStringForConfirmingOnly + ". (probably less by the time you click step 2)", m_BuyStep2placeholder);
        new WBreak(m_BuyStep2placeholder);
        new WText("ARE YOU SURE? THERE'S NO TURNING BACK AFTER THIS", m_BuyStep2placeholder);
        new WBreak(m_BuyStep2placeholder);
        WPushButton *buySlotFillerStep2Button = new WPushButton("Buy At This Price (Step 2 of 2)", m_BuyStep2placeholder); //TODOoptional: this button could have the js-price-falling thing on it, so they look right at the price when they click it (and buy event 'right before' they click buy step 1 is more likely to be seen/noticed!)
        buySlotFillerStep2Button->clicked().connect(this, &AnonymousBitcoinComputingWtGUI::buySlotStep2d3faultCampaign0ButtonClicked);
        buySlotFillerStep2Button->clicked().connect(buySlotFillerStep2Button, &WPushButton::disable); //we don't rely on this
    }
}
void AnonymousBitcoinComputingWtGUI::ensureSlotDoesntExistThenContinueWithLockingUserAccountIntoAttemptingToBuyItAndThenGoAheadWithBuyEtcIfThatSucceeds(bool lcbOpSuccessAkaWhetherOrNotTheSlotWeWantToBuyExistsBitch, bool dbError)
{
    if(dbError)
    {
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        cerr << "ensureSlotDoesntExistThenContinueWithLockingUserAccountIntoAttemptingToBuyItAndThenGoAheadWithBuyEtcIfThatSucceeds db error" << endl;
        resumeRendering();
        return;
    }
    if(lcbOpSuccessAkaWhetherOrNotTheSlotWeWantToBuyExistsBitch)
    {
        new WText("Sorry, someone beat you to it", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget); //TODOoptional: make these error messages the same via a define
        if(!environment().ajax())
        {
            doHackyOneTimeBuyEventUpdateThingoForNoJavascriptUser();
        }
        resumeRendering();
        return;
    }

    //slot doesn't exist, so it's safe to lock user account attempting to fill it (safe only because we use cas from BEFORE the slot existent check (hard to explain but suck my dick (i barely understand it so yea)))

    ptree pt;
    std::istringstream is(m_UserAccountUnlockedJustBeforeBuyingJson); //we've already parsed this json once to verify it isn't already locked attempting to buy some slot
    read_json(is, pt);

    std::string userBalanceString = pt.get<std::string>(JSON_USER_ACCOUNT_BALANCE);
    double userBalance = boost::lexical_cast<double>(userBalanceString);

    //TO DOnereq: we should probably calculate balance HERE/now instead of in buySlotStep2d3faultCampaign0ButtonClicked (where it isn't even needed). Those extra [SUB ;-P]-milliseconds will get me millions and millions of satoshis over time muahhahaha superman 3

    double currentDateTime = static_cast<double>(WDateTime::currentDateTime().toTime_t());

    if(m_HackedInD3faultCampaign0_NoPreviousSlotPurchases)
    {
        //no last purchase, so use min price
        m_CurrentPriceToUseForBuying = boost::lexical_cast<double>(m_HackedInD3faultCampaign0_MinPrice);
    }
    else
    {
        //there is a last purchase

        //check to see if last purchase is expired
        double lastSlotFilledAkaPurchasedExpireDateTime = (boost::lexical_cast<double>(m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedStartTimestamp)+((double)(boost::lexical_cast<double>(m_HackedInD3faultCampaign0_SlotLengthHours)*(3600.0))));
        if(currentDateTime >= lastSlotFilledAkaPurchasedExpireDateTime)
        {
            //expired, so use min price
            m_CurrentPriceToUseForBuying = boost::lexical_cast<double>(m_HackedInD3faultCampaign0_MinPrice);
        }
        else
        {
            //not expired, so calculate

            //calculate internal price
            m_CurrentPriceToUseForBuying = calculateCurrentPrice(currentDateTime, boost::lexical_cast<double>(m_HackedInD3faultCampaign0_MinPrice), (boost::lexical_cast<double>(m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedPurchasePrice)*2.0), lastSlotFilledAkaPurchasedExpireDateTime, boost::lexical_cast<double>(m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedPurchaseTimestamp));

            //TODOreq: when we use this variable later, we first check m_HackedInD3faultCampaign0_NoPreviousSlotPurchases. If true, we then use the PURCHASE time as the start time. That is also true for when the last purchase is already expired (check both and in mentioned order). We also need to make sure we always check m_HackedInD3faultCampaign0_NoPreviousSlotPurchases before checking if last purchase is expired, because there isn't a last purchase so we'd get undefined results
            m_LastSlotFilledAkaPurchasedExpireDateTime_ToBeUsedAsStartDateTimeIfTheBuySucceeds = boost::lexical_cast<std::string>(lastSlotFilledAkaPurchasedExpireDateTime);

            m_HackedInD3faultCampaign0_LastSlotPurchasesIsExpired = false;
        }
    }

    //std::ostringstream currentPriceStream; //TODOreq: all other number->string conversions should use this method
    //TODOreq: why the fuck does setprecision(6) give me 8 decimal places and setprecision(8) gives me 10!?!?!? lol C++. BUT SERIOUSLY THOUGH I need to make sure that this doesn't fuck up shit and money get leaked/lost/whatever. Maybe rounding errors in this method of converting double -> string, and since I'm using it as the actual value in the json doc, I need it to be accurate. The very fact that I have to use 6 instead of 8 just makes me wonder...
    //TODOreq: ^ok wtf now i got 7 decimal places, so maybe it's dependent on the value........... maybe i should just store/utilize as many decimal places as possible (maybe trimming to 8 _ONLY_ when the user sees the value).... and then force the bitcoin client to do the rounding shizzle :-P
    //currentPriceStream /*<< setprecision(6)*/ << m_CurrentPriceToUseForBuying; //TODOreq:rounding errors maybe? I need to make a decision on that, and I need to make sure that what I tell them it was purchased at is the same thing we have in our db
    //m_CurrentPriceToUseForBuyingString = currentPriceStream.str();
    m_CurrentPriceToUseForBuyingString = boost::lexical_cast<std::string>(m_CurrentPriceToUseForBuying); //or snprintf? I'm thinking lexical_cast will keep as many decimal places as it can (but is that what i want, or do i want to mimic bitcoin rounding?), so...
    m_PurchaseTimestampForUseInSlotItselfAndAlsoUpdatingCampaignDocAfterPurchase = boost::lexical_cast<std::string>(currentDateTime); //TO DOnereq: just fixed this, but need to make sure other code paths (???) also do the same: i was doing 'currentDateTime' twice and in between couchbase requests... but i need to make sure that the timestamp used to calculate the price is the same one stored alongside it in the json doc as 'purchase time' (and possibly start time, depending if last is expired). just fixed it now i'm pretty sure. since i was calculating it twice and the second time was later after a couchbase request, the timestamp would have been off by [sub]-milliseconds... and we've all seen superman 3 (actually i'm not sure that i have (probably have when i was young as shit, but don't remember it (hmmmm time for a rewatch)), but i've definitely seen office space (oh yea i want to rewatch that also!))

    if(userBalance >= m_CurrentPriceToUseForBuying) //idk why but this makes me cringe. suspicion of rounding errors and/or other hackability...
    {
        //proceed with trying to lock account

        //make 'account locked' json doc [by just appending to the one we already have]

        pt.put(JSON_USER_ACCOUNT_SLOT_ATTEMPTING_TO_FILL, m_AdSlotAboutToBeFilledIfLockIsSuccessful);
        //TODOreq (read next TODOreq first): seriously it appears as though i'm still not getting the user's input to verify the slot index. we could have a 'buy event' that could update m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedSlotIndex and even push it to their client just milliseconds before they hit 'buy' step 2.... and if they have sufficient funds they'd now buy at near-exactly twice what they wanted to [and be pissed]. i need some boolean guards in javascript surrounding a "are you sure" prompt thingo (except i can't/shouldn't depend on js so gah) -> jsignal-emit-with-that-value -> unlock the boolean guards (to allow buy events to update that slot index and/or price). an amateur wouldn't see this HUGE bug
        //TODOreq: ^bleh, i should 'lock in the slot index' when the user clicks buy step 1, DUH (fuck js) (if they receive a buy event during that time, we undo step 1, requiring them to click it again [at a now double price]. BUT it's _VITAL_ that i don't allow the internal code to modify their locked in slot index and allow them to proceed forward with the buy)
        pt.put(JSON_USER_ACCOUNT_SLOT_TO_ATTEMPT_TO_FILL_IT_WITH, m_SlotFillerToUseInBuy);
        std::ostringstream jsonDocBuffer;
        write_json(jsonDocBuffer, pt, false);
        std::string accountLockedForBuyJsonDoc = jsonDocBuffer.str();

        store_SETonly_CouchbaseDocumentByKeyWithInputCasBegin(userAccountKey(m_CurrentlyLoggedInUsername), accountLockedForBuyJsonDoc, m_UserAccountUnlockedJustBeforeBuyingCas, StoreCouchbaseDocumentByKeyRequest::SaveOutputCasMode);
        m_WhatTheStoreWithInputCasSavingOutputCasWasFor = HACKEDIND3FAULTCAMPAIGN0BUYSTEP2bLOCKACCOUNTFORBUYINGSETWITHCASSAVINGCAS;
    }
    else
    {
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WText("Insufficient Funds. Add Funds Here: ", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ACCOUNT), ABC_ANCHOR_TEXTS_ACCOUNT, m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        resumeRendering();
        //TO DOnereq(did opposite, no account activity while purchase is in progress): doesn't belong here but i probalby shouldn't let them do ANY purchase activity when they have 'pending'/unconfirmed (bitcoin) funds. It just complicates the logic too much and I'm probably going to use account locking for that too
    }
}
void AnonymousBitcoinComputingWtGUI::buySlotStep2d3faultCampaign0ButtonClicked()
{
    if(m_SlotIndexImmediatelyAfterBuyStep1wasPressed_aka_PreviousSlotIndexToTheOneTheyWantToBuy != m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedSlotIndex)
    {
        //shouldn't get here but maybe via a race condition?
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WText("Someone just bought that slot, so you have to click step 1 again to buy the NEXT slot", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        rollBackToBeforeBuyStep1ifNeeded();
        return;
    }

    int currentIndex = m_AllSlotFillersComboBox->currentIndex();
    if(currentIndex < 0 || currentIndex >= m_AllSlotFillersAdsCount)
    {
        //user forged invalid range for combo box
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WText("Haha nice try forging dat combo box", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        return;
    }

    //handle two or more clicks perfectly timed that might be accidental, and additionally on the button we haven't yet rolled back (they want to buy another, it has to be after they click step 1 again)
    if(m_BuyInProgress)
    {
        return;
    }
    m_BuyInProgress = true;

    deferRendering(); //haha it hit me while laying in bed that you can call this multiple times. never thought i'd use that feature but blamo here i am using it and loving Wt :). TODOreq: make sure the 'second' deferRendering is called in all error cases from here
    //TO DOnereqopt: idk if required or optional or optimization, but it makes sense that we consolidate the defer/resume renderings here. The result of that button clicked (the signal that brought us here) makes us do TWO couchbase round trips: the first one to verify balance and lock account, the second to do the actual slot-filling/buying. It makes sense to only defer rendering once HERE, and to resume rendering only once LATER (error, or slot filled, etc). It might not matter at all.

    //DO ACTUAL BUY (check balance + lock user account, then associate slot with slot filler, aka fill the slot)

    new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
    new WText("Attempting to buy ad space with advertisement: '" + m_AllSlotFillersComboBox->currentText() + "'...", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);

    //TODOreq: change to 'sending them the time' [for use in js], because it not only solves the timezones/incorrect-system-time problem, but it also has the added benefit of GUARANTEEING that the price they see will be above the internal price (because of latency)... so much so that we don't need them to send it in anymore. the slot index would be enough. Also worth noting that the js should use an accurate timer to calculate the 'current time' (from the time we sent them and they saved). I doubt setInterval is a high precision timer. I was originally thinking they could delta the time we sent them against their system time, but then there'd be the problem of if they changed their system time while running then it'd fuck shit up (not a big deal though since it's a rare case. still if js has a high precision timer object ("time elapsed since x"), use that).
    //^that guarantee depends on all wt nodes being time sync'd like fuck, but that goes without saying

    //TODOreq: handle cases where no puchases yet (use min), no next, no balls, current expired (use min),etc

    //had calculate current price here, but moved it to a few sub-milliseconds later

    m_SlotFillerToUseInBuy = adSpaceSlotFillerKey(m_CurrentlyLoggedInUsername, boost::lexical_cast<std::string>(currentIndex));

    getCouchbaseDocumentByKeySavingCasBegin(userAccountKey(m_CurrentlyLoggedInUsername));
    m_WhatTheGetSavingCasWasFor = HACKEDIND3FAULTCAMPAIGN0BUYSTEP2aVERIFYBALANCEANDGETCASFORSWAPLOCKGET;

    //TO DOnereq (vague so pretty much done (spread out to many other reqs)): the above get shouldn't fail since they need to already be logged in to get this far, but what the fuck do i know? it probably CAN (db overload etc), so i need to account for that
    //^everything can DEFINITELY fail, it is only success that can ever at most PROBABLY happen

    //TODOreq: failed to lock your account for the purchase (???), failed to buy/fill the slot (insufficient funds, or someone beat us to it (unlock in both cases))
}
void AnonymousBitcoinComputingWtGUI::rollBackToBeforeBuyStep1ifNeeded()
{
    if(m_BuyStep2placeholder)
    {
        delete m_BuyStep2placeholder;
        m_BuyStep2placeholder = 0;
        m_BuySlotFillerStep1Button->enable();
    }
}
void AnonymousBitcoinComputingWtGUI::verifyUserHasSufficientFundsAndThatTheirAccountIsntAlreadyLockedAndThenStartTryingToLockItIfItIsntAlreadyLocked(const string &userAccountJsonDoc, u_int64_t cas, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        //TODOreq: now that i think of it, a lot of these errors, whether 500 or 'lcbOpSuccess' are going to require rolling back things. maybe just gui type stuff, but maybe db type stuff. lots to consider.
        //TODOreq: 500 internal server error

        //temp:
        cerr << "verifyUserHasSufficientFundsAndThatTheirAccountIsntAlreadyLockedAndThenStartTryingToLockItIfItIsntAlreadyLocked db error" << endl;

        //TODOreq: MAYBE rollBackToBeforeBuyStep1(); for this, lcbOpFail, and all subsequent db/lcbOpFails before the slot fill completes. this is the first db error where it's applicable though, so don't go backwards

        resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //TODOreq: 500 internal server error
        cerr << "TOTAL SYSTEM FAILURE: got 'key does not exist' in 'verify user has sufficient funds', which requires them to already be logged in (them logging in proves the key exists)" << endl;

        resumeRendering();
        return;
    }

    //since we haven't done anything we can still roll back with ease. common-ish race condition
    if(m_SlotIndexImmediatelyAfterBuyStep1wasPressed_aka_PreviousSlotIndexToTheOneTheyWantToBuy != m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedSlotIndex)
    {
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WText("Someone just bought that slot, so you have to click step 1 again to buy the NEXT slot", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        rollBackToBeforeBuyStep1ifNeeded();
        resumeRendering();
        return;
    }


    m_UserAccountUnlockedJustBeforeBuyingJson = userAccountJsonDoc; //our starting point for when we debit the user account during unlock (after the slot fill later on), and also used after one last check that the slot doesn't exist before user account locking (just below is the dispatch for that)
    m_UserAccountUnlockedJustBeforeBuyingCas = cas;
    ptree pt;
    std::istringstream is(userAccountJsonDoc);
    read_json(is, pt);

    m_HackedInD3faultCampaign0_LastSlotPurchasesIsExpired = true; //default to true until we prove it false

    std::string ALREADY_LOCKED_CHECK_slotToAttemptToFillAkaPurchase = pt.get<std::string>(JSON_USER_ACCOUNT_SLOT_ATTEMPTING_TO_FILL, "n");
    if(ALREADY_LOCKED_CHECK_slotToAttemptToFillAkaPurchase == "n")
    {
        //not already locked

        //TODOreq: make sure the slot doesn't already exist before proceeding, because it might and if it does we will fuck shit up if we lock the user account AGAIN towards one we already purchased, which is a race condition that happens a lot when js is disabled
        int slotIndexToAttemptToBuy = 0;
        if(!m_HackedInD3faultCampaign0_NoPreviousSlotPurchases)
        {
            slotIndexToAttemptToBuy = (boost::lexical_cast<int>(m_SlotIndexImmediatelyAfterBuyStep1wasPressed_aka_PreviousSlotIndexToTheOneTheyWantToBuy) + 1);
        }
        m_AdSlotIndexToBeFilledIfLockIsSuccessful_AndForUseInUpdateCampaignDocAfterPurchase = boost::lexical_cast<std::string>(slotIndexToAttemptToBuy);
        m_AdSlotAboutToBeFilledIfLockIsSuccessful = adSpaceCampaignSlotKey("d3fault", "0", m_AdSlotIndexToBeFilledIfLockIsSuccessful_AndForUseInUpdateCampaignDocAfterPurchase);
        getCouchbaseDocumentByKeyBegin(m_AdSlotAboutToBeFilledIfLockIsSuccessful);
        m_WhatTheGetWasFor = ENSURESLOTABOUTTOLOCKUSERACCOUNTTOWARDSATTEMPTINGTOPURCHASEDOESNTEXISTBECAUSEIFITDOESANDWEBOUGHTITSTATEWOULDBEFUCKED;
    }
    else
    {
        new WText("Our records indicate you're already trying to purchase a slot. Either do one at a time, or perhaps you should try logging out and back in again to remedy the issue", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        resumeRendering();
        //we probably want to allow them to continue with the buy even if the account is locked, so long as it's locked "to" the same one they're trying to buy now (buy failure recovery) -- TODOreq: this should go either in login recovery or here, but not both (since getting _here_ requires being logged in (ALTHOUGH ACTUALLY NVM, IT IS POSSIBLE TO GET HERE IF THE DB/node FAILS WHILE THEY ARE LOGGED IN LOOKING AT BUY PAGE FFFFFF))
        //Semi-outdated:
        //TO DOnereq(describes the roll-back hack): account already locked so error out of this buy. they're logged in elsewhere and trying to buy two things at once? etc. It jumps at me that it might be a place to do 'recovery' code, but I think for now I'm only going to do that during 'login' (TO DOnereq: maybe doing recovery at login isn't such a good idea (at least, the PROCEED WITH BUY kind isn't a good idea (rollback is DEFINITELY good idea (once we verify that they didn't get it(OMG RACE CONDITION HACKABLE TO DOnereq: they are on two machines they log in on a different one just after clicking "buy" and get lucky so that their account IS locked, but the separate-login-machine checks to see if they got the slot. When it sees they didn't, it rolls back their account. Meanwhile the original machine they clicked "buy" on is still trying to buy the slot (by LCB_ADD'ing the slot). The hack depends on the machine they press "buy" on being slower (more load, etc) than the alternate one they log in to (AND NOTE, BY MACHINE I MEAN WT SERVER, not end user machine). So wtf rolling back is dangerous? Wat do. It would also depend on the buying machine crashing immediately after the slot is purchased, because otherwise it would be all like 'hey wtf who unlocked that account motherfucker, I was still working on it' and at least error out TO DOnereq(can't think of a way to detect that scenario because recovery-possy can cas-swap-unlock+debitting also)))))
        //^very real race condition vuln aside, what i was getting at originally is that maybe it's not a good idea to do "recover->proceed-with-buy" because who the fuck knows how much later they'd log in... and like maybe currentPrice would be waaaaay less than when they originally locked/tried-and-failed. It makes sense to at least ask them: "do you want to try this buy again" and then to also recalculate the price (which means re-locking the account (which probably has security considerations to boot)) on login
    }
}
void AnonymousBitcoinComputingWtGUI::continueRecoveringLockedAccountAtLoginAttempt_OrAnalyzingSlotBuyBeatToThePunch(const string &maybeExistentSlot, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        if(!m_LoggedIn)
        {
            //login recovery
            m_LoginStatusMessagesPlaceholder->setText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE);
        }
        else
        {
            //slot purchase beat to the punch
            new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
            m_AdvertisingBuyAdSpaceD3faultCampaign0Widget->addWidget(new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE));
        }
        resumeRendering();
        return;
    }

    if(!lcbOpSuccess)
    {
        //slot not purchased/filled
        if(!m_LoggedIn)
        {
            //login recovery
            m_LoginStatusMessagesPlaceholder->setText("We're sorry, our system has experience a temporary failure and your account is locked until someone else buys that slot you tried to purchase during your last session. Please try logging in again once that slot is purchased");
        }
        else
        {
            //slot purchase beat to the punch

            //this will never happen, because the LCB_ADD for the slot had to have failed via lcbOp failure in order for us to get here (which means someone did an LCB_ADD already... yet this code block says that it ISN'T filled...
            new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
            m_AdvertisingBuyAdSpaceD3faultCampaign0Widget->addWidget(new WText("Nothing makes any sense, suicide is the only option"));

            cerr << "Nothing makes any sense, suicide is the only option" << endl;
        }
        resumeRendering();
        return;
        //TODOreq: allow them to buy it, because clearly they've expressed interest
        //TODOoptimization: think of a way to allow them to roll back.... though be careful because this is dangerous
    }
    else
    {
        //slot purchased/filled, now check if it was us

        ptree pt;
        std::istringstream is(maybeExistentSlot);
        read_json(is, pt);

        std::string slotFilledWith = pt.get<std::string>(JSON_AD_SPACE_CAMPAIGN_SLOT_FILLED_WITH);
        if(slotFilledWith == m_AccountLockedRecoveryWhatTheUserWasTryingToFillTheSlotWithHack)
        {
            //we got it, so do NOTHING because recovery process will handle it, a TODOoptimization would be to trigger recovery possy code, but that requires a merge and fuck that
            if(!m_LoggedIn)
            {
                //login recovery
                m_LoginStatusMessagesPlaceholder->setText("Try logging in again in like 1 minute (don't ask why)"); //fuck teh police~
            }
            else
            {
                //slot purchase beat to the punch
                //this also should never happen (actually it can once i enable "continue buying declared attempt after fail after account lock"), but even then it's a race condition I can't wrap my head completely around right now. Something like 'buy and go forward with locked buy on a new tab/session simultaneously, the normal buying session would get here.
                new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
                m_AdvertisingBuyAdSpaceD3faultCampaign0Widget->addWidget(new WText("Don't use multiple tabs/windows plz"));

            }
            resumeRendering();
        }
        else
        {
            //we didn't get it, so it is now safe to roll-back/unlock the user account without debitting

            ptree pt2;
            std::istringstream is2(m_UserAccountLockedJsonToMaybeUseInAccountRecoveryAtLogin);
            read_json(is2, pt2);

            pt2.erase(JSON_USER_ACCOUNT_SLOT_ATTEMPTING_TO_FILL);
            pt2.erase(JSON_USER_ACCOUNT_SLOT_TO_ATTEMPT_TO_FILL_IT_WITH);

            std::ostringstream userAccountUnlockedJsonBuffer;
            write_json(userAccountUnlockedJsonBuffer, pt2, false);

            //cas-swap-unlock(no-debit)-accepting-fail (neighbor logins could have recovered)
            store_SETonly_CouchbaseDocumentByKeyWithInputCasBegin(userAccountKey(m_CurrentlyLoggedInUsername), userAccountUnlockedJsonBuffer.str(), m_CasFromUserAccountLockedAndStuckLockedButErrRecordedDuringRecoveryProcessAfterLoginOrSomethingLoLWutIamHighButActuallyNotNeedMoneyToGetHighGuhLifeLoLSoErrLemmeTellYouAboutMyDay, StoreCouchbaseDocumentByKeyRequest::DiscardOuputCasMode);
            m_WhatTheStoreWithInputCasWasFor = LOGINACCOUNTRECOVERY_AND_SLOTPURCHASEBEATTOTHEPUNCH_UNLOCKINGWITHOUTDEBITTINGUSERACCOUNT;
        }
    }
}
void AnonymousBitcoinComputingWtGUI::userAccountLockAttemptFinish_IfOkayDoTheActualSlotFillAdd(u_int64_t casFromLockSoWeCanSafelyUnlockLater, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        //TODOreq: 500 internal server error?

        //temp:
        cerr << "userAccountLockAttemptFinish_IfOkayDoTheActualSlotFillAdd db error" << endl;

        resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WText("It appears you're logged in to more than one location and are trying to make multiple purchases simultaneously. You can only do one at a time", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        rollBackToBeforeBuyStep1ifNeeded();
        resumeRendering();
        return;
    }

    //if we get here, the user account is locked and pointing to the slot we're about to fill

    //TODOoptional: we could maybe check the buy event race condition here too and roll back including unlocking without debitting, but eh seems kinda dangerous without thinking it through so idk...

    m_CasFromUserAccountLockSoWeCanSafelyUnlockLater = casFromLockSoWeCanSafelyUnlockLater;
    //TODOreq: handle beat to punch error case (unlock without deducting)

    //do the LCB_ADD for the slot!

    //figure out start timestamp
    m_StartTimestampUsedInNewPurchase = m_PurchaseTimestampForUseInSlotItselfAndAlsoUpdatingCampaignDocAfterPurchase; //start timestamp is purchase timestamp if there is no last purchase, or if the last purchase is already expired
    if(!m_HackedInD3faultCampaign0_NoPreviousSlotPurchases)
    {
        //there is a last purchase, so we base our start time off of that if it isn't expired
        if(!m_HackedInD3faultCampaign0_LastSlotPurchasesIsExpired)
        {
            //last isn't expired, so we put the start of this new one to be when that one expires
            m_StartTimestampUsedInNewPurchase = m_LastSlotFilledAkaPurchasedExpireDateTime_ToBeUsedAsStartDateTimeIfTheBuySucceeds;
        }
    }

    //populate 'slot' doc
    ptree pt;
    pt.put(JSON_AD_SPACE_CAMPAIGN_SLOT_PURCHASE_TIMESTAMP, m_PurchaseTimestampForUseInSlotItselfAndAlsoUpdatingCampaignDocAfterPurchase);
    pt.put(JSON_AD_SPACE_CAMPAIGN_SLOT_START_TIMESTAMP, m_StartTimestampUsedInNewPurchase);
    pt.put(JSON_AD_SPACE_CAMPAIGN_SLOT_PURCHASE_PRICE, m_CurrentPriceToUseForBuyingString);
    pt.put(JSON_AD_SPACE_CAMPAIGN_SLOT_FILLED_WITH, m_SlotFillerToUseInBuy);
    std::ostringstream jsonDocBuffer;
    write_json(jsonDocBuffer, pt, false);
    store_ADDbyDefault_WithoutInputCasCouchbaseDocumentByKeyBegin(m_AdSlotAboutToBeFilledIfLockIsSuccessful, jsonDocBuffer.str());
    m_WhatTheStoreWithoutInputCasWasFor = BUYAKAFILLSLOTWITHSLOTFILLERSTOREWITHOUTINPUTCAS;
}
void AnonymousBitcoinComputingWtGUI::slotFillAkaPurchaseAddAttemptFinished(bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        //TODOreq: 500 internal server error?

        //temp:
        cerr << "slotFillAkaPurchaseAddAttemptFinished db error" << endl;

        resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //getting beat to the punch
        unlockUserAccountWithoutDebittingIfSlotDeclaredAttemptingToPurchaseIsPurchasedBySomeoneElse(m_AdSlotAboutToBeFilledIfLockIsSuccessful, m_SlotFillerToUseInBuy, m_UserAccountUnlockedJustBeforeBuyingJson, m_CasFromUserAccountLockSoWeCanSafelyUnlockLater);
        rollBackToBeforeBuyStep1ifNeeded();
        return;
    }

    //if we get here, the slot fill purchased and we need to make transaction, unlock user account (debitting), then update campaign doc

    //create transaction doc using lcb_add accepting fail -- i can probably re-use this code later (merge into functions), for now KISS
    ptree pt;
    pt.put(JSON_TRANSACTION_BUYER, m_CurrentlyLoggedInUsername);
    pt.put(JSON_TRANSACTION_SELLER, "d3fault"); //TODOreq: implied/deducable from key name...
    pt.put(JSON_TRANSACTION_AMOUNT, m_CurrentPriceToUseForBuyingString);
    std::ostringstream transactionBuffer;
    write_json(transactionBuffer, pt, false);
    store_ADDbyDefault_WithoutInputCasCouchbaseDocumentByKeyBegin(transactionKey("d3fault", "0", m_AdSlotIndexToBeFilledIfLockIsSuccessful_AndForUseInUpdateCampaignDocAfterPurchase), transactionBuffer.str(), StoreCouchbaseDocumentByKeyRequest::AddMode);
    m_WhatTheStoreWithoutInputCasWasFor = CREATETRANSACTIONDOCSTOREWITHOUTINPUTCAS; //TODOreq: it goes without saying that 'recovery possy' needs it's own set of these, so as not to conflict
    //TODOreq: i need a way of telling the backend that certain adds (like this one) are okay to fail. But really I already need a whole slew of error case handling to be coded into the backend, I guess I'll just do it later? So basically for adds where fails are not ok, we need to have two code paths... but for this one where the add failing is ok, we just pick up with one code path. I think the easiest way of doing this is to return a bool telling whether or not the add succeeded, and to just ignore it if it doesn't matter. But since there's many many ways of failing, maybe I should be passing around the LCB_ERROR itself? So far I've tried to keep front end and back end separate, so idk maybe "bool opTypeFail and bool dbTypeFail", where the first one is relating to cas/add fails and the second is like "500 internal server error" (of course, the backend would have retried backing off exponentially etc before resorting to that)
}
void AnonymousBitcoinComputingWtGUI::transactionDocCreatedSoCasSwapUnlockAcceptingFailUserAccountDebitting(bool dbError)
{
    if(dbError)
    {
        //TODOreq: 500 internal server error?

        cerr << "transactionDocCreatedSoCasSwapUnlockAcceptingFailUserAccountDebitting db error" << endl;

        resumeRendering();
        return;
    }
#if 0
    if(!lcbOpSuccess)
    {
        //we accept fail and carry on!
    }
#endif

    //if we get here, the transaction doc is created (by someone, not necessarily us)... so carry on...

    //TODOreq: user account unlock -- i worry that the user would be able to fuck with the state from the time they hit buy2 -> now (and keeping the rendering deferred until now might be the solution)... like i can't quite put my finger on it, but something to do with "when we repopulate the json doc for unlocking the account, they've modified something so that now something unintentional happens" (a big one being balance not being deducted properly, but it could be something else subtler)

    //user account debiting + unlock
    //first parse the json doc that we saved before doing the lock
    ptree pt;
    std::istringstream is(m_UserAccountUnlockedJustBeforeBuyingJson);
    read_json(is, pt);
    //now do the debit of the balance and put it back in the json doc
    double userBalance = boost::lexical_cast<double>(pt.get<std::string>(JSON_USER_ACCOUNT_BALANCE));
    userBalance -= m_CurrentPriceToUseForBuying; //TODOreq: again, just scurred of rounding errors etc. I think as long as I  use 'more precision than needed' (as much as an double provides), I should be ok...
    std::string balanceString = boost::lexical_cast<std::string>(userBalance);
    m_CurrentlyLoggedInUsersBalanceForDisplayOnlyLabel->setText(balanceString); //TODOoptional: jumping the gun by setting it here before even finishing the cas-swap of their user-account doc, BUT the transaction doc and slot fill/add are already complete at this point so we KNOW it will 'become' accurate soon (if we fail, recovery possy will do it)... so seeing as this is visual ONLY, it's acceptable. Unlike account creditting ("done with this key"), which waits until the cas swap succeeds before updating the label, we KNOW that if we get here that it will work (and if we were to wait for the cas swap and the cas swap FAILED, the balance label would never be updated (because recovery possy is different app (i guess i could... (*shoots self in face*)))
    pt.put(JSON_USER_ACCOUNT_BALANCE, balanceString);
    //now convert the json doc back to string for couchbase
    std::ostringstream jsonDocWithBalanceDeducted;
    write_json(jsonDocWithBalanceDeducted, pt, false);

    store_SETonly_CouchbaseDocumentByKeyWithInputCasBegin(userAccountKey(m_CurrentlyLoggedInUsername), jsonDocWithBalanceDeducted.str(), m_CasFromUserAccountLockSoWeCanSafelyUnlockLater, StoreCouchbaseDocumentByKeyRequest::DiscardOuputCasMode);
    m_WhatTheStoreWithInputCasWasFor = HACKEDIND3FAULTCAMPAIGN0BUYPURCHASSUCCESSFULSOUNLOCKUSERACCOUNTSAFELYUSINGCAS; //TODOreq: handle cas swap UNLOCK failure. can happen and sometimes will, but mostly won't. handle it by doing nothing but continuing the process

    //TO DOnereq(set without cas): should we have done a multi-get to update the campaign doc also? i actually think not since we don't have the cas for that doc (but we could have gotten it~). more importantly i don't know whether or not i need the cas for it when i do the swap. seems safer no doubt.. but really would there be any contesting?? maybe only subsequent buys milliseconds later (probably in error, but maybe the campaign is just liek popular ya know ;-P)... so yea TODOreq do a cas-swap of the campaign doc, makes sure we're only n+1 the last purchased.... and i think maybe do an exponential backoff trying... because it's the same code that runs for when subsequent ones are purchased seconds later? or no... maybe we just do a set without cas because we know that all attempts to buy will fail until that set is complete (because they try to buy n+1 (which is what we just bought (so it will fail safely as beat-to-thepunch)), not n+2 (until the set we're about to do, is done))
    //TODOreq:^^Does the order of user-account-unlock and setting-the-campaign-doc-with-new-last-purchase matter? can we do them asynchronously (AKA HERE IS WHERE WE'D START 'UPDATING'/LCB_SET'ing CAMPAIGN), or do we need to do one before the other and then wait for whichever goes first to complete?


    //TODOreq: need to update the adSpaceSlotsd3fault0 to make the filler that just succeded the most recent purchased
    //TODOreq: dispatch the buy event to our neighbor wt nodes so they can post the buy event to their end users etc (how da fuq? rpc? get-and-subscribe polling model? guh). polling is easiest and really having N[ode-count] hits ever i[N]terval [m]seconds is still not that expensive (get and subscribe also acts as caching layer so that we don't get 10-freaking-thousand hits per second to that same document)
    //TODOreq: definitely doesn't belong here, but a get-and-subscribe / polling method could do a "poll" for the current value and when the poll finishes, the last end-user could have disconnected in that time and we MIGHT think it would be an error but we'd be wrong. in that case we'd just discard/disregard that last polled value (and of course, not do any more polling until at least one end-user connected)
    //TODOreq: ^it goes without saying that 'buy' events do not use the cached value. (NVM:) Hell, even hitting 'buy step 1' should maybe even do a proper get for the actual value (or just nudge cache to do it for him).... BUT then it becomes a DDOS point. since I'm thinking the polling intervals will be like 100-500ms, doing a non-cached get like that on "buy step 1" is probably not necessary (/NVM). But we definitely absolutely need to do it for buy step 2 (duh), and that is NOT a DDOS point because noobs would be giving ya monay each time so i mean yea ddos all ya fuggan want nobs <3

    //TODOreq: even though it seemed like a small optimization to do both the user-account-unlock and updating of campaign doc with a new 'last purchased' slot, that pattern of programming will lead to disaster so i shouldn't do it just in principle. Yes with this one case it wouldn't have led to disaster, but actually it MIGHT have given extreme circumstances. it opens up a race condition: say the 'user account unlock' happens really fast and the 'update campaign doc' happens to take a while. if we gave control back to the user after the 'user account unlock' was finished, they could do some action that would then conflict with the m_WhatThe[blahblahblah]WasFor related to the 'update campaign doc' store. As in, when the 'update campaign doc' finishes and gets posted back to the WApplication, the user's actions could have started something else and then we wouldn't have handled the finishing of the 'update campaign doc' properly (which would mean that we never notify our neighbors of buy event? actually not a problem if we do 'get-and-subscribe-polling' like i think i'm going to do (BUT LIKE I SAID THIS ONE GETS LUCKY, BUT AN OVERALL PATTERN TO AVOID)). So I'm going to do the successive sets synchronously, despite seeing a clear opportunity for optimization
}
void AnonymousBitcoinComputingWtGUI::doneUnlockingUserAccountAfterSuccessfulPurchaseSoNowUpdateCampaignDocCasSwapAcceptingFail_SettingOurPurchaseAsLastPurchase(bool dbError)
{
    if(dbError)
    {
        //TODOreq: 500 internal server error

        //temp:
        cerr << "doneUnlockingUserAccountAfterSuccessfulPurchaseSoNowUpdateCampaignDocCasSwapAcceptingFail_SettingOurPurchaseAsLastPurchase db error" << endl;

        resumeRendering();
        return;
    }
#if 0 //nvm this isn't valid proof of the hack, because recovery possy can beat them to it in a not-so-rare race condition...
    if(!lcbOpSuccess)
    {
        //TODOreq: handle and notify

        //temp:
        cerr << "TOTAL SYSTEM FAILURE: fail-after-buy-attempt 'roll back hack' (they still got slot): " << m_CurrentlyLoggedInUsername << endl;

        resumeRendering();
        return;
    }
#endif

    //TODOreq (DONE I THINK, transaction doc solves this): need a recovery path that both SAFELY (see vuln above) debits user account and also updates campaign doc.

    //if we get here, the user-account is unlocked and debitted (probably by us, but not necessarily)

    //we already have the campaign doc because we were looking at it when we hit buy step 1!
    ptree pt;
    std::istringstream is(m_HackedInD3faultCampaign0JsonDocForUpdatingLaterAfterSuccessfulPurchase);
    read_json(is, pt);

    //boost::optional<ptree&> lastSlotFilledAkaPurchased = pt.get_child_optional("lastSlotFilledAkaPurchased");
    //boost::optional<ptree&> currentSlotOnDisplay = pt.get_child_optional("currentSlotOnDisplay");
    //boost::optional<ptree&> nextSlotOnDisplay = pt.get_child_optional("nextSlotOnDisplay");

    //TODOreq: ^starting to think the current/next slot shit can/SHOULD go on it's own document (that, as we mentioned somewhere, can be re-figuredout/generated somewhat easily). getting the doc should maybe check expiration, existence of 'next', etc... but idk i'll design that shit later. BUT TODOreq I think it's worth noting that actually having current/next on the same doc as the 'lastPurchased' might give weird conflicting errors against the "rand() % 10" recovery possy. for example, they're supposed to not care when a campaign doc update fails because of cas failure. they are supposed to not care because then that means one of the other recovery possy (or probable driver) must have done it. HOWEVER, by putting current/next on there, an 'expiration' event would now trigger the cas failure and the campaign doc might not get updated. BUT now that I've written all that out, I think all that would mean is that the recovery process would run AGAIN (this time using the current/next'd doc as the orig) and succeed...... but it's definitely worth writing and considering and analyzing and ensuring whatever i come up with is ok

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
    lastPurchasedPt.put(JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED_INDEX, m_AdSlotIndexToBeFilledIfLockIsSuccessful_AndForUseInUpdateCampaignDocAfterPurchase);
    lastPurchasedPt.put(JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED_PURCHASE_TIMESTAMP, m_PurchaseTimestampForUseInSlotItselfAndAlsoUpdatingCampaignDocAfterPurchase);
    lastPurchasedPt.put(JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED_START_TIMESTAMP, m_StartTimestampUsedInNewPurchase);
    lastPurchasedPt.put(JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED_PURCHASE_PRICE, m_CurrentPriceToUseForBuyingString);

    pt.put_child(JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED, lastPurchasedPt);

    //TODOreq: we also need to MAYBE update 'current' and/or 'next', depending on if it's appropriate. the only one we DEFINITELY update is 'last purchased'. if current is expired and there is a next -- holy race condition batman TODOreq (maybe solution is to not list "next" in the doc... and shit i'm actually starting to think maybe i shouldn't even list current. maybe whether or not there is a current is detected on the fly and when needed... by seeing if the (slotIndex-of-the-just-expired-current)+1 exists.. and if it doesn't then shit we don't have a current :-P... yea actually i'm liking that more and more :). simplifies managing the campaign doc too, only two states: no purchase and last purchase (whether it's expired is not that relevant to us, except maybe later when doing a buy announce event (but... polling...?). STILL, I do feel as though I need to keep track of 'current' SOMEWHERE. My thoughts are drifting and although it does make sense to do the 'get n+1' and that's our next if it exists method, who and when exactly would that be done? If it's the "client" of abc (so like the page where i'm fucking dancing naked coding tripping balls on video), who initiates the "ok expired now gimmeh next" and then abc does the n+1 shit,... err i guess what i mean is who has the final say of what n is to begin with? should for example the client die and come back and forget n, how would it figure out it's state again? I GUESS we could just keep doing n+1 or n-1 until we found it, and hell we could even use hella accurate jumping to the middle of all of our slots based on SlotLength, slot0 start time (polled), and the current date time. Would be accurate as fuck list jump with maybe an extra get to correct off by one. SO maybe the abc client does keep track of "n" and then gives it to abc as a parameter a la "hint". Client(dance-vid-page): abc.getNextSlot(n_justExpired /*or n+1, but it doesn't matter who does the incrementing*/), and if it's 0 (state lost somehow) then abc does the list jump mechanism to find it asap. but guh, i haven't worked out any of the details of the abc client (dance-vid-page), so idfk what it's state or code or ANYTHING even looks like on that side of things...
    //^regardless of what i choose, the fact that we can figure out the current slot by doing math with near 100% accuracy (3 gets tops i'd estimate (first for slot 0, second for our guess based on math (so yea 4 gets if you include campaign doc get), third as potential recovery from mistake in math (timezones? idfk...) -- assumes slotLengthHours never changes of course...
    //^^i think i'll cross that bridge when i get there (buy event and 'gimmeh ad plx client request'). dats how da hackas doeet (that being said, i should now consider current and next to NOT be in the campaign json doc (will re-add later if determined i should))

    //if current is expired and there is no next, we fill our latest purchase in as the current
    //if

    std::ostringstream updatedCampaignJsonDocBuffer;
    write_json(updatedCampaignJsonDocBuffer, pt, false);

    //CAS-swap-accepting-fail
    store_SETonly_CouchbaseDocumentByKeyWithInputCasBegin(adSpaceCampaignKey("d3fault", "0"), updatedCampaignJsonDocBuffer.str(), m_HackedInD3faultCampaign0CasForSafelyUpdatingCampaignDocLaterAfterSuccessfulPurchase, StoreCouchbaseDocumentByKeyRequest::DiscardOuputCasMode);
    m_WhatTheStoreWithInputCasWasFor = HACKEDIND3FAULTCAMPAIGN0USERACCOUNTUNLOCKDONESOUPDATECAMPAIGNDOCSETWITHINPUTCAS;
}
void AnonymousBitcoinComputingWtGUI::doneUpdatingCampaignDocSoErrYeaTellUserWeAreCompletelyDoneWithTheSlotFillAkaPurchase(bool dbError)
{
    if(dbError)
    {
        //TODOreq: 500 internal server error
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WText("There was an internal error, but you probably still purchased the slot. Try refreshing in a few minutes to see if you got the slot. Don't worry, if you didn't get the purchase, you won't be charged", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget); //TODOreq: similar errors where appropriated
        rollBackToBeforeBuyStep1ifNeeded();

        //temp:
        cerr << "doneUpdatingCampaignDocSoErrYeaTellUserWeAreCompletelyDoneWithTheSlotFillAkaPurchase db error" << endl;

        resumeRendering();
        return;
    }

    //if we get here, the campaign doc is updated (probably by us, but not necessarily)

    if(!environment().ajax())
    {
        //no-js doesn't have 'buy event', so we get the fresh results even though it would be a TODOoptimization to have just saved them (but if recovery possy did campaign update in race condition, we still would have to do the get we're about to do (if the cas swap succeeded, we could have just called finishShowingAdvertisingBuyAdSpaceD3faultCampaign0Widget directly for that optimization)). KISS
        doHackyOneTimeBuyEventUpdateThingoForNoJavascriptUser();
        //getCouchbaseDocumentByKeySavingCasBegin(adSpaceCampaignKey("d3fault", "0"));
        //m_WhatTheGetSavingCasWasFor = HACKEDIND3FAULTCAMPAIGN0GET;
    }

    new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
    new WText("SUCCESS! You bought the ad space for BTC: " + m_CurrentPriceToUseForBuyingString, m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
    //buy event rolls us back to before buy step 1 [for next slot]. will come shortly for js, and we just scheduled it above for no-js
    resumeRendering();
    //TODOoptimization(dumb): getting here means were the driver (unless i merge this function, but yea) so we can now ahead-of-time-a-la-event-driven update the 'get and subscribe' people (at least the ones on this wt node)... but we don't really NEED to...
}
void AnonymousBitcoinComputingWtGUI::doneAttemptingUserAccountUnlockingWithoutDebittingFor_LoginRecovery_And_SlotPurchaseBeatToThePunch(bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        if(!m_LoggedIn)
        {
            //login recovery
            m_LoginStatusMessagesPlaceholder->setText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE);
        }
        else
        {
            //slot purchase beat to the punch
            new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
            m_AdvertisingBuyAdSpaceD3faultCampaign0Widget->addWidget(new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE));
        }
        resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        if(!m_LoggedIn)
        {
            //login recovery
            m_LoginStatusMessagesPlaceholder->setText("Uhh, try logging in again. I'm not sure what just happened..."); //seriously, can't wrap my head around this right now... xD (neighbor login beat them to the recovery.... but what should i _DO_??? it is perhaps NOTHING, but i can't guarantee that the user-account isn't now locked towards something else (and still in failed state (or something confusing fuck it))
        }
        else
        {
            //slot purchase beat to the punch
            //not 100% sure (just like for login recovery above), but i think this is a similar race condition only available once i enable 'going forward' on a locked user account as the one in the chunk of code that initiated the db hit that we are not responding to
            new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
            m_AdvertisingBuyAdSpaceD3faultCampaign0Widget->addWidget(new WText("Don't use multiple tabs/windows plz"));
        }
        resumeRendering();
        return;
    }

    if(!m_LoggedIn)
    {
        //account locked recovery complete, make it look like a regular old login
        doLoginTasks();
    }
    else
    {
        //slot purchase beat to the punch
        //TODOreq: schedule getting new slot to populate buy step 1 proper, right now it rolls back, but to the outdated values
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WText("Sorry, someone else bought the slot just moments before you...", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        if(!environment().ajax())
        {
            //js only sees above error in rare race condition, and if/when they do they will get the subscription update to 'fix' it soon anyways. no-js needs to request the update manually
            doHackyOneTimeBuyEventUpdateThingoForNoJavascriptUser();
        }
        resumeRendering();
    }
}
void AnonymousBitcoinComputingWtGUI::doHackyOneTimeBuyEventUpdateThingoForNoJavascriptUser()
{
    //this is effectively a db get, but preferably from the subscription cache (else a ddos point), of the current contents of the campaign doc. it is for no-js clients, because if they have js then they will already be receiving updates.

    //all no-js:
    //1) navigating to campaign 0 for the first time
    //2) coming back to campaign 0 ('re-subscribe' analog)
    //3) 'sorry, someone else bought the slot just moments before you' (multiple)
    //4) successful slot purchase

    getAndSubscribeCouchbaseDocumentByKeySavingCas(adSpaceCampaignKey("d3fault", "0"), GetCouchbaseDocumentByKeyRequest::GetAndSubscribeJustKiddingNoJavascriptHereSoIjustWantONEsubscriptionUpdateValOrAHardGetIfNeedBeKthx);
    m_CurrentlySubscribedTo = SINGLESUBSCRIPTIONUPDATEFORNOJAVASCRIPTUSERSHACKPLXTHX;
    deferRendering(); //getAndSubscribe doesn't defer/resume, but we need to since no-js (the switch/'case' for the above special no-js enum does the resume)
}
void AnonymousBitcoinComputingWtGUI::getCouchbaseDocumentByKeyBegin(const std::string &keyToCouchbaseDocument)
{
    deferRendering();
    GetCouchbaseDocumentByKeyRequest couchbaseRequest(sessionId(), this, keyToCouchbaseDocument, GetCouchbaseDocumentByKeyRequest::DiscardCASMode);
    ABC_SERIALIZE_COUCHBASE_REQUEST_AND_SEND_TO_COUCHBASE_ON_RANDOM_MUTEX_PROTECTED_MESSAGE_QUEUE(Get)
}
void AnonymousBitcoinComputingWtGUI::getCouchbaseDocumentByKeySavingCasBegin(const string &keyToCouchbaseDocument)
{
    deferRendering();
    GetCouchbaseDocumentByKeyRequest couchbaseRequest(sessionId(), this, keyToCouchbaseDocument, GetCouchbaseDocumentByKeyRequest::SaveCASMode);
    ABC_SERIALIZE_COUCHBASE_REQUEST_AND_SEND_TO_COUCHBASE_ON_RANDOM_MUTEX_PROTECTED_MESSAGE_QUEUE(Get)
}
void AnonymousBitcoinComputingWtGUI::getAndSubscribeCouchbaseDocumentByKeySavingCas(const string &keyToCouchbaseDocument, GetCouchbaseDocumentByKeyRequest::GetAndSubscribeEnum subscribeMode)
{
    GetCouchbaseDocumentByKeyRequest couchbaseRequest(sessionId(), this, keyToCouchbaseDocument, GetCouchbaseDocumentByKeyRequest::SaveCASMode, subscribeMode);
    ABC_SERIALIZE_COUCHBASE_REQUEST_AND_SEND_TO_COUCHBASE_ON_RANDOM_MUTEX_PROTECTED_MESSAGE_QUEUE(Get)
}
//original semi-outdated code with comments
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
        //^the problem is that we need a place to put the error message, and need a generic message too!
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
void AnonymousBitcoinComputingWtGUI::store_ADDbyDefault_WithoutInputCasCouchbaseDocumentByKeyBegin(const std::string &keyToCouchbaseDocument, const std::string &couchbaseDocument, StoreCouchbaseDocumentByKeyRequest::LcbStoreMode_AndWhetherOrNotThereIsInputCasEnum storeMode)
{
    deferRendering();
    StoreCouchbaseDocumentByKeyRequest couchbaseRequest(sessionId(), this, keyToCouchbaseDocument, couchbaseDocument, storeMode);
    ABC_SERIALIZE_COUCHBASE_REQUEST_AND_SEND_TO_COUCHBASE_ON_RANDOM_MUTEX_PROTECTED_MESSAGE_QUEUE(Store)
}
void AnonymousBitcoinComputingWtGUI::store_SETonly_CouchbaseDocumentByKeyWithInputCasBegin(const string &keyToCouchbaseDocument, const string &couchbaseDocument, u_int64_t cas, StoreCouchbaseDocumentByKeyRequest::WhatToDoWithOutputCasEnum whatToDoWithOutputCasEnum)
{
    deferRendering();
    StoreCouchbaseDocumentByKeyRequest couchbaseRequest(sessionId(), this, keyToCouchbaseDocument, couchbaseDocument, cas, whatToDoWithOutputCasEnum);
    ABC_SERIALIZE_COUCHBASE_REQUEST_AND_SEND_TO_COUCHBASE_ON_RANDOM_MUTEX_PROTECTED_MESSAGE_QUEUE(Store)
}
void AnonymousBitcoinComputingWtGUI::storeLarge_ADDbyDefault_WithoutInputCasCouchbaseDocumentByKeyBegin(const string &keyToCouchbaseDocument, const string &couchbaseDocument, StoreCouchbaseDocumentByKeyRequest::LcbStoreMode_AndWhetherOrNotThereIsInputCasEnum storeMode)
{
    deferRendering();
    StoreCouchbaseDocumentByKeyRequest couchbaseRequest(sessionId(), this, keyToCouchbaseDocument, couchbaseDocument, storeMode);
    ABC_SERIALIZE_COUCHBASE_REQUEST_AND_SEND_TO_COUCHBASE_ON_RANDOM_MUTEX_PROTECTED_MESSAGE_QUEUE(StoreLarge)
}
void AnonymousBitcoinComputingWtGUI::storeLarge_SETonly_CouchbaseDocumentByKeyWithInputCasBegin(const string &keyToCouchbaseDocument, const string &couchbaseDocument, u_int64_t cas, StoreCouchbaseDocumentByKeyRequest::WhatToDoWithOutputCasEnum whatToDoWithOutputCasEnum)
{
    deferRendering();
    StoreCouchbaseDocumentByKeyRequest couchbaseRequest(sessionId(), this, keyToCouchbaseDocument, couchbaseDocument, cas, whatToDoWithOutputCasEnum);
    ABC_SERIALIZE_COUCHBASE_REQUEST_AND_SEND_TO_COUCHBASE_ON_RANDOM_MUTEX_PROTECTED_MESSAGE_QUEUE(StoreLarge)
}
void AnonymousBitcoinComputingWtGUI::getCouchbaseDocumentByKeyFinished(const std::string &keyToCouchbaseDocument, const std::string &couchbaseDocument, bool lcbOpSuccess, bool dbError)
{
    resumeRendering();
    //this hack STILL makes me giggle like a little school girl, tee hee. the hack becomes life and all you know, and is no longer 'considered' a hack
    switch(m_WhatTheGetWasFor)
    {
    case HACKEDIND3FAULTCAMPAIGN0BUYSTEP1GET:
    {
        buySlotPopulateStep2d3faultCampaign0(couchbaseDocument, lcbOpSuccess, dbError);
    }
        break;
    case ENSURESLOTABOUTTOLOCKUSERACCOUNTTOWARDSATTEMPTINGTOPURCHASEDOESNTEXISTBECAUSEIFITDOESANDWEBOUGHTITSTATEWOULDBEFUCKED:
    {
        ensureSlotDoesntExistThenContinueWithLockingUserAccountIntoAttemptingToBuyItAndThenGoAheadWithBuyEtcIfThatSucceeds(lcbOpSuccess, dbError);
    }
        break;
    case ONLOGINACCOUNTLOCKEDRECOVERY_AND_SLOTPURCHASEBEATTOTHEPUNCH_DOESSLOTEXISTCHECK:
    {
        continueRecoveringLockedAccountAtLoginAttempt_OrAnalyzingSlotBuyBeatToThePunch(couchbaseDocument, lcbOpSuccess, dbError);
    }
        break;
    case GETNICKNAMEOFADSLOTFILLERNOTINALLADSLOTFILLERSDOCFORADDINGITTOIT_THEN_TRYADDINGTONEXTSLOTFILLERINDEXPLZ:
    {
        m_NewAdSlotFillerAccountTab->getAdSlotFillerThatIsntInAllAdSlotFillersAttemptFinished_soAddItToAllAddSlotFillersAndInitiateSlotFillerAddAtNextIndex(couchbaseDocument, lcbOpSuccess, dbError); //'slot filler add' = buyer creating a new ad; 'slot fill add' = buyer buying ad slot using created ad. xD too fucking subtle of a difference (but i'm so amped right now i can grasp the difference for the rest of this month long hack marathon)...
    }
        break;
    case GETALLADSLOTFILLERSDOCFORVIEWING:
    {
        m_ViewAllExistingAdSlotFillersAccountTab->attemptToGetAllAdSlotFillersFinished(couchbaseDocument, lcbOpSuccess, dbError);
    }
        break;
    case HACKYMULTIGETAPAGEWORTHOFADSLOTFILLERS:
    {
        m_ViewAllExistingAdSlotFillersAccountTab->oneAdSlotFillerFromHackyMultiGetAttemptFinished(keyToCouchbaseDocument, couchbaseDocument, lcbOpSuccess, dbError);
    }
        break;
    case INITIALINVALIDNULLGET:
    default:
        cerr << "got a couchbase 'get' response we weren't expecting:" << endl << "unexpected key: " << keyToCouchbaseDocument << endl << "unexpected value: " << couchbaseDocument << endl;
        //TODOreq: 500 internal server error shown to user?
        break;
    }
    //if(environment().ajax())
    //{
    //    triggerUpdate(); //this and enableUpdates are probably not needed if i'm always defer/resuming
    //}
}
void AnonymousBitcoinComputingWtGUI::getCouchbaseDocumentByKeySavingCasFinished(const string &keyToCouchbaseDocument, const string &couchbaseDocument, u_int64_t cas, bool lcbOpSuccess, bool dbError)
{
    resumeRendering();
    switch(m_WhatTheGetSavingCasWasFor)
    {
    case LOGINATTEMPTGET:
    {
        loginIfInputHashedEqualsDbInfo(couchbaseDocument, cas, lcbOpSuccess, dbError);
    }
        break;
#if 0
    case HACKEDIND3FAULTCAMPAIGN0GET:
    {
        finishShowingAdvertisingBuyAdSpaceD3faultCampaign0Widget(couchbaseDocument, cas); //TODOreq: tempted to pass lcbOpsSuccess and dbError into here, but this is ultimately going to change when i implement get-and-subscribe-via-polling so i'm not sure they still apply (not sure they don't apply also!)
    }
        break;
#endif
    case HACKEDIND3FAULTCAMPAIGN0BUYSTEP2aVERIFYBALANCEANDGETCASFORSWAPLOCKGET:
    {
        verifyUserHasSufficientFundsAndThatTheirAccountIsntAlreadyLockedAndThenStartTryingToLockItIfItIsntAlreadyLocked(couchbaseDocument, cas, lcbOpSuccess, dbError);
    }
        break;
    case GETUSERACCOUNTFORGOINGINTOGETTINGBITCOINKEYMODE:
    {
        m_AddFundsAccountTab->checkNotAttemptingToFillAkaPurchaseSlotThenTransitionIntoGettingBitcoinKeyState(couchbaseDocument, cas, lcbOpSuccess, dbError);
    }
        break;
    case DETERMINEBITCOINSTATEBUTDONTLETTHEMPROCEEDIFLOCKEDATTEMPTINGTOFILLAKAPURCHASESLOT:
    {
        m_AddFundsAccountTab->determineBitcoinStateButDontLetThemProceedForwardIfLockedAttemptingToFillAkaPurchaseSlot(couchbaseDocument, cas, lcbOpSuccess, dbError);
    }
        break;
    case GETBITCOINKEYSETNCURRENTPAGETOSEEWHATPAGEITISONANDIFITISLOCKED:
    {
        m_AddFundsAccountTab->analyzeBitcoinKeySetN_CurrentPageDocToSeeWhatPageItIsOnAndIfItIsLocked(couchbaseDocument, cas, lcbOpSuccess, dbError);
    }
        break;
    case GETBITCOINKEYSETNACTUALPAGETOSEEIFUUIDONITENOUGHROOM:
    {
        m_AddFundsAccountTab->gotBitcoinKeySetNpageYSoAnalyzeItForUUIDandEnoughRoomEtc(couchbaseDocument, cas, lcbOpSuccess, dbError);
    }
        break;
    case GETBITCOINKEYSETNPAGEYANDIFITEXISTSLOOPAROUNDCHECKINGUUIDBUTIFNOTEXISTMAKEITEXISTBITCH:
    {
        m_AddFundsAccountTab->getBitcoinKeySetNPageYAttemptFinishedSoCheckItIfItExistsAndMakeItIfItDont(couchbaseDocument, cas, lcbOpSuccess, dbError);
    }
        break;
    case GETHUGEBITCOINKEYLISTCURRENTPAGE:
    {
        m_AddFundsAccountTab->hugeBitcoinKeyListCurrentPageGetComplete(couchbaseDocument, cas, lcbOpSuccess, dbError);
    }
        break;
    case GETHUGEBITCOINKEYLISTACTUALPAGEFORANALYZINGANDMAYBECLAIMINGKEYRANGE:
    {
        m_AddFundsAccountTab->getHugeBitcoinKeyListActualPageAttemptCompleted(couchbaseDocument, cas, lcbOpSuccess, dbError);
    }
        break;
    case CREDITCONFIRMEDBITCOINAMOUNTAFTERANALYZINGUSERACCOUNT:
    {
        m_AddFundsAccountTab->creditConfirmedBitcoinAmountAfterAnalyzingUserAccount(couchbaseDocument, cas, lcbOpSuccess, dbError);
    }
        break;
    case GETALLADSLOTFILLERSTODETERMINENEXTINDEXANDTOUPDATEITAFTERADDINGAFILLERGETSAVINGCAS:
    {
        m_NewAdSlotFillerAccountTab->determineNextSlotFillerIndexAndThenAddSlotFillerToIt(couchbaseDocument, cas, lcbOpSuccess, dbError);
    }
        break;
    case INITIALINVALIDNULLGETSAVINGCAS:
    default:
        cerr << "got a couchbase 'get' (saving cas) response we weren't expecting:" << endl << "unexpected key: " << keyToCouchbaseDocument << endl << "unexpected value: " << couchbaseDocument << endl << "unexpected cas: " << cas << endl;
        //TODOreq: 500 internal server error shown to user?
        break;
    }
    //if(environment().ajax())
    //{
    //    triggerUpdate(); //this and enableUpdates are probably not needed if i'm always defer/resuming
    //}
}
void AnonymousBitcoinComputingWtGUI::getAndSubscribeCouchbaseDocumentByKeySavingCas_UPDATE(const string &keyToCouchbaseDocument, const string &couchbaseDocument, u_int64_t cas, bool lcbOpSuccess, bool dbError)
{
    switch(m_CurrentlySubscribedTo)
    {
    case HACKEDIND3FAULTCAMPAIGN0GETANDSUBSCRIBESAVINGCAS:
    {
        finishShowingAdvertisingBuyAdSpaceD3faultCampaign0Widget(couchbaseDocument, cas); //TODOreq: lcbOpSuccess + dbError? my backend already needs special attention to this stuff for the request hackily being used for polling, so that may be related to this
    }
        break;
    case SINGLESUBSCRIPTIONUPDATEFORNOJAVASCRIPTUSERSHACKPLXTHX:
    {
        resumeRendering(); //get and subscribe mode generally does not defer/resume, but this is a no-js hack so it makes an exception!
        finishShowingAdvertisingBuyAdSpaceD3faultCampaign0Widget(couchbaseDocument, cas);
        m_CurrentlySubscribedTo = INITIALINVALIDNULLNOTSUBSCRIBEDTOANYTHING; //end of subscription after just one (so we don't 'unsubscribe' or send change session id stuff)
    }
        break;
    case INITIALINVALIDNULLNOTSUBSCRIBEDTOANYTHING:
    default:
        cerr << "got a couchbase 'get and subscribe' (saving cas) response/update/populate we weren't expecting: " << endl << "unexpected key: " << keyToCouchbaseDocument << endl << "unexpected value: " << couchbaseDocument << endl << "unexpected cas: " << cas << endl; //TODOreq: changing subscriptions might make an old subscription go to the wrong subscription callback!!! there is also a race condition where an unsubscribe request is sent but not processed by backend, who gives Posts it and we get to this code path. should just do nothing imo. the race condition about CHANGING subscriptions is much worse/harder-to-fix
        break;
    }
}
void AnonymousBitcoinComputingWtGUI::storeWithoutInputCasCouchbaseDocumentByKeyFinished(const string &keyToCouchbaseDocument, bool lcbOpSuccess, bool dbError)
{
    //no need to pass in the value, and we probably don't even need the key here... but might in the future. i do know that if i do the enableUpdates/triggerUpdate async design that i would probably very much need the key at least here. imagine they dispatch two "add" requests before the first one can return. but meh that's a pretty big design overhaul as it is so not worrying about it right now (if only there was a generator that could...)
    resumeRendering();
    switch(m_WhatTheStoreWithoutInputCasWasFor)
    {
    case REGISTERATTEMPTSTOREWITHOUTINPUTCAS:
    {
        registerAttemptFinished(lcbOpSuccess, dbError);
    }
        break;
    case BUYAKAFILLSLOTWITHSLOTFILLERSTOREWITHOUTINPUTCAS:
    {
        slotFillAkaPurchaseAddAttemptFinished(lcbOpSuccess, dbError);
    }
        break;
    case CREATETRANSACTIONDOCSTOREWITHOUTINPUTCAS:
    {
        transactionDocCreatedSoCasSwapUnlockAcceptingFailUserAccountDebitting(dbError);
    }
        break;
    case LARGE_ADIMAGEUPLOADBUYERSETTINGUPADSLOTFILLERFORUSEINPURCHASINGLATERONSTOREWITHOUTINPUTCAS:
    {
        m_NewAdSlotFillerAccountTab->storeLargeAdImageInCouchbaseDbAttemptComplete(keyToCouchbaseDocument, lcbOpSuccess, dbError);
    }
        break;
    case BITCOINKEYSETNPAGEYCREATIONVIALCBADD:
    {
        m_AddFundsAccountTab->doneAttemptingBitcoinKeySetNnextPageYcreation(dbError);
    }
        break;
    case INITIALINVALIDNULLSTOREWITHOUTINPUTCAS:
    default:
        cerr << "got a couchbase 'store without input cas' response we weren't expecting:" << endl << "unexpected key: " << keyToCouchbaseDocument << endl;
        //TODOreq: 500 internal server error?
        break;
    }
    //if(environment().ajax())
    //{
    //    triggerUpdate();
    //}
}
void AnonymousBitcoinComputingWtGUI::storeCouchbaseDocumentByKeyWithInputCasFinished(const string &keyToCouchbaseDocument, bool lcbOpSuccess, bool dbError)
{
    resumeRendering();
    switch(m_WhatTheStoreWithInputCasWasFor)
    {
    case HACKEDIND3FAULTCAMPAIGN0BUYPURCHASSUCCESSFULSOUNLOCKUSERACCOUNTSAFELYUSINGCAS:
    {
        doneUnlockingUserAccountAfterSuccessfulPurchaseSoNowUpdateCampaignDocCasSwapAcceptingFail_SettingOurPurchaseAsLastPurchase(dbError);
    }
        break;
    case HACKEDIND3FAULTCAMPAIGN0USERACCOUNTUNLOCKDONESOUPDATECAMPAIGNDOCSETWITHINPUTCAS:
    {
        doneUpdatingCampaignDocSoErrYeaTellUserWeAreCompletelyDoneWithTheSlotFillAkaPurchase(dbError);
    }
        break;
    case LOGINACCOUNTRECOVERY_AND_SLOTPURCHASEBEATTOTHEPUNCH_UNLOCKINGWITHOUTDEBITTINGUSERACCOUNT:
    {
        doneAttemptingUserAccountUnlockingWithoutDebittingFor_LoginRecovery_And_SlotPurchaseBeatToThePunch(lcbOpSuccess, dbError);
    }
        break;
    case UNLOCKUSERACCOUNTFROMBITCOINGETTINGKEYTOBITCOINHAVEKEY:
    {
        m_AddFundsAccountTab->doneAttemptingToUnlockUserAccountFromBitcoinGettingKeyToBitcoinHaveKey(lcbOpSuccess, dbError);
    }
        break;
    case CLAIMBITCOINKEYONBITCOINKEYSETVIACASSWAP:
    {
        m_AddFundsAccountTab->doneAttemptingToUpdateBitcoinKeySetViaCASswapAkaClaimingAKey(lcbOpSuccess, dbError);
    }
        break;
    case HUGEBITCOINKEYLISTKEYRANGECLAIMATTEMPT:
    {
        m_AddFundsAccountTab->doneAttemptingHugeBitcoinKeyListKeyRangeClaim(lcbOpSuccess, dbError);
    }
        break;
    case SAFELYUNLOCKBITCOINKEYSETNCURRENTPAGEDOCFAILINGTOLERABLY:
    {
        m_AddFundsAccountTab->doneAttemptingToUnlockBitcoinKeySetN_CurrentPage(lcbOpSuccess, dbError);
    }
        break;
    case HUGEBITCOINKEYLISTPAGECHANGE:
    {
        m_AddFundsAccountTab->doneChangingHugeBitcoinKeyListCurrentPage(dbError);
    }
        break;
    case RELOCKBITCOINKEYSETN_CURRENTPAGETONONEXISTENTPAGEZASNECESSARYOPTIMIZATION:
    {
        m_AddFundsAccountTab->doneReLockingBitcoinKeySetN_CurrentPage_withNewFromPageZvalue(dbError);
    }
        break;
    case CREDITCONFIRMEDBITCOINBALANCEFORCURRENTPAYMENTKEYCASSWAPUSERACCOUNT:
    {
        m_AddFundsAccountTab->doneAttemptingCredittingConfirmedBitcoinBalanceForCurrentPaymentKeyCasSwapUserAccount(lcbOpSuccess, dbError);
    }
        break;
    case UPDATEALLADSLOTFILLERSDOCSINCEWEJUSTCREATEDNEWADSLOTFILLER:
    {
        m_NewAdSlotFillerAccountTab->doneAttemptingToUpdateAllAdSlotFillersDocSinceWeJustCreatedANewAdSlotFiller(lcbOpSuccess, dbError);
    }
        break;
    case INITIALINVALIDNULLSTOREWITHCAS:
    default:
        cerr << "got a couchbase 'set' with input cas response we weren't expecting:" << endl << "unexpected key: " << keyToCouchbaseDocument << endl;
        //TODOreq: 500 internal server error?
        break;
    }
    //if(environment().ajax())
    //{
    //    triggerUpdate();
    //}
}
void AnonymousBitcoinComputingWtGUI::storeCouchbaseDocumentByKeyWithInputCasSavingOutputCasFinished(const string &keyToCouchbaseDocument, u_int64_t outputCas, bool lcbOpSuccess, bool dbError)
{
    resumeRendering();
    switch(m_WhatTheStoreWithInputCasSavingOutputCasWasFor)
    {
    case HACKEDIND3FAULTCAMPAIGN0BUYSTEP2bLOCKACCOUNTFORBUYINGSETWITHCASSAVINGCAS:
    {
        userAccountLockAttemptFinish_IfOkayDoTheActualSlotFillAdd(outputCas, lcbOpSuccess, dbError);
    }
        break;
    case USERACCOUNTBITCOINLOCKEDINTOGETTINGKEYMODE:
    {
        m_AddFundsAccountTab->userAccountBitcoinLockedIntoGettingKeyAttemptComplete(outputCas, lcbOpSuccess, dbError);
    }
        break;
    case LOCKINGBITCOINKEYSETNINTOFILLINGNEXTPAGEMODE:
    {
        m_AddFundsAccountTab->attemptToLockBitcoinKeySetNintoFillingNextPageModeComplete(outputCas, lcbOpSuccess, dbError);
    }
        break;
    case INITIALINVALIDNULLSTOREWITHCASSAVINGCAS:
    default:
        cerr << "got a couchbase 'set' with input cas saving output cas response we weren't expecting:" << endl << "unexpected key: " << keyToCouchbaseDocument << endl;
        //TODOreq: 500 internal server error?
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
    if(!internalPathMatches(ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE_D3FAULT_CAMPAIGN_0)) //we would check ALL eligible subscriptions before going into this if block (future proof)
    {
        //now in 'non-subscribeable' area

        if(m_CurrentlySubscribedTo != INITIALINVALIDNULLNOTSUBSCRIBEDTOANYTHING)
        {
            //we need to unsubscribe from whatever we're subscribed to

            //looks like i need some auxillary message queues for communicating with backend... fffff /lazy
            //also applies to changing sessionId, guh. 'get' and 'store' just don't cut-it/qualify (though i could PROBABLY hack them in xD). I'm mainly hesitant because it is MACRO HELL dealing with that shiz (i was tempted to do it for 'cas' vs. 'no-cas' etc, but ultimately said fuck it and just hacked onto the regular (but in hindsight, it wouldn't have worked [easily] because i only have 1x couchbase get/store callback!). I smell a refactor commit, which scares me because they often are the last times i touch codebases

            getAndSubscribeCouchbaseDocumentByKeySavingCas(adSpaceCampaignKey("d3fault", "0"), GetCouchbaseDocumentByKeyRequest::GetAndSubscribeUnsubscribeMode); //TODOreq: to be future proof for use with other subscriptions i'd have to call a method passing in m_CurrentlySubscribedTo in order to get the key to pass in here (easy but lazy)

            //we don't expect a response from the backend, so this is our frontend's flag that we are now unsubscribed
            m_CurrentlySubscribedTo = INITIALINVALIDNULLNOTSUBSCRIBEDTOANYTHING;
        }

        if(isHomePath(newInternalPath)) //why do we have this both here and in the constructor? because setInternalPath() does not go to/through the constructor, so showHome() would never be called if they click a link etc that does setInternalPath("/home"). They'd only be able to get there by navigating directly to the site/home without a session (which is a common case but yea~)
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
                    showAdvertisingBuyAdSpaceD3faultWidget();
                    return;
                }
                showAdvertisingBuyAdSpaceWidget();
                return;
            }
            showAdvertisingWidget();
            return;
        }
        if(newInternalPath == ABC_INTERNAL_PATH_ACCOUNT)
        {
            showAccountWidget();
            return;
        }
        if(newInternalPath == ABC_INTERNAL_PATH_REGISTER)
        {
            showRegisterWidget();
            return;
        }

        //404 Not Found
        if(!m_404NotFoundWidget)
        {
            m_404NotFoundWidget = new WContainerWidget(m_MainStack);
            new WText("404 Not Found", m_404NotFoundWidget);
        }
        m_MainStack->setCurrentWidget(m_404NotFoundWidget);
    }

    //TODOreq: to be future proof for other subscriptions, we need to do an 'unsubscribe' -> subscribe-to-different code path, and maybe they can/should share the ride to the backend ;-P. low priority for now since only one subscription. but really they don't need to share a ride since unsubscribe doesn't respond. we just async send unsubscribe and then do the new subscribe, ez

    beginShowingAdvertisingBuyAdSpaceD3faultCampaign0Widget();
}
void AnonymousBitcoinComputingWtGUI::handleRegisterButtonClicked()
{
    if(m_RegisterUsernameLineEdit->validate() != WValidator::Valid || m_RegisterPasswordLineEdit->validate() != WValidator::Valid)
    {
        m_RegisterStatusMessagesPlaceholder->setText(m_LettersNumbersOnlyValidatorAndInputFilter->invalidNoMatchText());
        return;
    }

    std::string username = m_RegisterUsernameLineEdit->text().toUTF8();
    std::string passwordPlainText = m_RegisterPasswordLineEdit->text().toUTF8();

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
    jsonDoc.put(JSON_USER_ACCOUNT_BALANCE, "0.0");
    jsonDoc.put(JSON_USER_ACCOUNT_PASSWORD_HASH, base64PasswordSaltHashed);
    jsonDoc.put(JSON_USER_ACCOUNT_PASSWORD_SALT, base64Salt);
    jsonDoc.put(JSON_USER_ACCOUNT_BITCOIN_STATE, JSON_USER_ACCOUNT_BITCOIN_STATE_NO_KEY);
    //string'ify json
    std::ostringstream jsonDocBuffer;
    write_json(jsonDocBuffer, jsonDoc, false);
    std::string jsonString = jsonDocBuffer.str();

    store_ADDbyDefault_WithoutInputCasCouchbaseDocumentByKeyBegin(userAccountKey(username), jsonString);
    m_WhatTheStoreWithoutInputCasWasFor = REGISTERATTEMPTSTOREWITHOUTINPUTCAS;
}
void AnonymousBitcoinComputingWtGUI::handleLoginButtonClicked()
{
    if(m_LoginUsernameLineEdit->validate() != WValidator::Valid || m_LoginPasswordLineEdit->validate() != WValidator::Valid)
    {
        m_LoginStatusMessagesPlaceholder->setText(m_LettersNumbersOnlyValidatorAndInputFilter->invalidNoMatchText());
        return;
    }

    //TODOreq: it might make sense to clear the line edit's in the login widget, and to save the login credentials as member variables, and to also clear the password member variable (memset) to zero after it has been compared with the db hash (or even just after it has been hashed (so just make the hash a member xD))
    std::string username = m_LoginUsernameLineEdit->text().toUTF8();
    deferRendering(); //2-layered defer/resume because we want to stay deferred until possible login recovery completes
    getCouchbaseDocumentByKeySavingCasBegin(userAccountKey(username));
    m_WhatTheGetSavingCasWasFor = LOGINATTEMPTGET;
}
void AnonymousBitcoinComputingWtGUI::loginIfInputHashedEqualsDbInfo(const std::string &userProfileCouchbaseDocAsJson, u_int64_t casOnlyUsedWhenDoingRecoveryAtLogin, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        m_LoginStatusMessagesPlaceholder->setText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE);

        //temp:
        cerr << "loginIfInputHashedEqualsDbInfo db error" << endl;

        resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        usernameOrPasswordYouProvidedIsIncorrect();
        resumeRendering();
        return;
    }

    //if we get here, the username exists

    ptree pt;
    std::istringstream is(userProfileCouchbaseDocAsJson);
    read_json(is, pt);

    std::string passwordHashInBase64FromDb = pt.get<std::string>(JSON_USER_ACCOUNT_PASSWORD_HASH);
    std::string passwordSaltInBase64FromDb = pt.get<std::string>(JSON_USER_ACCOUNT_PASSWORD_SALT);

    std::string passwordFromUserInput = m_LoginPasswordLineEdit->text().toUTF8();
    std::string passwordHashFromUserInput = sha1(passwordFromUserInput + passwordSaltInBase64FromDb);

    //hmm i COULD use base64decode but it doesn't matter which of the two converts to the other's format
    std::string passwordHashBase64FromUserInput = base64Encode(passwordHashFromUserInput);

    if(passwordHashBase64FromUserInput == passwordHashInBase64FromDb)
    {
        //login
        //TODOreq: it's either DDOS point or a bugged feature to do account locked recovery that "polls" for the existence of the transaction (or was it slot?) document. If we do exponential backoff, it could be HOURS/DAYS before someone [else] buys the slot to make us confident in unlocking-without-debitting.... so the exponential backoff will have grown to be a ridiculously long time (unless i do a max cap, but see next sentence about ddos). If I poll for existence at set interval or at a maxed cap, then it could MAYBE be ddos'd if the user was somehow able to predictably lock the user-account and then crash the node before the slot fill. ACTUALLY that's fucking highly unlikely and would indicate a different bug altogether... so fuck this problem. We should use exponential + max cap of like 3-5 seconds (still a DDOS kinda if they then log in to every Wt node (now every Wt node they try to log in through is doing that 3-5 second polling

        //TODOreq: if the user logs in and the account is locked, even before we offer them fancy "did you want this?" functionality.. we should still allow them to buy the slot they were trying to buy when the account got to this fucked state (clearly they are interested). i can either let a locked account slide by (sounds dangerous, but actually i can't think of any reasons why it would fuck shit up (just a teensy hunch is all)) if it's pointing to the slot you're trying to purchase, or i can implement the "fancy" recovery functionality presented on login...

        changeSessionId();

        if(m_CurrentlySubscribedTo != INITIALINVALIDNULLNOTSUBSCRIBEDTOANYTHING)
        {
            //we're subscribed to something, so need to notify backend that we changed session id
            getAndSubscribeCouchbaseDocumentByKeySavingCas(adSpaceCampaignKey("d3fault", "0"), GetCouchbaseDocumentByKeyRequest::GetAndSubscribeChangeSessionIdMode); //TODOreq: see handleInternalPath's comment about making key dynamic (ez)
        }

        m_CurrentlyLoggedInUsername = m_LoginUsernameLineEdit->text().toUTF8(); //TODOreq(depends on how much security deferRending gives me): do we need to keep rendering deferred until we capture this (or perhaps capture it earlier (when it's used to LCB_GET)?)? Maybe they could 'change usernames' at precise moment to give them access to account that isn't theirs. idk [wt] tbh, but sounds plausible. should be captured as m_UsernameToAttemptToLoginAs (because this one is reserved for post-login), and then use simple assignment HERE

        //TODOreq: do this on key not found fail (and all other relevant errors) also (if only there was a generator that let you see if there was any kind of error and then let you drill down).
        m_LoginUsernameLineEdit->setText("");
        m_LoginPasswordLineEdit->setText("");

        //we save a copy of it here, but we don't display it until doLoginTasks is called, or recovery completes (which also calls that)
        m_CurrentlyLoggedInUsersBalanceStringForDisplayingOnly = pt.get<std::string>(JSON_USER_ACCOUNT_BALANCE);

        std::string slotToAttemptToFillAkaPurchase_ACCOUNT_LOCKED_TEST = pt.get<std::string>(JSON_USER_ACCOUNT_SLOT_ATTEMPTING_TO_FILL, "n");
        if(slotToAttemptToFillAkaPurchase_ACCOUNT_LOCKED_TEST == "n")
        {
            doLoginTasks();
        }
        else
        {
            //account locked, so do recovery steps

            //in the following, slotToAttemptToFillAkaPurchase == the slot itself, we've already determined that the FIELD in UserAccount exists...

            //if slotToAttemptToFillAkaPurchase exists and we got it, do nothing
            //if slotToAttemptToFillAkaPurchase exists and we didn't get it, unlock [without debitting]
            //NOPE FOR NOW /lazy: if slotToAttemptToFillAkaPurchase doesn't exist, offer user to get it (just redirect to page? but that would require login guh)
            unlockUserAccountWithoutDebittingIfSlotDeclaredAttemptingToPurchaseIsPurchasedBySomeoneElse(slotToAttemptToFillAkaPurchase_ACCOUNT_LOCKED_TEST, pt.get<std::string>(JSON_USER_ACCOUNT_SLOT_TO_ATTEMPT_TO_FILL_IT_WITH), userProfileCouchbaseDocAsJson, casOnlyUsedWhenDoingRecoveryAtLogin); //if it's empty, we are stuck waiting. if it's purchased by us, recovery possy will unlock us (debitting)
        }
    }
    else
    {
        //login fail
        usernameOrPasswordYouProvidedIsIncorrect();
        resumeRendering();
        return;
    }
}
//method used for both login account recovery and attempting buying a slot and simply getting beat to the punch
void AnonymousBitcoinComputingWtGUI::unlockUserAccountWithoutDebittingIfSlotDeclaredAttemptingToPurchaseIsPurchasedBySomeoneElse(const std::string &slotLockedUserAccountIsDeclaredAttemptingToFill, const std::string &slotFillerLockedUserAccountIsDeclaredAttemptingToFillSlotWith, const std::string &lockedUserAccountJson, u_int64_t casOfLockedUserAccount)
{
    getCouchbaseDocumentByKeyBegin(slotLockedUserAccountIsDeclaredAttemptingToFill);
    m_WhatTheGetWasFor = ONLOGINACCOUNTLOCKEDRECOVERY_AND_SLOTPURCHASEBEATTOTHEPUNCH_DOESSLOTEXISTCHECK;
    m_AccountLockedRecoveryWhatTheUserWasTryingToFillTheSlotWithHack = slotFillerLockedUserAccountIsDeclaredAttemptingToFillSlotWith;
    m_UserAccountLockedJsonToMaybeUseInAccountRecoveryAtLogin = lockedUserAccountJson;
    m_CasFromUserAccountLockedAndStuckLockedButErrRecordedDuringRecoveryProcessAfterLoginOrSomethingLoLWutIamHighButActuallyNotNeedMoneyToGetHighGuhLifeLoLSoErrLemmeTellYouAboutMyDay = casOfLockedUserAccount;
}
void AnonymousBitcoinComputingWtGUI::usernameOrPasswordYouProvidedIsIncorrect()
{
    m_LoginStatusMessagesPlaceholder->setText("The username or password you provided is incorrect"); //TODOreq: how do we tell them that it failed the second time? 'highlighting' animation makes sense, as does clearing the message when they start typing again
    m_LoginPasswordLineEdit->setText("");
}
void AnonymousBitcoinComputingWtGUI::doLoginTasks()
{
    //TODOreq(done except the registration widget one): logout -> login as different user will show old username in logout widget guh (same probablem with register successful widget)...
    if(m_LogoutWidget)
    {
        delete m_LogoutWidget;
    }
    m_LogoutWidget = new WContainerWidget(m_LoginLogoutStackWidget);
    m_LogoutWidget->setContentAlignment(Wt::AlignTop | Wt::AlignRight);
    new WText("Hello, " + m_CurrentlyLoggedInUsername + " ", m_LogoutWidget);
    WPushButton *logoutButton = new WPushButton("Log Out", m_LogoutWidget);
    logoutButton->clicked().connect(this, &AnonymousBitcoinComputingWtGUI::handleLogoutButtonClicked);
    new WBreak(m_LogoutWidget);
    new WText("Balance: BTC ", m_LogoutWidget);
    m_CurrentlyLoggedInUsersBalanceForDisplayOnlyLabel = new WText(m_CurrentlyLoggedInUsersBalanceStringForDisplayingOnly, m_LogoutWidget);
    new WBreak(m_LogoutWidget);
    m_LinkToAccount = new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ACCOUNT), ABC_ANCHOR_TEXTS_ACCOUNT, m_LogoutWidget);
    m_LoginLogoutStackWidget->setCurrentWidget(m_LogoutWidget);
    m_LoginStatusMessagesPlaceholder->setText("");

    m_LoggedIn = true;
    resumeRendering();
}
void AnonymousBitcoinComputingWtGUI::handleLogoutButtonClicked()
{
    if(!m_LoggedIn)
        return;

    if(m_AccountTabWidget)
    {
        delete m_AccountTabWidget;
        m_AccountTabWidget = 0;
        m_NewAdSlotFillerAccountTab = 0; //it gets deleted by object parent/child heirchy when m_AccountTabWidget is deleted, but we set it to zero ALSO because we can't defer rendering during upload, we check that this isn't now zero when the upload finished signal completes (m_LoggedIn doesn't cut it because they could have logged out and in again in the meantime). It might also fuck shit up if we stop showing m_NewAdSlotFillerAccountTab as the first tab in m_AccountTabWidget, but I am unsure TODOreq verify it's safe and/or fix
    }
    if(m_LinkToAccount) //check probably not necessary since it's created at login
    {
        delete m_LinkToAccount;
        m_LinkToAccount = 0;
    }
    if(m_BuyStep2placeholder)
    {
        delete m_BuyStep2placeholder;
        m_BuyStep2placeholder = 0;
        m_BuySlotFillerStep1Button->enable();
    }    
    m_LoggedIn = false;
    m_CurrentlyLoggedInUsername = "";
    m_LoginLogoutStackWidget->setCurrentWidget(m_LoginWidget);
}
void AnonymousBitcoinComputingWtGUI::newAndOpenAllWtMessageQueues()
{
    //m_StoreWtMessageQueue[0] = new message_queue(open_only, "BlahStore0");
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO, ABC_WT_NEW_AND_OPEN_MESSAGE_QUEUE_MACRO)
}
void AnonymousBitcoinComputingWtGUI::deleteAllWtMessageQueues()
{
    //the couchbase half does the message_queue::remove -- we just delete (implicitly 'closing') our pointers

    //delete m_StoreWtMessageQueue[0]; m_StoreWtMessageQueue[0] = NULL;
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO, ABC_WT_DELETE_MESSAGE_QUEUE_MACRO)
}

//message_queue *AnonymousBitcoinComputingWtGUI::m_StoreWtMessageQueues[ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_Store];
BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_MACRO, ABC_WT_STATIC_MESSAGE_QUEUE_SOURCE_DEFINITION_MACRO)

//event *AnonymousBitcoinComputingWtGUI::m_StoreEventCallbacksForWt[ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_Store];
BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_MACRO, ABC_WT_STATIC_EVENT_SOURCE_DEFINITION_MACRO)

//boost::mutex AnonymousBitcoinComputingWtGUI::m_StoreMutexArray[ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_Store];
BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_MACRO, ABC_WT_STATIC_MUTEX_SOURCE_DEFINITION_MACRO)
