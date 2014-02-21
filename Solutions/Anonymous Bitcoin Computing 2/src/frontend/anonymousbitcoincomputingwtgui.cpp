#include "anonymousbitcoincomputingwtgui.h"

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

//TODOreq: FOREVER: any feature that modifies user-account doc must do so with a CAS-swap, _AND_ the 'get' for that CAS-swap must ALWAYS check that "slotToAttemptToFillAkaPurchase" isn't set on the user-account. If "slotToAttemptToFillAkaPurchase" is set, that feature must not work until "slotToAttemptToFillAkaPurchase" is gone. It should error out saying "don't use multiple tabs" or "please try again in a few seconds".
//^just worth mentioning, but not worth fixing yet: when the buy slot aka slot fill fails just after using account locking (but before slot fill) and they log in and basically can't do shit with their account, it effectively makes the bitcoin stuff off limits too since it relies on the user account not being locked. As of right now they have to wait until the slot is purchased, but once I let them proceed/retry with the buy, then they can get themselves out of it (if they still want to do the purchase)
//TODOreq: "Forgot Your Password?" --> "Tough shit, I hope you learned your lesson"
//TODOreq: no-js get-from-subscription-cache-but-dont-subscribe
//TODOreq: find out when a signal connected to a dangerous slot can NOT be invoked by a user sifting through the html/js: signal disconnect should do it, but what about setVisible(false) and setEnabled(false), or changing a stackedwidget's current item (slot connected to button on stack item underneath). The easiest way to find this out is to ask; fuck trying to dig through Wt (and definitely fuck trying to dig through the html/js). Another related question: is there a race condition between resumeRendering at the beginning of a function and accessing a [editable-via-gui] member variable later in that same method (hacky solution to that is to always resumeRendering at the end of the method, and a hacky way to do that with lots of scope exit points in the function is using the stack struct destructor hack). I'm guessing the resumeRendering thing isn't a problem because the input events aren't (read:PROBABLY-AREN't(idk)) processed until the method ends and wt gets control again. hell, i don't even know for sure that deferRendering is even giving me the protection i'm assuming it is...
//TODOreq: litter if(!m_LoggedIn) lots of places. It's a cheap but effective safeguard
//TODOreq: to double or to int64? that is the question. boost::lexical_cast threw an exception when i tried to convert it to u_int64_t, maybe int64_t will have better luck. there's a "proper money handling" page that tells you to use int64.... but i'm trying to figure out what the fuck the point of that even is if i have to go "through" double anyways. maybe i should just string replace the decimal place for an empty string and then viola it is an int64 (still needs lexical casting for maths :-/). re: "through double" = double is usually more than enough on most platforms, but on some it isn't <- rationale for using int64. but if you're going string -> double -> int64 anyways, won't you have already lost precision? or maybe it's only when the math is being performed. I still am leaning towards int64 (if i can fucking convert my strings to it) because it does appear to be the right way to do rounding (deal with single Satoshis = no rounding needed)
//venting/OT: fucking bitcoin testnet-box-3 has tons of bitcoins, but they're all immature/orphaned so unspendable until i mine 120 fucking blocks. i'm averaging 1 block an hour -_-. box 2 didn't have this problem when i was fucking around with it years ago. i have the code written to test bitcoin interactions, but no test bitcoins available hahaha (i guess i can write the bulk bitcoins generator -> hugeBitcoinList/bitcoinKeySetN setter upper thingo first... zzz...

//TODOoptimization: lots of stuff needs to be moved into it's own object instead of just being a member in this class. it will reduce the memory-per-connection by...  maybe-a-significant... amount. for example the HackedInD3faultCampaign0 shit should be on it's own widget, but ON TOP OF THAT each "step" in the HackedInD3faultCampaign0 thing (buy step 1, buy step 2) can/should be it's own object (member of of HackedInD3faultCampaign0 object). We just have too many unused and unneeded-most-of-the-time member variables in this monster class... but KISS so ima continue for now, despite cringing at how ugly/hacky it's becoming :)

//TODOreq: can't remember if i wrote this anywhere or only thought of it, but i need to add underscores between certain things in my couchbase docs. for example a user named JimboKnives0 would have a conflict with a user named JimboKnives. adSpaceSlotFillersJimboKnives0 would now point to a specific ad slot filler, _AND_ the "profile view" (last 10 slot fillers set up) for JimboKnives0 -- hence, conflict. Could maybe solve this another way by perhaps always having the username be the very last part of the key???

//TODOoptimization: walking the internal path via "next sub path" (including sanitization at each step) is a good way to organize the website and additionally fill in json key pieces. example: /ads/buy-ad-space/d3fault/0 could be organized in Wt as: class Ads { class BuyAdSpace { m_SellerUsername = d3fault; m_SellerCampaign = 0; } }.... and organized in json as ads_buy_d3fault_0.  sanitization errors would be simple 404s, and it goes without saying that neither can/should include underscores or slashes
//^sanitize underscores(well,maybe.. if used as key 'split' identifier (isn't currently)), slashes(ditto if done for internal path 'walking'), and spaces (couchbase keys cannot contain spaces) from all user input AS A MINIMUM. really i'm probably going to go overkill and just allow a-z and 0-9 in a whitelist :-D. fuggit

AnonymousBitcoinComputingWtGUI::AnonymousBitcoinComputingWtGUI(const WEnvironment &myEnv)
    : WApplication(myEnv),
      m_HeaderHlayout(new WHBoxLayout()),
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
      m_AccountWidget(0),
      m_AuthenticationRequiredWidget(0),
      m_PendingBitcoinBalanceLabel(0),
      m_ConfirmedBitcoinBalanceLabel(0),
      m_ConfirmedBitcoinBalanceToBeCredittedWhenDoneButtonClicked(0.0),
      m_RegisterWidget(0),
      /*m_RegisterSuccessfulWidget(0),*/ m_AdvertisingBuyAdSpaceD3faultWidget(0),
      m_AdvertisingBuyAdSpaceD3faultCampaign0Widget(0),
      m_FirstPopulate(false),
      m_CurrentPriceLabel(0),
      m_BuySlotFillerStep1Button(0),
      m_HackedInD3faultCampaign0_NoPreviousSlotPurchases(true),
      m_AllSlotFillersComboBox(0),
      m_HackedInD3faultCampaign0_LastSlotPurchasesIsExpired(true),
      m_WhatTheStoreWithoutInputCasWasFor(INITIALINVALIDNULLSTOREWITHOUTINPUTCAS),
      m_WhatTheStoreWithInputCasWasFor(INITIALINVALIDNULLSTOREWITHCAS),
      m_WhatTheStoreWithInputCasSavingOutputCasWasFor(INITIALINVALIDNULLSTOREWITHCASSAVINGCAS),
      m_WhatTheGetWasFor(INITIALINVALIDNULLGET),
      m_WhatTheGetSavingCasWasFor(INITIALINVALIDNULLGETSAVINGCAS),
      m_CurrentlySubscribedTo(INITIALINVALIDNULLNOTSUBSCRIBEDTOANYTHING),
      m_LoggedIn(false),
      m_BitcoinAddressBalancePollerPollingPendingBalance(true),
      m_BitcoinAddressBalancePoller(0)
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
        getAndSubscribeCouchbaseDocumentByKeySavingCas("adSpaceSlotsd3fault0", GetCouchbaseDocumentByKeyRequest::GetAndSubscribeUnsubscribeMode); //TODOreq: see handleInternalPath's comment about making key dynamic (ez)
        //no need to reset m_CurrentlySubscribedTo to because 'this' is about to be deleted...
    }
    WApplication::finalize();
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

    m_HeaderHlayout->addWidget(new WText("<h2>Anonymous Bitcoin Computing</h2>"), 1, Wt::AlignTop | Wt::AlignCenter);
    m_HeaderHlayout->addWidget(m_LoginLogoutStackWidget, 0, Wt::AlignTop | Wt::AlignRight);

    m_MainVLayout->addLayout(m_HeaderHlayout, 0, Wt::AlignTop | Wt::AlignRight);

    //LINKS -- TODOoptional: disable clickability of current location
    m_LinksVLayout->addWidget(new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_HOME), ABC_ANCHOR_TEXTS_PATH_HOME));
    m_LinksVLayout->addWidget(new WBreak());
    m_LinksVLayout->addWidget(new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS), ABC_ANCHOR_TEXTS_PATH_ADS));
    m_LinksVLayout->addWidget(new WBreak());
    m_LinksVLayout->addWidget(new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE), "-" ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE));
    m_LinksVLayout->addWidget(new WBreak());
    m_LinksVLayout->addWidget(new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE_D3FAULT), ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE_D3FAULT));
    m_LinksVLayout->addWidget(new WBreak());
    m_LinksVLayout->addWidget(new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE_D3FAULT_CAMPAIGN_0), ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE_D3FAULT_CAMPAIGN_0));
    m_LinksVLayout->addWidget(new WBreak());
    m_LinksVLayout->addWidget(new WBreak()); //second wbreak to give one line gap between regular links and user account link (which isn't created until login

    m_BodyHLayout->addLayout(m_LinksVLayout, 0, Wt::AlignTop | Wt::AlignLeft);
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
        if(m_AccountWidget)
        {
            delete m_AccountWidget;
            m_AccountWidget = 0;
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
    if(!m_AccountWidget)
    {        
        m_AccountWidget = new WContainerWidget(m_MainStack);
        WVBoxLayout *accountVLayout = new WVBoxLayout(m_AccountWidget);

        accountVLayout->addWidget(new WText("Step 1) Fund Your Account via Bitcoin"));
        accountVLayout->addWidget(new WBreak());
        WPushButton *addFundsPushButton = new WPushButton("Add Funds");
        addFundsPushButton->clicked().connect(this, &AnonymousBitcoinComputingWtGUI::handleAddFundsClicked); //TODOreq: perhaps disable after clicking? re-enable somewhere(s) too
        accountVLayout->addWidget(addFundsPushButton);
        accountVLayout->addWidget(new WBreak());
        accountVLayout->addWidget(new WBreak());
        m_AddFundsPlaceholderLayout = new WVBoxLayout();
        accountVLayout->addLayout(m_AddFundsPlaceholderLayout);
        accountVLayout->addWidget(new WBreak());
        accountVLayout->addWidget(new WBreak());
        accountVLayout->addWidget(new WBreak());
        accountVLayout->addWidget(new WBreak());


        accountVLayout->addWidget(new WText("Step 2) Upload Advertisements (For use in buying ad space)")); //TODOoptimization: perhaps a captcha must be solved before every upload, thwarting it's potential as a DDOS point (but it would take a specific app to take advantage of it (but Wt's element id randomization protects me zilch against screen based automation clickers (i could hack me)))
        accountVLayout->addWidget(new WBreak());
        WGridLayout *uploadNewSlotFillerGridLayout = new WGridLayout();

        int rowIndex = -1;

        uploadNewSlotFillerGridLayout->addWidget(new WText("--- 1) Nickname (unseen by others):"), ++rowIndex, 0);
        m_UploadNewSlotFiller_NICKNAME = new WLineEdit();
        uploadNewSlotFillerGridLayout->addWidget(m_UploadNewSlotFiller_NICKNAME, rowIndex, 1);

        uploadNewSlotFillerGridLayout->addWidget(new WText("--- 2) Mouse Hover Text:"), ++rowIndex, 0);
        m_UploadNewSlotFiller_HOVERTEXT = new WLineEdit();
        uploadNewSlotFillerGridLayout->addWidget(m_UploadNewSlotFiller_HOVERTEXT, rowIndex, 1);

        uploadNewSlotFillerGridLayout->addWidget(new WText("--- 3) URL:"), ++rowIndex, 0);
        m_UploadNewSlotFiller_URL = new WLineEdit();
        uploadNewSlotFillerGridLayout->addWidget(m_UploadNewSlotFiller_URL, rowIndex, 1);

        //TODOreq: sanitize above line edits

        uploadNewSlotFillerGridLayout->addWidget(new WText("--- 4) Ad Image:"), ++rowIndex, 0);
        m_AdImageUploader = new WFileUpload();
        //TODOreq: i can probably specify the temporary location of the uploaded file, and obviously i would want to use a tmpfs. would be even better if i could just upload into memory...
        m_AdImageUploader->setFileTextSize(40); //TODOreq: wtf is this, filename size or file size? i'll probably disregard filename when putting into b64/json... so if it's filename set it to like 256 or some sane max idfk, but worth noting i use the filename in a WFileResource to serve them the FIRST preview copy of the image they just uploaded
        m_AdImageUploader->uploaded().connect(this, &AnonymousBitcoinComputingWtGUI::handleAdSlotFillerSubmitButtonClickedAkaImageUploadFinished); //TODOreq: doc says i should delete the WFileUpload after an upload
        m_AdImageUploader->fileTooLarge().connect(this, &AnonymousBitcoinComputingWtGUI::handleAdImageUploadFailedFileTooLarge);
        uploadNewSlotFillerGridLayout->addWidget(m_AdImageUploader, rowIndex, 1);

        uploadNewSlotFillerGridLayout->addWidget(new WText("==Image Maximums==  FileSize: 2 mb --- Width: " + boost::lexical_cast<std::string>(ABC_MAX_AD_SLOT_FILLER_IMAGE_WIDTH_PIXELS) + " px --- Height: " + boost::lexical_cast<std::string>(ABC_MAX_AD_SLOT_FILLER_IMAGE_HEIGHT_PIXELS) + " px"), ++rowIndex, 0, 1, 2);
        uploadNewSlotFillerGridLayout->addWidget(new WText("Accepted Image Formats: png/jpeg/gif/bmp/svg"), ++rowIndex, 0, 1, 2); //TODOreq: do i really want to allow animated gifs?!?!?!? always thought no, but idk never really thought about it lol

        WPushButton *adImageUploadButton = new WPushButton("Submit");
        adImageUploadButton->clicked().connect(m_AdImageUploader, &WFileUpload::upload);
        adImageUploadButton->clicked().connect(adImageUploadButton, &WPushButton::disable); //TODOreq: maybe catch the clicked signal so we can deferRendering, and THEN call upload manually?
        uploadNewSlotFillerGridLayout->addWidget(adImageUploadButton, ++rowIndex, 1);
        //TODOoptional: progress bar would be nice (and not too hard), but eh these images aren't going to take long to upload so fuck it

        uploadNewSlotFillerGridLayout->addWidget(new WText("WARNING: Ads can't be deleted or modified after uploading"), ++rowIndex, 0, 1, 2);

        accountVLayout->addLayout(uploadNewSlotFillerGridLayout);

        //TODOreq: a place for them to view their uploaded ads (so they can verify their images uploaded ok and get a preview of how it will look (we might shrink it, for example (TODOreq: determine width/height, tell them what it is on this upload page) stuff)

        m_AdImageUploadResultsVLayout = new WVBoxLayout(); //TODOreq: maybe make this a VBoxLayout so it doesn't go behind the fields like in the pic?
        m_AdImageUploadResultsVLayout->addWidget(new WBreak());
        m_AdImageUploadResultsVLayout->addWidget(new WBreak());
        accountVLayout->addLayout(m_AdImageUploadResultsVLayout);


        //TODOreq: after the upload, set them up to do another upload (and perhaps show the previous upload right here/now)
    }
    m_MainStack->setCurrentWidget(m_AccountWidget);
}
void AnonymousBitcoinComputingWtGUI::handleAddFundsClicked()
{
    deferRendering();

    //even though we are already logged in, we still need to get the user-account doc because if we kept it in memory instead it might be stale or some shit. could be wrong here idfk, but fuck it

    getCouchbaseDocumentByKeySavingCasBegin("user" + m_CurrentlyLoggedInUsername); //ACTUALLY: changing back to get with cas because if we see the state is in "GettingKey" mode, we do light recovery from there and in fact need the cas for that. NOPE: was tempted to have this be a 'get with cas', but the user might wait a while and possibly do other things (slot fills namely) before doing any further actions, so the CAS may be hella stale by then.
    m_WhatTheGetSavingCasWasFor = ADDFUNDSBUTTONCLICKEDSODETERMINEBITCOINSTATEBUTDONTLETTHEMPROCEEDIFLOCKEDATTEMPTINGTOFILLAKAPURCHASESLOT;
}
void AnonymousBitcoinComputingWtGUI::handleAdSlotFillerSubmitButtonClickedAkaImageUploadFinished()
{
    deferRendering();

    //TO DOnereqNOPE-ish (outdated, see below): maybe optional, idfk, but basically i want this store to just eh 'keep incrementing slot filler index until the LCB_ADD succeeds'. I'm just unsure whether or not the front-end or backend should be driving that. I think I have a choice here... and idk which is betterererer. Having the backend do it would probably be more efficient, but eh I think the front-end has to here/now/before-the-STORE-one-line-below determine the slot filler index to start trying to LCB_ADD at (else we'd be incredibly inefficient if we started at 0 and crawled up every damn time)... SO SINCE the front-end is already getting involved with it, maybe he should be the one directing the incrementing? It does seem "user specific" (user being frontend -> using backend), _BUT_ it also could be very easily genericized and used in tons of other apps... so long as we either use some "%N%" (no) in the key or make the number the very last part of the key (yes). I might even use this same thing with the bitcoin keys, but eh I haven't figured those out yet and don't want to yet (one problem at a time).

    //^^^^decided to just KISS and do it here on the front-end, knowing full well it'd be stupidly re-sending the image over and over on retries, FUCKIT. I'm going to be doing 'inline recovery' and the design for that is able to get complex, but by doing it here instead of on backend I am able to not eat my shotgun.
    //^^^^^tl;dr: get allAdSlotFillers doc, choose the n+1 index, LCB_ADD to it, if lcbOpFail: cas-swap-accepting-fail updating allAdSlotFillers (inline recovery), do n+2 (really n+1 relative to the newest update) LCB_ADD, if fail: <repeat-this-segment-X-times> -> when the LCB_ADD doesn't fail, cas-swap-accepting-fail updating allAdSlotFillers, DONE. the very last cas-swap-accepting-fail can fail (as in, not happen!) and on the NEXT ad upload the state will be recovered (might be strange from end user's perspective, but fuck it)

    //get adSpaceAllSlotFillers<username>
    getCouchbaseDocumentByKeySavingCasBegin("adSpaceAllSlotFillers" + m_CurrentlyLoggedInUsername);
    m_WhatTheGetSavingCasWasFor = ALLADSLOTFILLERSTODETERMINENEXTINDEXANDTOUPDATEITAFTERADDINGAFILLERGETSAVINGCAS;

    //fml: memory(httpd-wt) -> file(chillen) -> memory(my-wt) ->b64encode-> memory(my-wt) ->messageQueue-> memory(sendQueue) -> memory(receiveQueue) ->->messageQueue-> memory(my-couchbase) -> memory(couchbase-send-buffer)
    //8 copies woo how efficient. maybe i should have just passed the backend the filename :-/...
    //comparison(hypothetical): memory(httpd-wt) -> file(chillen), <send-filename-to-backend>, -> memory(my-couchbase) ->b64encode-> memory(my-couchbase) -> memory(couchbase-send-buffer)
    //^^forgot the json, so 9? 10? fuckit just want it to work...
    //4, hmmmm......... BUT I PUT SO MUCH EFFORT INTO THAT MACRO REFACTOR ;-p (fail argument is fail, but very common among humans). still, doing it here has the advantage of being on a threadpool thread (backend is just one thread), so it keeps the backend free from doing a disk read and base64encode. there's also the weak argument that my base64encode is a Wt function (that boost b64 shit (in commit history) failed like fuck (b64? seriously boost? 2014?)).. but bleh i could always hack a wrapper to Wt's into the frontend2backend message class. I genuinely don't know which is more efficient, but doing it here on the front-end seems to make more DESIGN/sanity sense (despite probably being less efficient (definitely less efficient with copies, but idk about whether or not it's less efficient overall (and such true/false evaluation might be dependent on filesize!))
}
void AnonymousBitcoinComputingWtGUI::handleAdImageUploadFailedFileTooLarge()
{
    m_AdImageUploadResultsVLayout->addWidget(new WText("Your ad's image exceeded the maximum file size of 2 megabytes"));
}
void AnonymousBitcoinComputingWtGUI::showRegisterWidget()
{
    if(!m_RegisterWidget)
    {
        m_RegisterWidget = new WContainerWidget(m_MainStack);
        WGridLayout *registerGridLayout = new WGridLayout(m_RegisterWidget);
        int rowIndex = -1;

        registerGridLayout->addWidget(new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_HOME), ABC_ANCHOR_TEXTS_PATH_HOME), ++rowIndex, 0, 1, 2);

        registerGridLayout->addWidget(new WText("Username:"), ++rowIndex, 0);
        m_RegisterUsernameLineEdit = new WLineEdit();
        registerGridLayout->addWidget(m_RegisterUsernameLineEdit, rowIndex, 1);
        m_RegisterUsernameLineEdit->enterPressed().connect(this, &AnonymousBitcoinComputingWtGUI::handleRegisterButtonClicked); //was tempted to not put this here because if they press enter in username then they probably aren't done, BUT that 'implicit form submission' bullshit would submit it anyways. might as well make sure it's pointing at the right form...

        registerGridLayout->addWidget(new WText("Password:"), ++rowIndex, 0);
        m_RegisterPasswordLineEdit = new WLineEdit();
        registerGridLayout->addWidget(m_RegisterPasswordLineEdit, rowIndex, 1);
        m_RegisterPasswordLineEdit->setEchoMode(WLineEdit::Password);
        m_RegisterPasswordLineEdit->enterPressed().connect(this, &AnonymousBitcoinComputingWtGUI::handleRegisterButtonClicked);

        ++rowIndex;

        WPushButton *registerButton = new WPushButton("Register");
        registerGridLayout->addWidget(registerButton, ++rowIndex, 1);
        registerButton->clicked().connect(this, &AnonymousBitcoinComputingWtGUI::handleRegisterButtonClicked);

        registerGridLayout->addWidget(new WText("WARNING: DO NOT LOSE/FORGET YOUR PASSWORD! THERE IS NO PASSWORD RESET FEATURE!!!"), ++rowIndex, 0, 1, 2);

        ++rowIndex;

        registerGridLayout->addWidget(new WText("Optional:"), ++rowIndex, 0);
        registerGridLayout->addWidget(new WText("Sexual Preference:"), ++rowIndex, 0);
        registerGridLayout->addWidget(new WLineEdit(), rowIndex, 1);
        registerGridLayout->addWidget(new WText("Social Security Number:"), ++rowIndex, 0);
        registerGridLayout->addWidget(new WLineEdit(), rowIndex, 1);
        registerGridLayout->addWidget(new WText("Religion:"), ++rowIndex, 0);
        registerGridLayout->addWidget(new WLineEdit(), rowIndex, 1);
        registerGridLayout->addWidget(new WText("Political Beliefs:"), ++rowIndex, 0);
        registerGridLayout->addWidget(new WLineEdit(), rowIndex, 1);
        registerGridLayout->addWidget(new WText("Time of day your wife is home alone:"), ++rowIndex, 0);
        registerGridLayout->addWidget(new WLineEdit(), rowIndex, 1);
        registerGridLayout->addWidget(new WText("Where you keep your gun:"), ++rowIndex, 0);
        registerGridLayout->addWidget(new WLineEdit(), rowIndex, 1);
        registerGridLayout->addWidget(new WText("Your wife's cycle:"), ++rowIndex, 0);
        registerGridLayout->addWidget(new WLineEdit(), rowIndex, 1);
        registerGridLayout->addWidget(new WText("Your mum's cycle:"), ++rowIndex, 0);
        registerGridLayout->addWidget(new WLineEdit(), rowIndex, 1);
        registerGridLayout->addWidget(new WText("Credit Card #:"), ++rowIndex, 0);
        registerGridLayout->addWidget(new WLineEdit(), rowIndex, 1);
        registerGridLayout->addWidget(new WText("Credit Card Pin #:"), ++rowIndex, 0);
        registerGridLayout->addWidget(new WLineEdit(), rowIndex, 1);

        //;-) and then secretly ;-) on the deployed/binary version ;-) i actually save these values ;-) and then sell them to hollywood ;-) to make movies off of them ;-) and then use the funds from that to take over the world ;-) and solve us of our corporate cancers (of which hollywood is an item) ;-) muaahahahahhahaha

        WPushButton *registerButton2 = new WPushButton("Register");
        registerGridLayout->addWidget(registerButton2, ++rowIndex, 1);
        registerButton2->clicked().connect(this, &AnonymousBitcoinComputingWtGUI::handleRegisterButtonClicked);

        registerGridLayout->addWidget(new WText("WARNING: DO NOT LOSE/FORGET YOUR PASSWORD! THERE IS NO PASSWORD RESET FEATURE!!!"), ++rowIndex, 0, 1, 2);
    }
    m_MainStack->setCurrentWidget(m_RegisterWidget);
}
void AnonymousBitcoinComputingWtGUI::registerAttemptFinished(bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        //TODOreq: 500 internal server error?
        return;
    }
    if(!lcbOpSuccess)
    {
        new WText("That username is already taken, please try another", m_RegisterWidget); //TODOreq: put this in a better spot. below all our joke fields probably wouldn't even be seen (we could put it just by the username field and make it disappear once they start typing again (or hit submit again (depending))
        return;
    }

    //if we get here, the registration doc add was successful

    WContainerWidget *registerSuccessfulWidget = new WContainerWidget(m_MainStack);
    new WText("Welcome to Anonymous Bitcoin Computing, " + m_RegisterUsernameLineEdit->text() + ". You can now log in.", registerSuccessfulWidget);

    new WBreak(registerSuccessfulWidget);
    new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_HOME), ABC_ANCHOR_TEXTS_PATH_HOME, registerSuccessfulWidget);

    //in case they browse back to it. TODOreq: probably should do this in other places as well
    m_RegisterUsernameLineEdit->setText("");
    m_RegisterPasswordLineEdit->setText("");

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
void AnonymousBitcoinComputingWtGUI::showAdvertisingBuyAdSpaceD3faultWidget()
{
    if(!m_AdvertisingBuyAdSpaceD3faultWidget)
    {
        m_AdvertisingBuyAdSpaceD3faultWidget = new WContainerWidget(m_MainStack);
        new WText("d3fault's ad campaigns:", m_AdvertisingBuyAdSpaceD3faultWidget);
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
        new WText(ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE_D3FAULT_CAMPAIGN_0, m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);

        //still dunno if i should block or do it async? instincts tell me async, but "SUB MILLISECOND LATENCY" tells me async might actually be wasteful/slower??? The obvious answer is "benchmark it xD" (FUCK FUCK FUCK FUCK FUCK THAT, async it is (because even if slower, it still allows us to scale bettarer))
        //^To clarify, I have 3 options: wait on a wait condition right here that is notified by couchbase thread and we serve up result viola, deferRendering/resumeRendering, or enableUpdates/TriggerUpdates. The last one requires ajax. Both the last 2 are async (despite defer "blocking" (read:disabling) the GUI)

        new WText("Price in BTC: ", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        m_CurrentPriceLabel = new WText(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        m_CurrentPriceDomPath = m_CurrentPriceLabel->jsRef();

        if(!environment().ajax())
        {
            //TODOreq: get, don't subscribe (but still use subsciption cache!)

            getCouchbaseDocumentByKeySavingCasBegin("adSpaceSlotsd3fault0");
            m_WhatTheGetSavingCasWasFor = HACKEDIND3FAULTCAMPAIGN0GET; //TODOreq: why the fuck do i save the cas? surely i should re-fetch the doc- OH BUT IT NEVER CHANGES UNLESS THERE'S A BUY EVENT!!!
        }
        else
        {
            enableUpdates(true); //TODOreq: putting here doesn't future proof us for other get-and-subscribe, but no need to put it in constructor because most other pages don't use it
            //TODOoptional: maybe enableUpdates(false) if/when they navigate away, which would mean i set back to true in the 're-subscribe' below (for when this widget is already created)

            m_FirstPopulate = true; //so we know to call resumeRendering on first populate/update, but not populates/updates thereafter
            deferRendering();

            getAndSubscribeCouchbaseDocumentByKeySavingCas("adSpaceSlotsd3fault0", GetCouchbaseDocumentByKeyRequest::GetAndSubscribeMode);
            m_CurrentlySubscribedTo = HACKEDIND3FAULTCAMPAIGN0GETANDSUBSCRIBESAVINGCAS;

            //TODOoptimization: shorten js variable names (LOL JS). use defines to retain sanity

            //placeholder/initialization javascript, not populated or active yet            
            m_CurrentPriceLabel->doJavaScript
            (
                m_CurrentPriceDomPath + ".z0bj = new Object();" +
                m_CurrentPriceDomPath + ".z0bj.minPrice = 0;" +
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
    else //widget already created, so just re-subscribe
    {
        //TODOreq: need to handle no-js mode here as well
        getAndSubscribeCouchbaseDocumentByKeySavingCas("adSpaceSlotsd3fault0", GetCouchbaseDocumentByKeyRequest::GetAndSubscribeMode);
        m_CurrentlySubscribedTo = HACKEDIND3FAULTCAMPAIGN0GETANDSUBSCRIBESAVINGCAS;
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
void AnonymousBitcoinComputingWtGUI::finishShowingAdvertisingBuyAdSpaceD3faultCampaign0Widget(const string &couchbaseDocument, u_int64_t casForSafelyUpdatingCampaignDocAfterSuccesfulPurchase)
{
    //This is ALSO (but not always (first get)) what I have referred to as a "buy event", so if they have clicked "buy step 1" we need to roll back the GUI so they have to click buy step 1 again (various GUI object organizational changes to support this)
    //Goes without saying that the new 'slot index' that they will try to buy (which is locked in after clicking buy step 1) should be set in this method
    //I'm thinking it might be easiest to do all the "new" ing in the beginShowing() method and then to merely modify/populate those objects/variables via setText in this one
    //TODOoptional: unsubscribing + resubscribing would make the resubscriber a NewSubscriber, and he'd get an update even if it's the same value (this probably won't matter (and isn't even worth coding for))

    m_HackedInD3faultCampaign0JsonDocForUpdatingLaterAfterSuccessfulPurchase = couchbaseDocument;
    m_HackedInD3faultCampaign0CasForSafelyUpdatingCampaignDocLaterAfterSuccessfulPurchase = casForSafelyUpdatingCampaignDocAfterSuccesfulPurchase;

    //TODOreq: this is the javascript impl of the countdown shit. we obviously need to still verify that the math is correct when a buy attempt happens (we'll be using C++ doubles as well, so will be more precise). we should detect when a value lower than 'current' is attempted, and then laugh at their silly hack attempt. i should make the software laugh at me to test that i have coded it properly (a temporary "submit at price [x]" line edit just for testing), but then leave it (the laughing when verification fails, NOT the line edit for testing) in for fun.
    //TODOreq: if we do the new-ing in 'begin' a populating in 'finish' (here), we need to deferRendering to make things such as 'buy step 1' not clickable until the first 'finish' (which is more appropriately 'update'/populate now)... but we shouldn't do resumeRendering every time, because buy events don't need to do it. I guess we could force it to be invisible until 'finish', but I'm not certain that makes it unclickable (so a hacker could segfault me still). I'm 50/50 on this, I think Wt might make invisible objects 'unclickable' in that sense. setDisabled should do the same, but again unsure if that makes the inner js -> c++ slot mechanism not usable/hackable(hackable-because-segfault)

    ptree pt;
    std::istringstream is(couchbaseDocument);
    read_json(is, pt);

    m_HackedInD3faultCampaign0_MinPrice = pt.get<std::string>("minPrice");
    m_HackedInD3faultCampaign0_SlotLengthHours = pt.get<std::string>("slotLengthHours");
    boost::optional<ptree&> lastSlotFilledAkaPurchased = pt.get_child_optional("lastSlotFilledAkaPurchased");
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


    if(environment().ajax())
    {        
        if(m_HackedInD3faultCampaign0_NoPreviousSlotPurchases)
        {
            //TODOreq (done i think, but test this): no purchases yet, use static min price (but still be able to transform to javascript countdown on buy event)
            m_CurrentPriceLabel->setText(m_HackedInD3faultCampaign0_MinPrice);
        }
        else
        {
            //these pt.gets() are a copy-paste job from environment()._NO_ajax() code path
            m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedSlotIndex = lastSlotFilledAkaPurchased.get().get<std::string>("slotIndex");
            m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedPurchaseTimestamp = lastSlotFilledAkaPurchased.get().get<std::string>("purchaseTimestamp");
            m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedStartTimestamp = lastSlotFilledAkaPurchased.get().get<std::string>("startTimestamp");
            m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedPurchasePrice = lastSlotFilledAkaPurchased.get().get<std::string>("purchasePrice");

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

        //TODOoptional(was req, but fuck it): navigating away should turn off the timer, coming back turns it back on
        if(m_FirstPopulate)
        {
            m_FirstPopulate = false;
            resumeRendering();
            m_CurrentPriceLabel->doJavaScript
            (
                m_CurrentPriceDomPath + ".z0bj.minPrice = " + m_HackedInD3faultCampaign0_MinPrice + ";" +
                ABC_START_JS_INTERVAL_SNIPPET
                //TODOreq: does re-subscribe re-enable the timer? i would think the timer would still be in whatever state it was in when we left/unsubscribed, SO THAT MEANS: if we unsubscribe -> the timer 'stops' (minprice) _AND_ there is a buy event before we re-subscribe = the timer is still sitting at stopped/min-price and does not get re-enabled (despite us having accurate 'values' for calculating).
                //^it appears that the timer only gets turned on at first populate. a buy event after the timer stops (min-price) probably won't enable it either, which is related to the directly above req
            );
        }
        triggerUpdate();
    }
    else
    {
        if(m_HackedInD3faultCampaign0_NoPreviousSlotPurchases) //TODOreq: not related to js/here/etc, but when a "buy event" is received and we're on, say, step 1 of 2 clicked, we would then possibly toggle the value of NoPreviousSlotPurchases and need to make sure that... err... something...
        {
            m_CurrentPriceLabel->setText(m_HackedInD3faultCampaign0_MinPrice);
        }
        else
        {
            //these pt.gets() are a copy-paste job from environment().ajax() code path
            m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedSlotIndex = lastSlotFilledAkaPurchased.get().get<std::string>("slotIndex");
            m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedPurchaseTimestamp = lastSlotFilledAkaPurchased.get().get<std::string>("purchaseTimestamp");
            m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedStartTimestamp = lastSlotFilledAkaPurchased.get().get<std::string>("startTimestamp");
            m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedPurchasePrice = lastSlotFilledAkaPurchased.get().get<std::string>("purchasePrice");


            //check expired
            double lastSlotFilledAkaPurchasedExpireDateTime = (boost::lexical_cast<double>(m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedStartTimestamp)+((double)(boost::lexical_cast<double>(m_HackedInD3faultCampaign0_SlotLengthHours)*(3600.0))));
            double currentDateTime = static_cast<double>(WDateTime::currentDateTime().toTime_t());
            if(currentDateTime >= lastSlotFilledAkaPurchasedExpireDateTime)
            {
                //expired, so use min price
                m_CurrentPriceToUseForBuying = boost::lexical_cast<double>(m_HackedInD3faultCampaign0_MinPrice);
            }
            else
            {
                //not expired, so calculate. despite determining it's not expired here, we are not going to set "m_HackedInD3faultCampaign0_LastSlotPurchasesIsExpired = false;", because it may have expired by the time they hit buy step 2. We have to check when/after they click buy step 2. Could be minutes/hours/days after the page has been rendered (this code)

                //TODOreq: javascript-less UI should update price after buy step 1 is clicked
                //TODOreq: maybe a 60 second timer to refresh js-less page, or a refresh button

                double currentPrice = calculateCurrentPrice(currentDateTime, boost::lexical_cast<double>(m_HackedInD3faultCampaign0_MinPrice), (boost::lexical_cast<double>(m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedPurchasePrice)*2.0), (boost::lexical_cast<double>(m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedStartTimestamp)+((double)(boost::lexical_cast<double>(m_HackedInD3faultCampaign0_SlotLengthHours)*(3600.0)))), boost::lexical_cast<double>(m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedPurchaseTimestamp));

                m_CurrentPriceLabel->setText(boost::lexical_cast<std::string>(currentPrice)); //TODOreq: 8 decimal places, since it's presented to user
                //TODOoptional: if viewing campaign/countdown with js on and you then disallow js, the price is empty/blank (not even zero). weirdly though, coming directly to the page (no session) with javascript already disabled works fine. I think it has something to do with noscript not re-fetching the source during the "reload"... because on the noscript-reload tab i see "Scripts currently forbidden" (with options to allow them), and on "navigated directly to campaign with js already off" tab I don't see that message (no js was served). I don't even think I CAN fix this problem heh :-P... but it might be a Wt bug... not gracefully degrading when session becomes js-less?

                //TODOreq: "current price is XXX and it will be back at YYYYY at ZZZZZZZ (should nobody buy any further slots). Click here to refresh this information to see if it's still valid, as someone may have purchased a slot since you loaded this page (enable javascript if you want to see it count down automatically)"
            }
        }
    }
    if(!m_BuySlotFillerStep1Button)
    {
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        m_BuySlotFillerStep1Button = new WPushButton("Buy At This Price (Step 1 of 2)", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        m_BuySlotFillerStep1Button->clicked().connect(this, &AnonymousBitcoinComputingWtGUI::buySlotStep1d3faultCampaign0ButtonClicked);
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
        return;
    }

    getCouchbaseDocumentByKeyBegin("adSpaceAllSlotFillers" + m_CurrentlyLoggedInUsername); //TODOreq: obviously we'd have sanitized the username by here...
    m_WhatTheGetWasFor = HACKEDIND3FAULTCAMPAIGN0BUYSTEP1GET;
    deferRendering();
}
void AnonymousBitcoinComputingWtGUI::buySlotPopulateStep2d3faultCampaign0(const std::string &allSlotFillersJsonDoc, bool lcbOpSuccess, bool dbError)
{
    resumeRendering();
    if(dbError)
    {
        //TODOreq: 500 internal server error
        return;
    }
    if(!lcbOpSuccess) //allAds doc doesn't exist
    {
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WText("You need to set up some advertisements before you can buy ad space: ", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ACCOUNT), ABC_ANCHOR_TEXTS_ACCOUNT, m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        //TODOreq: on logout, we should roll back everything back to pre-step1-clicked (my deleting of account link made me think of that, but it applies to way more things)
        return;
    }

    //if we get here, allAds doc exists (which means it has at least one ad)

    if(!m_AllSlotFillersComboBox)
    {
        ptree pt;
        std::istringstream is(allSlotFillersJsonDoc);
        read_json(is, pt);

        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WText("Select which advertisement you want to use (you cannot change this later):", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);

        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        m_AllSlotFillersComboBox = new WComboBox(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        int adsCount = boost::lexical_cast<int>(pt.get<std::string>("adsCount"));
        for(int i = 0; i < adsCount; ++i)
        {
            //json looks like this: "0" : "<nickname>"
            m_AllSlotFillersComboBox->insertItem(i, pt.get<std::string>(boost::lexical_cast<std::string>(i)));
            //TODOreq: Wt probably/should already do[es] this, but the nicknames definitely need to be sanitized (when they're created)
        }

        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WText("ARE YOU SURE? THERE'S NO TURNING BACK AFTER THIS", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        WPushButton *buySlotFillerStep2Button = new WPushButton("Buy At This Price (Step 2 of 2)", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget); //TODOoptional: this button could have the js-price-falling thing on it, so they look right at the price when they click it (and buy event 'right before' they click buy step 1 is more likely to be seen/noticed!)
        buySlotFillerStep2Button->clicked().connect(this, &AnonymousBitcoinComputingWtGUI::buySlotStep2d3faultCampaign0ButtonClicked);
    }
}
void AnonymousBitcoinComputingWtGUI::buySlotStep2d3faultCampaign0ButtonClicked()
{
    deferRendering(); //haha it hit me while laying in bed that you can call this multiple times. never thought i'd use that feature but blamo here i am using it and loving Wt :). TODOreq: make sure the 'second' deferRendering is called in all error cases from here
    //TO DOnereqopt: idk if required or optional or optimization, but it makes sense that we consolidate the defer/resume renderings here. The result of that button clicked (the signal that brought us here) makes us do TWO couchbase round trips: the first one to verify balance and lock account, the second to do the actual slot-filling/buying. It makes sense to only defer rendering once HERE, and to resume rendering only once LATER (error, or slot filled, etc). It might not matter at all.

    //DO ACTUAL BUY (check balance + lock user account, then associate slot with slot filler, aka fill the slot)

   //TODOreq: this doesn't belong here, but the combo box probably depends on the user to send in the slotfiller index, so we need to sanitize that input (also applies to nickname if it is used (i don't currently plan to))

    new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
    new WText("Attempting to buy slot with filler: " + m_AllSlotFillersComboBox->currentText(), m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);

    //TODOreq: change to 'sending them the time' [for use in js], because it not only solves the timezones/incorrect-system-time problem, but it also has the added benefit of GUARANTEEING that the price they see will be above the internal price (because of latency)... so much so that we don't need them to send it in anymore. the slot index would be enough. Also worth noting that the js should use an accurate timer to calculate the 'current time' (from the time we sent them and they saved). I doubt setInterval is a high precision timer. I was originally thinking they could delta the time we sent them against their system time, but then there'd be the problem of if they changed their system time while running then it'd fuck shit up (not a big deal though since it's a rare case. still if js has a high precision timer object ("time elapsed since x"), use that).
    //^that guarantee depends on all wt nodes being time sync'd like fuck, but that goes without saying

    //TODOreq: handle cases where no puchases yet (use min), no next, no balls, current expired (use min),etc

    //had calculate current price here, but moved it to a few sub-milliseconds later

    //TODOreq: sanitize slot index (user could have forged), verify it in fact exists (was thinking i should try to pull the doc, BUT we already have the "allSlotFillers" doc and can just verify that it's in that instead (so we would be depending on earlier sanity checks when setting up the slotFiller instead)

    m_SlotFillerToUseInBuy = "adSpaceSlotFillers" + m_CurrentlyLoggedInUsername + boost::lexical_cast<std::string>(m_AllSlotFillersComboBox->currentIndex());
    //TODOreq: making username 'last' part of key doesn't solve the problem: adSpaceSlotFillers12joe (slot filler 1/username '2joe' conflicts with slot filler 12/username 'joe'). It would solve it if we used leading zeros, but fuck that underscores is better solution

    getCouchbaseDocumentByKeySavingCasBegin("user" + m_CurrentlyLoggedInUsername);
    m_WhatTheGetSavingCasWasFor = HACKEDIND3FAULTCAMPAIGN0BUYSTEP2aVERIFYBALANCEANDGETCASFORSWAPLOCKGET;

    //TO DOnereq (vague so pretty much done (spread out to many other reqs)): the above get shouldn't fail since they need to already be logged in to get this far, but what the fuck do i know? it probably CAN (db overload etc), so i need to account for that
    //^everything can DEFINITELY fail, it is only success that can ever at most PROBABLY happen

    //TODOreq: failed to lock your account for the purchase (???), failed to buy/fill the slot (insufficient funds, or someone beat us to it (unlock in both cases))
}
void AnonymousBitcoinComputingWtGUI::checkNotAttemptingToFillAkaPurchaseSlotThenTransitionIntoGettingBitcoinKeyState(const string &userAccountDoc, u_int64_t cas, bool lcbOpSuccess, bool dbError)
{
    //TODOreq: we check that we are in fact in "NoKey" state before we transition out of it because if they have multiple tabs open or something they could have already requested one and done the transition (in which case, we'd be transitioning into ourselves (and getting another key?). TODOreq: all such transitions check the transition FROM state is what is expected

    if(dbError)
    {
        //TODOreq: handle and notify

        //temp:
        cerr << "checkNotAttemptingToFillAkaPurchaseSlotThenTransitionIntoGettingBitcoinKeyState db error" << endl;

        resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //TODOreq: handle and notify

        //temp:
        cerr << "TOTAL SYSTEM FAILURE USER ACCOUNT DIDN'T EXIST AND WE ARE LOGGED IN WTFZ: checkNotAttemptingToFillAkaPurchaseSlotThenTransitionIntoGettingBitcoinKeyState" << endl;

        resumeRendering();
        return;
    }

    m_UserAccountJsonForLockingIntoGettingBitcoinKey = userAccountDoc;
    m_UserAccountCASforBitcoinGettingKeyLockingAndUnlocking = cas;

    ptree pt;
    std::istringstream is(userAccountDoc);
    read_json(is, pt);

    string slotToAttemptToFillAkaPurchase_LOCKED_CHECK = pt.get<std::string>("slotToAttemptToFillAkaPurchase", "n");
    if(slotToAttemptToFillAkaPurchase_LOCKED_CHECK != "n")
    {
        m_AddFundsPlaceholderLayout->addWidget(new WText("It appears you are attempting to purchase a slot. Wait until that finishes and try again"));
        resumeRendering();
        return;
    }

    string bitcoinState = pt.get<std::string>("bitcoinState");
    if(bitcoinState != "NoKey")
    {
        if(bitcoinState != "GettingKey") //TODOreq: this is our second check? why the below message and not just giving them the key?
        {
            m_AddFundsPlaceholderLayout->addWidget(new WText("Please only use one browser tab/window at a time")); //TODOreq: we should throw them out of add funds mode or something (making them click it again)? idfk
            resumeRendering();
            return;
        }

        //GettingKey (recovery)
        m_UserAccountBitcoinDoingGettingKeyRecoverySoAccountLocked = true;

        //Fill in details from previous GettingKey attempt
        m_BitcoinKeySetIndex_aka_setN = pt.get<std::string>("bitcoinStateData");
        m_BitcoinKeySetPage_aka_PageY = pt.get<std::string>("bitcoinSetPage");
        m_PerGetBitcoinKeyUUID = pt.get<std::string>("bitcoinGetKeyUUID");
    }
    else //bitcoinState == "NoKey"
    {
        //begin transitioning to "GettingKey", but first check slotToAttemptToFillAkaPurchase still isn't set
        m_UserAccountBitcoinDoingGettingKeyRecoverySoAccountLocked = false;

        //generate random set number
        mt19937 rng;
        rng.seed(static_cast<int>(WDateTime::currentDateTime().toTime_t()));
        uniform_int_distribution<> zeroThrough999(0,(ABC_NUM_BITCOINKEYSETS-1));
        m_BitcoinKeySetIndex_aka_setN = boost::lexical_cast<std::string>(zeroThrough999(rng));
        //generate uuid-per-request (could be done later, but needs to not overwrite recovery one set above, so best to do it here/now)
        string uuidSeed = m_CurrentlyLoggedInUsername + uniqueId() + WDateTime::currentDateTime().toString().toUTF8() + boost::lexical_cast<std::string>(m_BitcoinKeySetIndex_aka_setN);
        m_PerGetBitcoinKeyUUID = base64Encode(sha1(uuidSeed)); //after one look at boost::uuid, looks shit

        //get the page that that set is currently on
        m_BitcoinKeySetPage_aka_PageY = ""; //hack to tell us we aren't in recovery code (recovery code already has page, _AND_ recovery can't use 'most recent' because it needs to check 'old' pages for the UUID).
        //TODOreq: ^not there yet, but i think i need a similar hack for PageZ
    }

    //recovery and normal mode MERGE

    //save cas because way later if we try to fill the next page, we need the cas to lock it
    getCouchbaseDocumentByKeySavingCasBegin("bitcoinKeySet" + m_BitcoinKeySetIndex_aka_setN + "_CurrentPage"); //this exact get (incl enum) is copy/pasted to a later point when a cas-swap fails
    m_WhatTheGetSavingCasWasFor = GETBITCOINKEYSETNCURRENTPAGETOSEEWHATPAGEITISONANDIFITISLOCKED;

    //doesn't belong here, but see "SHIT editting this doc" in "bitcoin.nonsense.ramblings.[...].txt", it is a TODOreq that is very important
}
void AnonymousBitcoinComputingWtGUI::verifyUserHasSufficientFundsAndThatTheirAccountIsntAlreadyLockedAndThenStartTryingToLockItIfItIsntAlreadyLocked(const string &userAccountJsonDoc, u_int64_t cas, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        //TODOreq: now that i think of it, a lot of these errors, whether 500 or 'lcbOpSuccess' are going to require rolling back things. maybe just gui type stuff, but maybe db type stuff. lots to consider.
        //TODOreq: 500 internal server error
        return;
    }
    if(!lcbOpSuccess)
    {
        //TODOreq: 500 internal server error
        cerr << "TOTAL SYSTEM FAILURE: got 'key does not exist' in 'verify user has sufficient funds', which requires them to already be logged in (them logging in proves the key exists)" << endl;
        return;
    }

    m_UserAccountLockedDuringBuyJson = userAccountJsonDoc; //our starting point for when we debit the user account during unlock (after the slot fill later on)
    ptree pt;
    std::istringstream is(userAccountJsonDoc);
    read_json(is, pt);

    std::string ALREADY_LOCKED_CHECK_slotToAttemptToFillAkaPurchase = pt.get<std::string>("slotToAttemptToFillAkaPurchase", "n");
    if(ALREADY_LOCKED_CHECK_slotToAttemptToFillAkaPurchase == "n")
    {
        //not already locked
        std::string userBalanceString = pt.get<std::string>("balance");
        double userBalance = boost::lexical_cast<double>(userBalanceString);

        //TO DOnereq: we should probably calculate balance HERE/now instead of in buySlotStep2d3faultCampaign0ButtonClicked (where it isn't even needed). Those extra [SUB ;-P]-milliseconds will get me millions and millions of satoshis over time muahhahaha superman 3

        double currentDateTime = static_cast<double>(WDateTime::currentDateTime().toTime_t());

        if(m_HackedInD3faultCampaign0_NoPreviousSlotPurchases)
        {
            //no last purchase, so use min price
            m_CurrentPriceToUseForBuying = boost::lexical_cast<double>(m_HackedInD3faultCampaign0_MinPrice);
            m_HackedInD3faultCampaign0_LastSlotPurchasesIsExpired = true; //not necessary to set here, but to be on the safe side..
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
                m_HackedInD3faultCampaign0_LastSlotPurchasesIsExpired = true;
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

            int slotIndexToAttemptToBuy = 0;
            if(!m_HackedInD3faultCampaign0_NoPreviousSlotPurchases)
            {
                slotIndexToAttemptToBuy = (boost::lexical_cast<int>(m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedSlotIndex) + 1);
            }
            m_AdSlotIndexToBeFilledIfLockIsSuccessful_AndForUseInUpdateCampaignDocAfterPurchase = boost::lexical_cast<std::string>(slotIndexToAttemptToBuy);
            m_AdSlotAboutToBeFilledIfLockIsSuccessful = "adSpaceSlotsd3fault0Slot" + m_AdSlotIndexToBeFilledIfLockIsSuccessful_AndForUseInUpdateCampaignDocAfterPurchase;
            pt.put("slotToAttemptToFillAkaPurchase", m_AdSlotAboutToBeFilledIfLockIsSuccessful);
            //TODOreq (read next TODOreq first): seriously it appears as though i'm still not getting the user's input to verify the slot index. we could have a 'buy event' that could update m_HackedInD3faultCampaign0_LastSlotFilledAkaPurchasedSlotIndex and even push it to their client just milliseconds before they hit 'buy' step 2.... and if they have sufficient funds they'd now buy at near-exactly twice what they wanted to [and be pissed]. i need some boolean guards in javascript surrounding a "are you sure" prompt thingo (except i can't/shouldn't depend on js so gah) -> jsignal-emit-with-that-value -> unlock the boolean guards (to allow buy events to update that slot index and/or price). an amateur wouldn't see this HUGE bug
            //TODOreq: ^bleh, i should 'lock in the slot index' when the user clicks buy step 1, DUH (fuck js) (if they receive a buy event during that time, we undo step 1, requiring them to click it again [at a now double price]. BUT it's _VITAL_ that i don't allow the internal code to modify their locked in slot index and allow them to proceed forward with the buy)
            pt.put("slotToAttemptToFillAkaPurchaseItWith", m_SlotFillerToUseInBuy);
            std::ostringstream jsonDocBuffer;
            write_json(jsonDocBuffer, pt, false);
            std::string accountLockedForBuyJsonDoc = jsonDocBuffer.str();

            store_SETonly_CouchbaseDocumentByKeyWithInputCasBegin("user" + m_CurrentlyLoggedInUsername, accountLockedForBuyJsonDoc, cas, StoreCouchbaseDocumentByKeyRequest::SaveOutputCasMode);
            m_WhatTheStoreWithInputCasSavingOutputCasWasFor = HACKEDIND3FAULTCAMPAIGN0BUYSTEP2bLOCKACCOUNTFORBUYINGSETWITHCASSAVINGCAS;
        }
        else
        {
            new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
            new WText("Insufficient Funds. Add Funds Here: ", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
            new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ACCOUNT), ABC_ANCHOR_TEXTS_ACCOUNT, m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
            resumeRendering();
            return;
            //TO DOnereq(did opposite, no account activity while purchase is in progress): doesn't belong here but i probalby shouldn't let them do ANY purchase activity when they have 'pending'/unconfirmed (bitcoin) funds. It just complicates the logic too much and I'm probably going to use account locking for that too
        }
    }
    else
    {
        //we probably want to allow them to continue with the buy even if the account is locked, so long as it's locked "to" the same one they're trying to buy now (buy failure recovery) -- TODOreq: this should go either in login recovery or here, but not both (since getting _here_ requires being logged in (ALTHOUGH ACTUALLY NVM, IT IS POSSIBLE TO GET HERE IF THE DB/node FAILS WHILE THEY ARE LOGGED IN LOOKING AT BUY PAGE FFFFFF))
        //Semi-outdated:
        //TO DOnereq(describes the roll-back hack): account already locked so error out of this buy. they're logged in elsewhere and trying to buy two things at once? etc. It jumps at me that it might be a place to do 'recovery' code, but I think for now I'm only going to do that during 'login' (TO DOnereq: maybe doing recovery at login isn't such a good idea (at least, the PROCEED WITH BUY kind isn't a good idea (rollback is DEFINITELY good idea (once we verify that they didn't get it(OMG RACE CONDITION HACKABLE TO DOnereq: they are on two machines they log in on a different one just after clicking "buy" and get lucky so that their account IS locked, but the separate-login-machine checks to see if they got the slot. When it sees they didn't, it rolls back their account. Meanwhile the original machine they clicked "buy" on is still trying to buy the slot (by LCB_ADD'ing the slot). The hack depends on the machine they press "buy" on being slower (more load, etc) than the alternate one they log in to (AND NOTE, BY MACHINE I MEAN WT SERVER, not end user machine). So wtf rolling back is dangerous? Wat do. It would also depend on the buying machine crashing immediately after the slot is purchased, because otherwise it would be all like 'hey wtf who unlocked that account motherfucker, I was still working on it' and at least error out TO DOnereq(can't think of a way to detect that scenario because recovery-possy can cas-swap-unlock+debitting also)))))
        //^very real race condition vuln aside, what i was getting at originally is that maybe it's not a good idea to do "recover->proceed-with-buy" because who the fuck knows how much later they'd log in... and like maybe currentPrice would be waaaaay less than when they originally locked/tried-and-failed. It makes sense to at least ask them: "do you want to try this buy again" and then to also recalculate the price (which means re-locking the account (which probably has security considerations to boot)) on login
    }
}
void AnonymousBitcoinComputingWtGUI::determineBitcoinStateButDontLetThemProceedForwardIfLockedAttemptingToFillAkaPurchaseSlot(const string &userAccountJsonDoc, u_int64_t casOnlyUsedWhenBitcoinStateIsInGettingKey_aka_lightRecovery, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        //TODOreq: notify and fail

        //temp:
        cerr << "determineBitcoinStateButDontLetThemProceedForwardIfLockedAttemptingToFillAkaPurchaseSlot db fail" << endl;

        resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //get failed?? should never happen

        //TODOreq: notify and fail

        //temp (we should 'notify' ourselves for any of these total system failures), or i guess hook one up to cerr ;-P
        cerr << "TOTAL SYSTEM FAILURE: user account doc doesn't exist and we're logged in wtfz? in determineBitcoinStateButDontLetThemProceedForwardIfLockedAttemptingToFillAkaPurchaseSlot method" << endl;

        resumeRendering();
        return;
    }

    //got user account doc

    //we can tell them the state of add funds (tell them their key if they have one), but we must not allow transitions if "slot attempting to fill" is set (TODOreq: we also check it isn't set after the buttons are clicked, in case they have multiple tabs open). For example we shouldn't proceed with recovery if both "slotToAttemptToFillAkaPurchase" is set and BitcoinState = GettingKey. We must first unlock the slot fill lock, since it is more important.
    ptree pt;
    std::istringstream is(userAccountJsonDoc);
    read_json(is, pt);

    string slotToAttemptToFillAkaPurchase_LOCKED_CHECK = pt.get<std::string>("slotToAttemptToFillAkaPurchase", "n");
    string bitcoinState = pt.get<std::string>("bitcoinState"); //TODOreq: registration sets this to "NoKey"
    if(bitcoinState == "HaveKey")
    {
        presentBitcoinKeyForPaymentsToUser(pt.get<std::string>("bitcoinStateData"));
    }
    else if(slotToAttemptToFillAkaPurchase_LOCKED_CHECK == "n")
    {
        if(bitcoinState == "NoKey")
        {
            WPushButton *getKeyButton = new WPushButton("Get Bitcoin Key"); //TODOreq: hide after clicking, unhide somewhere(s) (or just delete/recreate idfk)
            getKeyButton->clicked().connect(this, &AnonymousBitcoinComputingWtGUI::handleGetBitcoinKeyButtonClicked);
            m_AddFundsPlaceholderLayout->addWidget(getKeyButton);
        }
        else if(bitcoinState == "GettingKey")
        {
            //light/safe recovery (don't resume rendering [yet] either)
            checkNotAttemptingToFillAkaPurchaseSlotThenTransitionIntoGettingBitcoinKeyState(userAccountJsonDoc, casOnlyUsedWhenBitcoinStateIsInGettingKey_aka_lightRecovery, true, false); //re-parsing json and re-checking slotToAttemptToFillAkaPurchase not set, but saving lines of code and complexity...

            return; //no resume rendering. TO DOnereq(worked-in-testing): since recovery and the button are slightly different paths, my defer/resume counts might not match and i need to hackily fix that. starting to think they match up perfectly. at least the code functions just fine. "get bitcoin key" push button slot defers, whereas we are ALREADY deferred and join up with that code moments later... so yea i think they match...
        }
    }
    else
    {
        m_AddFundsPlaceholderLayout->addWidget(new WText("You are attempting to purchase a slot, so bitcoin key activity is disabled until that finishes"));
    }
    resumeRendering();
}
void AnonymousBitcoinComputingWtGUI::presentBitcoinKeyForPaymentsToUser(const string &bitcoinKey)
{
    m_CurrentBitcoinKeyForPayments = bitcoinKey;
    m_AddFundsPlaceholderLayout->addWidget(new WText("Current Bitcoin Key: " + bitcoinKey));

    WPushButton *checkForPendingButton = new WPushButton("Check For Pending Bitcoins"); //changes to "Check For Pending Again" after > 0 pending seen
    checkForPendingButton->clicked().connect(this, &AnonymousBitcoinComputingWtGUI::checkForPendingBitcoinBalanceButtonClicked);
    m_AddFundsPlaceholderLayout->addWidget(checkForPendingButton); //TODOreq: 2 keys in same session needs to not remake this (and similar) buttons

    //check for pending and check for confirmed don't modify the BitcoinState in the user-account, so that means they are safe to perform even when slotToAttemptToFillAkaPurchase is set
}
void AnonymousBitcoinComputingWtGUI::gotBitcoinKeySetNpageYSoAnalyzeItForUUIDandEnoughRoomEtc(const string &bitcoinKeySetNpageY, u_int64_t bitcoinKeySetNpageY_CAS, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        //TODOreq: handle and notify

        //temp:
        cerr << "gotBitcoinKeySetNpageYSoAnalyzeItForUUIDandEnoughRoomEtc db error" << endl;

        resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //TODOreq: handle and notify

        //temp:
        cerr << "TOTAL SYSTEM FAILURE: lcbOpFail: bitcoinKeySetNpageY was supposed to exist but didn't in gotBitcoinKeySetNpageYSoAnalyzeItForUUIDandEnoughRoomEtc" << endl;

        resumeRendering();
        return;
    }

    //got bitcoinKeySetNpageY
    ptree pt;
    std::istringstream is(bitcoinKeySetNpageY);
    read_json(is, pt);

    bool foundPerKeyRequestUuidOnThisPage = false;
    bool doneInsertingIntoFirstUnclaimed = false; //we insert on first one we find, BUT we might discard the json if we 'detect' the uuid later (unlikely)

    for(int i = 0; i < ABC_NUM_BITCOIN_KEYS_ON_EACH_BITCOINKEYSET_PAGE; ++i)
    {
        const std::string keyPrefix = "key" + boost::lexical_cast<std::string>(i);
        const std::string &bitcoinKey = pt.get<std::string>(keyPrefix); //yes, key is get'd by only the prefix (english teachers are raging)
        const std::string &claimedUuid = pt.get<std::string>(keyPrefix + "claimedUuid", "n");
        //we don't care about GET'ing the claimedUsername (TO DOnereq: be sure to pt::put it (is just good book keeping imo, since the uuid will vanish later)
        if(claimedUuid != "n") //claimed, check uuid
        {
            if(claimedUuid == m_PerGetBitcoinKeyUUID)
            {
                m_BitcoinKeyToGiveToUserOncePerKeyRequestUuidIsOnABitcoinKeySetPage = bitcoinKey;
                foundPerKeyRequestUuidOnThisPage = true;
                break;
            }
        }
        else if(!doneInsertingIntoFirstUnclaimed) //unclaimed spot found! but we might not need it if the uuid is seen during this iteration
        {
            m_BitcoinKeyToGiveToUserOncePerKeyRequestUuidIsOnABitcoinKeySetPage = bitcoinKey;
            pt.put(keyPrefix + "claimedUuid", m_PerGetBitcoinKeyUUID);
            pt.put(keyPrefix + "claimedUsername", m_CurrentlyLoggedInUsername);
            doneInsertingIntoFirstUnclaimed = true;
            break;
        }
    }

    if(foundPerKeyRequestUuidOnThisPage)
    {
        //woot the uuid is matched up with a bitcoin key, now present it to the user

        unlockUserAccountSafelyFromBitcoinGettingKeyBecauseShitWeGotAfuckingKey_NoSweatIfBeatenToIt();
    }
    else //uuid not found
    {
        if(doneInsertingIntoFirstUnclaimed)
        {
            //space left (and we already added outselves claiming one of the keys, but that doc hasn't yet been cas-swap-stored)

            //jump to unlockUserAccountSafelyFromBitcoinGettingKeyBecauseShitWeGotAfuckingKey_NoSweatIfBeatenToIt if the cas (the m_BitcoinKeySetNpageY_CAS_ForClaimingAkey one below, NOT the cas mentioned in the method name) swap succeeds

            std::ostringstream jsonDocBuffer;
            write_json(jsonDocBuffer, pt, false);

            store_SETonly_CouchbaseDocumentByKeyWithInputCasBegin("bitcoinKeySet" + m_BitcoinKeySetIndex_aka_setN + "_Page" + m_BitcoinKeySetPage_aka_PageY, jsonDocBuffer.str(),bitcoinKeySetNpageY_CAS, StoreCouchbaseDocumentByKeyRequest::DiscardOuputCasMode);
            m_WhatTheStoreWithInputCasWasFor = CLAIMBITCOINKEYONBITCOINKEYSETVIACASSWAP;
        }
        else
        {
            //no space left
            //get next page (fill if not exist, check uuid/space loop if exist)

            //PageY++
            m_BitcoinKeySetPage_aka_PageY = boost::lexical_cast<std::string>(boost::lexical_cast<int>(m_BitcoinKeySetPage_aka_PageY)+1);
            getCouchbaseDocumentByKeySavingCasBegin("bitcoinKeySet" + m_BitcoinKeySetIndex_aka_setN + "_Page" + m_BitcoinKeySetPage_aka_PageY);
            m_WhatTheGetSavingCasWasFor = GETBITCOINKEYSETNPAGEYANDIFITEXISTSLOOPAROUNDCHECKINGUUIDBUTIFNOTEXISTMAKEITEXISTBITCH;
        }
    }
}
void AnonymousBitcoinComputingWtGUI::getBitcoinKeySetNPageYAttemptFinishedSoCheckItIfItExistsAndMakeItIfItDont(const string &bitcoinKeySetNpageY_orNot, u_int64_t bitcoinKeySetNpageY_CAS_orNot, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        //TODOreq: handle and notify

        //temp:
        cerr << "getBitcoinKeySetNPageYAttemptFinishedSoCheckItIfItExistsAndMakeItIfItDont db error" << endl;

        resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //or not ;-)

        //fill next page
        //Get hugeBitcoinKeyList_CurrentPage to see what page it's on
        getCouchbaseDocumentByKeySavingCasBegin("hugeBitcoinKeyList_CurrentPage");
        m_WhatTheGetSavingCasWasFor = GETHUGEBITCOINKEYLISTCURRENTPAGE;
    }
    else
    {
        //race condition
        //loop around to checking UUID/space-left, and possibly even getting here again <3
        gotBitcoinKeySetNpageYSoAnalyzeItForUUIDandEnoughRoomEtc(bitcoinKeySetNpageY_orNot, bitcoinKeySetNpageY_CAS_orNot, true, false);
    }
}
void AnonymousBitcoinComputingWtGUI::getHugeBitcoinKeyListActualPageAttemptCompleted(const string &hugeBitcoinKeyListActualPage, u_int64_t casForUsingInSafelyInsertingOurPerFillUuid, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        //TODOreq: handle and notify

        //temp:
        cerr << "getHugeBitcoinKeyListActualPageAttemptCompleted db error" << endl;

        resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //re-lock 'filling from page Z' in the db pointing to this non-existent page, to ensure the OLD/full page isn't pulled like crazy which would likely make the nodes run out of memory
        if(m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList_NON_CHANGING != m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList) //hack but fuck it
        {
            ptree pt2;

            //TODOreq: not sure it's safe to re-make this doc for re-locking it with new Z using these member variables (stale/inaccurate maybe???), but my instincts tell me it is safe and they are accurate. Still, so many fucking recovery paths and shit I'm a tad worried...
            pt2.put("fillingNextBitcoinKeySetPage", m_BitcoinKeySetPage_aka_PageY);
            pt2.put("uuidPerFill", m_FillingNextBitcoinKeySetPerFillUuid_ForAfterCASswapLockSucceeds);
            pt2.put("hugeBitcoinKeyListStartingPage", m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList);

            std::ostringstream reLockedToNewPageZJsonBuffer;
            write_json(reLockedToNewPageZJsonBuffer, pt2, false);

            store_SETonly_CouchbaseDocumentByKeyWithInputCasBegin("bitcoinKeySet" + m_BitcoinKeySetIndex_aka_setN + "_CurrentPage", reLockedToNewPageZJsonBuffer.str(), m_BitcoinKeySetCurrentPageCASForFillingNextPageWayLater, StoreCouchbaseDocumentByKeyRequest::DiscardOuputCasMode);
            m_WhatTheStoreWithInputCasWasFor = RELOCKBITCOINKEYSETN_CURRENTPAGETONONEXISTENTPAGEZASNECESSARYOPTIMIZATION;

            //displays the error (as in the else) after the op completes
        }
        else
        {
            //just display the error
            showOutOfBitcoinKeysErrorToUserInAddFundsPlaceholderLayout();
        }
        return; //huge error to go on
    }

    //got hugeBitcoinKeyListActualPage
    ptree pt;
    std::istringstream is(hugeBitcoinKeyListActualPage);
    read_json(is, pt);

    bool foundPerRefillUuidOnThisPage = false;
    bool doneInsertingIntoFirstUnclaimed = false; //we insert on first one we find, BUT we might discard the json if we 'detect' the uuid later (unlikely)

    for(int i = 0; i < ABC_NUM_BITCOIN_KEY_RANGES_ON_EACH_HUGEBITCOINLIST_PAGE; ++i) //100 key ranges of 100 keys each, so 10k keys per hugeBitcoinKeyList page (~350kb/page)
    {
        const std::string keyPrefix = "keyRange" + boost::lexical_cast<std::string>(i);
        const std::string &claimedUuid = pt.get<std::string>(keyPrefix + "claimedUuid", "n");
        //we don't care about GET'ing the claimedSetN (or PageY if we decide to utilize it)

        if(claimedUuid != "n") //claimed, check uuid
        {
            if(claimedUuid == m_FillingNextBitcoinKeySetPerFillUuid_ForAfterCASswapLockSucceeds)
            {
                //copy-paste'd below
                string commaSeparatedKeyRangeFromHugeBitcoinKeyList = pt.get<std::string>(keyPrefix);
                m_BitcoinKeysVectorToUseForNextPageFillOncePerFillUuidIsSeenOnHugeBitcoinKeyList.clear();
                boost::split(m_BitcoinKeysVectorToUseForNextPageFillOncePerFillUuidIsSeenOnHugeBitcoinKeyList,commaSeparatedKeyRangeFromHugeBitcoinKeyList,boost::is_any_of(","));

                foundPerRefillUuidOnThisPage = true;
                break;
            }
        }
        else if(!doneInsertingIntoFirstUnclaimed) //unclaimed spot found! but we might not need it if the uuid is seen during this iteration
        {
            //copy-pasted from above
            string commaSeparatedKeyRangeFromHugeBitcoinKeyList = pt.get<std::string>(keyPrefix);
            m_BitcoinKeysVectorToUseForNextPageFillOncePerFillUuidIsSeenOnHugeBitcoinKeyList.clear();
            boost::split(m_BitcoinKeysVectorToUseForNextPageFillOncePerFillUuidIsSeenOnHugeBitcoinKeyList,commaSeparatedKeyRangeFromHugeBitcoinKeyList,boost::is_any_of(","));

            pt.put(keyPrefix + "claimedUuid", m_FillingNextBitcoinKeySetPerFillUuid_ForAfterCASswapLockSucceeds);
            //pt.put(keyPrefix + "claimedSetN", m_FillingNextBitcoinKeySetPage_ForAfterCASswapLockSucceeds);
            //TODOreq: PageY stored? i think pageY is what above refers to as "SetPage"... so where the fuck is setN?

            doneInsertingIntoFirstUnclaimed = true;
            break;
        }
    }

    if(foundPerRefillUuidOnThisPage)
    {
        //woot the uuid is matched up with a bitcoin key range, now use the key range for the next page fill
        uuidPerRefillIsSeenOnHugeBitcoinListSoProceedWithActualNextPageFill(); //method because we jump directly to it when the cas-swap below succeeds
    }
    else //uuid not found
    {
        if(doneInsertingIntoFirstUnclaimed)
        {
            //space left (and we already added outselves claiming one of the key ranges, but that doc hasn't yet been cas-swap-stored)

            std::ostringstream jsonDocBuffer;
            write_json(jsonDocBuffer, pt, false);

            //CAS-swap attempt hugeBitcoinKeyList[pageZ] claiming a range for the next bitcoinKeySet[setN]_PageY
            store_SETonly_CouchbaseDocumentByKeyWithInputCasBegin("hugeBitcoinKeyList_Page" + m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList, jsonDocBuffer.str(), casForUsingInSafelyInsertingOurPerFillUuid, StoreCouchbaseDocumentByKeyRequest::DiscardOuputCasMode);
            m_WhatTheStoreWithInputCasWasFor = HUGEBITCOINKEYLISTKEYRANGECLAIMATTEMPT;
        }
        else
        {
            //no space left, but there might be a next page

            //TODOreq: regarding what i was wondering with whether the hugeBitcoinKeyList_CurrentPage increment should be a cas-swap or just a plane set/store, I am now leaning towards cas swap because how do we know that it isn't now on the page after the one we are setting it to? It probably doesn't matter though because I think it'd just get set'd again after realizing that the page in between is full. I am unsure tbh, but keep this in mind: recovery needs to iterate all the pages between where it was locked to and the current page (might not even be relevant).

            //++PageZ;
            m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList = boost::lexical_cast<std::string>(boost::lexical_cast<int>(m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList)+1);

            if(m_PageZisFromCurrentPageInDbSoCasSwapIncremementCurrentPageWhenIncrementingPageZ_AsOpposedToPageZbeingSeenFromLockedRecoveryInWhichCaseDont)
            {
                //set CurrentPage, then go to the method directly called in the else (even if cas swap fails? methinks yes)
                ptree pt2;
                pt2.put("currentPage", m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList);
                std::ostringstream jsonDocBuffer;
                write_json(jsonDocBuffer, pt2, false);
                store_SETonly_CouchbaseDocumentByKeyWithInputCasBegin("hugeBitcoinKeyList_CurrentPage", jsonDocBuffer.str(), m_CasToUseForSafelyUpdatingHugeBitcoinKeyList_CurrentPageUsingIncrementedPageZ, StoreCouchbaseDocumentByKeyRequest::DiscardOuputCasMode);
                m_WhatTheStoreWithInputCasWasFor = HUGEBITCOINKEYLISTPAGECHANGE;
            }
            else
            {
                bitcoinKeySetN_currentPage_Locked_soDoKeyRangeClaimAndNextPageCreation();
                //old/error: proceedToBitcoinKeySetNgettingAfterLockingUserAccountInto_GetAkeyFromPageYofSetNusingUuidPerKeyRequest_UnlessUserAccountAlreadyLocked();
            }
        }
    }
}
void AnonymousBitcoinComputingWtGUI::creditConfirmedBitcoinAmountAfterAnalyzingUserAccount(const string &userAccountJsonDoc, u_int64_t userAccountCAS, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        //TODOreq: handle and notify

        //temp:
        cerr << "creditConfirmedBitcoinAmountAfterAnalyzingUserAccount db error" << endl;

        resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //TODOreq: handle and notify

        //temp:
        cerr << "TOTAL SYSTEM FAILURE: user account didn't exist and must. creditConfirmedBitcoinAmountAfterAnalyzingUserAccount" << endl;

        resumeRendering();
        return;
    }

    //got user-account doc

    std::istringstream is(userAccountJsonDoc);
    ptree pt;
    read_json(is, pt);

    string slotToAttemptToFillAkaPurchase_LOCKED_CHECK = pt.get<std::string>("slotToAttemptToFillAkaPurchase", "n");
    if(slotToAttemptToFillAkaPurchase_LOCKED_CHECK != "n")
    {
        m_AddFundsPlaceholderLayout->addWidget(new WText("Please try again in a few moments"));
        resumeRendering();
        return;
    }

    //check that they're not trying to credit the amount multiple times
    string bitcoinState = pt.get<std::string>("bitcoinState");
    string bitcoinStateData = pt.get<std::string>("bitcoinStateData", "n"); //key ("n" because they could segfault us by doing the hack we're about to check for)
    if(bitcoinState != "HaveKey" || bitcoinStateData != m_CurrentBitcoinKeyForPayments)
    {
        m_AddFundsPlaceholderLayout->addWidget(new WText("Nice try, you already creditted those bitcoins in another tab/window ;-)")); //TODOreq: no-js allows you to use the same session in multiple windows by copy/pasting sessionId, but what are the implications for that with the state of the web-app. Can they be viewing different pages? Don't they share member variables? I can't exactly put my finger on it, but I think that this hack still might be vulnerable. Something like this: tab1 is at 'done' step, just before clicking (and getting here). tab2 then steals sessionId and (???). Still not sure what they could do (change the key? idfk. don't think they could even do that...), BUT the paranoid thought wonders whether or not tab1 can still click 'done' or not. i also wrote a TODOreq about this up towards the top, relating to when the fuck a slot is not longer activatable (idk lol)

        resumeRendering();
        return;
    }

    double userBalance = boost::lexical_cast<double>(pt.get<std::string>("balance"));
    userBalance += m_ConfirmedBitcoinBalanceToBeCredittedWhenDoneButtonClicked; //TODOreq: usual paranoia applies... rounding errors, etc

    pt.put("balance", boost::lexical_cast<std::string>(userBalance));
    pt.put("bitcoinState", "NoKey");
    pt.erase("bitcoinStateData");

    std::ostringstream userAccountWithConfirmedBitcoinAmountCredittedJsonBuffer;
    write_json(userAccountWithConfirmedBitcoinAmountCredittedJsonBuffer, pt, false);

    store_SETonly_CouchbaseDocumentByKeyWithInputCasBegin("user" + m_CurrentlyLoggedInUsername, userAccountWithConfirmedBitcoinAmountCredittedJsonBuffer.str(), userAccountCAS, StoreCouchbaseDocumentByKeyRequest::DiscardOuputCasMode);
    m_WhatTheStoreWithInputCasWasFor = CREDITCONFIRMEDBITCOINBALANCEFORCURRENTPAYMENTKEYCASSWAPUSERACCOUNT;
}
void AnonymousBitcoinComputingWtGUI::uuidPerRefillIsSeenOnHugeBitcoinListSoProceedWithActualNextPageFill()
{
    //TODOreq
    //create bitcoinKeySet[setN] Page Y using the range obtained from hugeBitcoinKeyList[pageZ] via an LCB_ADD-accepting fail
    ptree pt;
    int bitcoinKeysVectorSize = static_cast<int>(m_BitcoinKeysVectorToUseForNextPageFillOncePerFillUuidIsSeenOnHugeBitcoinKeyList.size());
    for(int i = 0; i < bitcoinKeysVectorSize; ++i)
    {
        pt.put("key" + boost::lexical_cast<std::string>(i), m_BitcoinKeysVectorToUseForNextPageFillOncePerFillUuidIsSeenOnHugeBitcoinKeyList[i]); //A stupid but actual TODOoptimization (don't do it) is to claim our key now.. would save us from doing it later. But nah suicide/insanity doesn't sound that fun, I'll pass...
    }

    std::ostringstream bitcoinKeySetPageJsonBuffer;
    write_json(bitcoinKeySetPageJsonBuffer, pt, false);

    store_ADDbyDefault_WithoutInputCasCouchbaseDocumentByKeyBegin("bitcoinKeySet" + m_BitcoinKeySetIndex_aka_setN + "_Page" + m_BitcoinKeySetPage_aka_PageY, bitcoinKeySetPageJsonBuffer.str(), StoreCouchbaseDocumentByKeyRequest::AddMode);
    m_WhatTheStoreWithoutInputCasWasFor = BITCOINKEYSETNPAGEYCREATIONVIALCBADD;

    m_BitcoinKeysVectorToUseForNextPageFillOncePerFillUuidIsSeenOnHugeBitcoinKeyList.clear();
}
void AnonymousBitcoinComputingWtGUI::unlockUserAccountSafelyFromBitcoinGettingKeyBecauseShitWeGotAfuckingKey_NoSweatIfBeatenToIt()
{
    ptree pt;
    std::istringstream is(m_UserAccountJsonForLockingIntoGettingBitcoinKey);
    read_json(is, pt);

    pt.put("bitcoinState", "HaveKey"); //TODOreq: this would be a good 'last resort' place to check if "slotToAttemptToFillAkaPurchase" is set and to fail/stop if it is... but it really should be checked earlier. AND besides there is also teh 'account locking into GettingKey' which would be a similar 'last resort' (two of em xD) to check for it. Depending on how confident you are that you checked them in the original GET...
    pt.put("bitcoinStateData", m_BitcoinKeyToGiveToUserOncePerKeyRequestUuidIsOnABitcoinKeySetPage); //TODOreq: pt::erase when transitioning to NoKey

    //pt::erase the extra "GettingKey" fields
    pt.erase("bitcoinSetPage");
    pt.erase("bitcoinGetKeyUUID");

    std::ostringstream userAccountUnlockedToHaveKeyModeJsonDocBuffer;
    write_json(userAccountUnlockedToHaveKeyModeJsonDocBuffer, pt, false);

    store_SETonly_CouchbaseDocumentByKeyWithInputCasBegin("user" + m_CurrentlyLoggedInUsername, userAccountUnlockedToHaveKeyModeJsonDocBuffer.str(), m_UserAccountCASforBitcoinGettingKeyLockingAndUnlocking, StoreCouchbaseDocumentByKeyRequest::DiscardOuputCasMode);
    m_WhatTheStoreWithInputCasWasFor = UNLOCKUSERACCOUNTFROMBITCOINGETTINGKEYTOBITCOINHAVEKEY;
}
void AnonymousBitcoinComputingWtGUI::checkForPendingBitcoinBalanceButtonClicked()
{
    //TODOreq: do check, enable "Check For Confirmed" button if appropriate

    m_BitcoinAddressBalancePollerPollingPendingBalance = true;
    sendRpcJsonBalanceRequestToBitcoinD();

    //TODOreq: doesn't belong here, but later in this code path after the "Done With This Key" button is pressed, it is IMPERITIVE that we check that slotToAttemptToFillAkaPurchase isn't set on the user-account before transitioning from BitcoinState:HaveKey to BitcoinState:NoKey (creditting account at same time). The GET for that CAS-swap is where we should make sure slotToAttemptToFillAkaPurchase is not set (if it is, fail saying try again later or stop using multiple tabs noob)
}
void AnonymousBitcoinComputingWtGUI::checkForConfirmedBitcoinBalanceButtonClicked()
{
    m_BitcoinAddressBalancePollerPollingPendingBalance = false;
    sendRpcJsonBalanceRequestToBitcoinD();
}
void AnonymousBitcoinComputingWtGUI::doneSendingBitcoinsToCurrentAddressButtonClicked()
{
    //cas-swap creditting user-account, but TODOreq: fail and say try again in a few moments _IF_ slotToAttemptToFill[...] is present (ie., the account is locked). Set bitcoinState back to NoKey during creditting cas-swap

    //TODOreq: (had:i am unsure if it makes sense to) first check that bitcoinState is even at "HaveKey" and that the key we have in the member variable is accurate. A tiny hunch (probably paranoia) tells me they could maybe credit the amount twice if we don't perform that verification... perhaps using two tabs or fuck I don't know (the cas swap should protect us from this, what am I on about (NO WAIT I AM RIGHT HAHA: two (or 'N' holy shit huuuuuge bug just squashed) sessions/tabs, walk both through pending->confirmed process, 'done' on first session, it completes, 'done' on respective sessions, they complete. We should DEFINITELY verify that bitcoinState is HaveKey and that the key is accurate!!! We should detect the double(N)-credit and laugh at them :-P. It is NOT a cas-swap-fail, but is an inaccurate bitcoinState or bitcoinStateData(key)))

    deferRendering();

    //get user-account for cas-swap-credit
    getCouchbaseDocumentByKeySavingCasBegin("user" + m_CurrentlyLoggedInUsername);
    m_WhatTheGetSavingCasWasFor = CREDITCONFIRMEDBITCOINAMOUNTAFTERANALYZINGUSERACCOUNT;
}
void AnonymousBitcoinComputingWtGUI::sendRpcJsonBalanceRequestToBitcoinD()
{
    //TODOreq: prevent DDOS of checking balance, since I _think_ it's an expensive operation. A simple 1-check-per-X-seconds would suffice, and there is also the design where I poll 'getblockcount' every few seconds and then only even bother doing a minconf=1 balance check when the block count changes
    if(!m_BitcoinAddressBalancePoller)
    {
        m_BitcoinAddressBalancePoller = new Wt::Http::Client(this);
        m_BitcoinAddressBalancePoller->setTimeout(30);
        m_BitcoinAddressBalancePoller->setMaximumResponseSize(2048);
        m_BitcoinAddressBalancePoller->done().connect(boost::bind(&AnonymousBitcoinComputingWtGUI::handleBitcoinAddressBalancePollerReceivedResponse, this, _1, _2));
    }
    Wt::Http::Message bitcoinJsonRpcMessage;
    bitcoinJsonRpcMessage.addHeader("content-type", "text/plain");
    bitcoinJsonRpcMessage.addBodyText("{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"getreceivedbyaddress\", \"params\": [\"" + m_CurrentBitcoinKeyForPayments + "\", " + (m_BitcoinAddressBalancePollerPollingPendingBalance ? "0" : "1" ) + "] }");

    if(m_BitcoinAddressBalancePoller->post("http://testu:testp@127.0.0.1:19001/", bitcoinJsonRpcMessage))
    {
        deferRendering();
    }
    else
    {
        //TODOreq: error message, shouldn't ever happen though... fuck it
    }
}
void AnonymousBitcoinComputingWtGUI::handleBitcoinAddressBalancePollerReceivedResponse(boost::system::error_code err, const Http::Message &response)
{
    resumeRendering();
    if (!err && response.status() == 200)
    {
        std::istringstream is(response.body());
        ptree pt;
        read_json(is, pt);
        if(pt.get<std::string>("error") != "null")
        {
            //TODOreq: handle and notify
            cerr << "error in json returned handleBitcoinAddressBalancePollerReceivedResponse: " << response.body() << endl;
            return;
        }
        string balanceString = pt.get<std::string>("result");
        double balance = boost::lexical_cast<double>(balanceString);
        if(balance > 0.0)
        {
            if(m_BitcoinAddressBalancePollerPollingPendingBalance)
            {
                if(!m_PendingBitcoinBalanceLabel)
                {
                    m_PendingBitcoinBalanceLabel = new WText();
                    m_AddFundsPlaceholderLayout->addWidget(m_PendingBitcoinBalanceLabel);

                    //enable 'check for confirmed'
                    WPushButton *checkForConfirmedPushButton = new WPushButton("Check For Confirmed Bitcoins");
                    checkForConfirmedPushButton->clicked().connect(this, &AnonymousBitcoinComputingWtGUI::checkForConfirmedBitcoinBalanceButtonClicked);
                }
                //display pending balance
                m_PendingBitcoinBalanceLabel->setText("Pending Bitcoins Received: " + balanceString);
            }
            else
            {
                if(!m_ConfirmedBitcoinBalanceLabel)
                {
                    m_ConfirmedBitcoinBalanceLabel = new WText();
                    m_AddFundsPlaceholderLayout->addWidget(m_ConfirmedBitcoinBalanceLabel);

                    //enable 'done with this key'
                    WPushButton *doneSendingBitcoinsButton = new WPushButton("Done Sending Bitcoins To This Address (Add To My Account)"); //TODOreq: explain more thoroughly to user that they can not send bitcoins to that address anymore after clicking this. Give a huge warning
                    doneSendingBitcoinsButton->clicked().connect(this, &AnonymousBitcoinComputingWtGUI::doneSendingBitcoinsToCurrentAddressButtonClicked);
                }
                //display confirmed balance
                m_ConfirmedBitcoinBalanceToBeCredittedWhenDoneButtonClicked = balance;
                m_ConfirmedBitcoinBalanceLabel->setText("Confirmed Bitcoins Received: " + balanceString);
            }
        }
        else
        {
            if(m_BitcoinAddressBalancePollerPollingPendingBalance)
            {
                m_AddFundsPlaceholderLayout->addWidget(new WText("No pending bitcoins received yet. Try again in a minute or so. You did send them, right ;-P?"));
            }
            else
            {
                m_AddFundsPlaceholderLayout->addWidget(new WText("It takes roughly 10 minutes for pending bitcoins to become confirmed. Try again when the next bitcoin block is mined."));
            }
        }
    }
    else
    {
        //TODOreq: handle and notify. lots of reasons it could fail, simple timeout for example
    }
}
void AnonymousBitcoinComputingWtGUI::handleGetBitcoinKeyButtonClicked()
{
    deferRendering();

    //TODOreq: get user-account for CAS, check AGAIN that slotAttemptingToFill isn't set, then do the hella long pseudo-code for getting a bitcoin key

    getCouchbaseDocumentByKeySavingCasBegin("user" + m_CurrentlyLoggedInUsername);
    m_WhatTheGetSavingCasWasFor = GETUSERACCOUNTFORGOINGINTOGETTINGBITCOINKEYMODE;
}
void AnonymousBitcoinComputingWtGUI::determineNextSlotFillerIndexAndThenAddSlotFillerToIt(const string &allAdSlotFillersJsonDoc, u_int64_t casOfAllSlotFillersDocForUpdatingSafely, bool lcbOpSuccess, bool dbError)
{
    //TODOreq: doesn't belong here, but there's a race condition that we wouldn't recover from using current recover design for ad slot filler creation. user has 3 tabs open and hits submit for all 3 at the same time. guh i can't think of it but it has something to do with the last cas-swap-accepting-fail update of allAdSlotFillers doc at the end... AND i might be wrong anyways and it isn't a problem. BUT IF IT IS, it's a potential segfault.

    if(dbError)
    {
        //TODOreq: handle. "500 Internal Server Error: Kill Yourself, Re-Evolve, Try Again (doing so puts you in an infinite loop of re-evolving/suiciding-at-the-same-spot that keeps looping and looping... until you eventually choose to not commit suicide. And since you can't live in a universe where you are dead (heh), you are already here in the universe where you chose not kill yourself. So yea just Try Again)"

        //temp:
        cerr << "determineNextSlotFillerIndexAndThenAddSlotFillerToIt db error" << endl;

        resumeRendering();
        return;
    }

    m_RunningAllSlotFillersJsonDoc.clear();
    m_RunningAllSlotFillersJsonDocCAS = casOfAllSlotFillersDocForUpdatingSafely;

    string slotFillerIndexToAttemptToAdd;

    if(!lcbOpSuccess)
    {
        //TODOreq: this is first slot filler add (or 2nd and first partially failed xD), allSlotFillersDoc doesn't exist yet, make it [after adding slot filler]. perhaps just set a flag indicating that after the slot filler is added, allSlotFillers doc needs to be ADD-accepting-fail INSTEAD OF cas-swap-accepting-fail (no previous = no cas)

        slotFillerIndexToAttemptToAdd = "0";
        m_RunningAllSlotFillersJsonDoc.put("adsCount", slotFillerIndexToAttemptToAdd); //initialize this 'string' index because later we +1 it, and it simplifies the code paths to already have it existing [on first ad slot filler create]
    }
    else
    {
        //allSlotFillersDoc exists
        std::istringstream is(allAdSlotFillersJsonDoc);
        read_json(is, m_RunningAllSlotFillersJsonDoc);

        slotFillerIndexToAttemptToAdd = m_RunningAllSlotFillersJsonDoc.get<std::string>("adsCount"); //seems like off-by-one to not add "+1" here, but isn't :-P
    }

    tryToAddAdSlotFillerToCouchbase(slotFillerIndexToAttemptToAdd);
    //^will most likely succeed, but our inline recovery code uses it too
}
void AnonymousBitcoinComputingWtGUI::tryToAddAdSlotFillerToCouchbase(const string &slotFillerIndexToAttemptToAdd)
{
    ptree pt;
    pt.put("username", m_CurrentlyLoggedInUsername);
    pt.put("nickname", m_UploadNewSlotFiller_NICKNAME->text().toUTF8()); //TODOreq: sanitize these
    pt.put("hoverText", m_UploadNewSlotFiller_HOVERTEXT->text().toUTF8());
    pt.put("url", m_UploadNewSlotFiller_URL->text().toUTF8());

    m_AdImageUploadFileLocation = m_AdImageUploader->spoolFileName();
    m_AdImageUploader->stealSpooledFile(); //TODOreq: if i re-use this WFileUpload object, will the 2nd+ file upload be already stolen? Will it use a new spoolFileName? Easiest way around this is to just make a new WFileUpload object for new uploads
    m_AdImageUploadClientFilename = m_AdImageUploader->clientFileName().toUTF8(); //TODOreq: sanitize before being passed to the mime type checker (or make sure mime type checker works safely)

    //unsure if i should do the expensive read/b64encode here or in the db backend (passing only filename). i want an async api obviously, but laziness/KISS might dictate otherwise

    //TODOoptimization: had this as 'req' but memory management/sanity is much simpler when doing it less optimially xD (it's only ever paid during recovery code anyways): when doing inline recovery, don't re-read from disk. also if we then 'store it' as member or some such, make sure to delete it when it finishes successfully. The solution should use some form of WObject parent/child deletion...
    streampos fileSizeHack;
    char *adImageBuffer; //TODOreq: maybe a smart pointer as a member of this class, so that it deallocates if the session dies (could be while we're still trying to store it, for example). Pretty much just like the WObject tree, somehow attach it.
    ifstream adImageFileStream(m_AdImageUploadFileLocation.c_str(), ios::in | ios::binary | ios::ate);
    if(adImageFileStream.is_open())
    {
        fileSizeHack = adImageFileStream.tellg();
        adImageBuffer = new char[fileSizeHack]; //TODOreq: delete[]. TODOoptimization: running out of memory (server critical load) throws an exception, we could pre-allocate this, but how to then share it among WApplications? guh fuckit. i think 'new' is mutex protected underneath anyways, so bleh maybe using a mutex and a pre-allocated buffer is even faster than this (would of course be best to use rand() + mutex shits xD)... but then we can't have multiple doing it on thread pool at same time (unless rand() + mutex shits)
        adImageFileStream.seekg(0,ios::beg);
        adImageFileStream.read(adImageBuffer, fileSizeHack);
        adImageFileStream.close();
    }
    else
    {
        //TODOreq: handle file failed to open error (delete, discard, give user error)
    }

    std::string adImageString = std::string(adImageBuffer, fileSizeHack);
    std::string adImageB64string = base64Encode(adImageString); //TODOreq: doesn't encoding in base64 make it larger, so don't i need to make my "StoreLarge" message size bigger? I know in theory it doesn't change the size, but it all depends on representation or some such idfk (i get con-

    pt.put("adImageB64", adImageB64string); //TODOoptimization: maybe save a copy by doing base64encode right into second argument here? doubt it (and if this were Qt land, I'd be hesitant to even try it (shit disappears yo))

    std::ostringstream adSlotFillerJsonDocBuffer;
    write_json(adSlotFillerJsonDocBuffer, pt, false);


    storeLarge_ADDbyDefault_WithoutInputCasCouchbaseDocumentByKeyBegin("adSpaceSlotFillers" + m_CurrentlyLoggedInUsername + slotFillerIndexToAttemptToAdd, adSlotFillerJsonDocBuffer.str());
    //storeLarge has it's own 'begin', but it shares the 'finish' with the typical store
    m_WhatTheStoreWithoutInputCasWasFor = LARGE_ADIMAGEUPLOADBUYERSETTINGUPADSLOTFILLERFORUSEINPURCHASINGLATERONSTOREWITHOUTINPUTCAS; //TODOoptional: make these enums more readable with underscores, and put numbering toward the beginning of them that matches up with the callback/function (perhaps renaming those too)

    delete [] adImageBuffer;
}
void AnonymousBitcoinComputingWtGUI::continueRecoveringLockedAccountAtLoginAttempt(const string &maybeExistentSlot, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        new WBreak(m_LoginWidget);
        new WText("500 Internal Server Error, try again later", m_LoginWidget);
        resumeRendering();
        return;
    }

    if(!lcbOpSuccess)
    {
        //slot not purchased/filled
        new WText("We're sorry, our system has experience a temporary failure and your account is locked until someone else buys that slot you tried to purchase during your last session. Please try logging in again once that slot is purchased", m_LoginWidget);
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

        std::string slotFilledWith = pt.get<std::string>("slotFilledWith");
        if(slotFilledWith == m_AccountLockedRecoveryWhatTheUserWasTryingToFillTheSlotWithHack)
        {
            //we got it, so do NOTHING because recovery process will handle it :-)
            new WText("Try logging in again in like 1 minute (don't ask why)", m_LoginWidget); //fuck teh police~
            resumeRendering();
        }
        else
        {
            //we didn't get it, so it is now safe to roll-back/unlock the user account without debitting

            ptree pt2;
            std::istringstream is2(m_UserAccountLockedJsonToMaybeUseInAccountRecoveryAtLogin);
            read_json(is2, pt2);

            pt2.erase("slotToAttemptToFillAkaPurchase");
            pt2.erase("slotToAttemptToFillAkaPurchaseItWith");

            std::ostringstream userAccountUnlockedJsonBuffer;
            write_json(userAccountUnlockedJsonBuffer, pt2, false);

            //cas-swap-unlock(no-debit)-accepting-fail (neighbor logins could have recovered)
            store_SETonly_CouchbaseDocumentByKeyWithInputCasBegin("user" + m_CurrentlyLoggedInUsername, userAccountUnlockedJsonBuffer.str(), m_CasFromUserAccountLockedAndStuckLockedButErrRecordedDuringRecoveryProcessAfterLoginOrSomethingLoLWutIamHighButActuallyNotNeedMoneyToGetHighGuhLifeLoLSoErrLemmeTellYouAboutMyDay, StoreCouchbaseDocumentByKeyRequest::DiscardOuputCasMode);
            m_WhatTheStoreWithInputCasWasFor = LOGINACCOUNTRECOVERYUNLOCKINGWITHOUTDEBITTINGUSERACCOUNT;
        }
    }
}
void AnonymousBitcoinComputingWtGUI::analyzeBitcoinKeySetN_CurrentPageDocToSeeWhatPageItIsOnAndIfItIsLocked(const string &bitcoinKeySetCurrentPageDoc, u_int64_t casOnlyForUseInFillingNextPageWayLaterOn, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        //TODOreq: handle and notify

        //temp:
        cerr << "analyzeBitcoinKeySetPageCacheDocToSeeWhatPageItIsOnAndIfItIsLocked db error" << endl;

        resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //TODOreq: handle and notify

        //temp:
        cerr << "TOTAL SYSTEM FAILURE: analyzeBitcoinKeySetPageCacheDocToSeeWhatPageItIsOnAndIfItIsLocked lcb op failed" << endl;

        resumeRendering();
        return;
    }

    m_BitcoinKeySetCurrentPageCASForFillingNextPageWayLater = casOnlyForUseInFillingNextPageWayLaterOn;

    //got the CurrentPage
    ptree pt;
    std::istringstream is(bitcoinKeySetCurrentPageDoc);
    read_json(is, pt);

    std::string fillingNextBitcoinKeySetPage_LOCKED_CHECK = pt.get<std::string>("fillingNextBitcoinKeySetPage", "n"); //TODOreq: pt::erase this and siblings at appropriate places
    if(fillingNextBitcoinKeySetPage_LOCKED_CHECK != "n")
    {
        //light recovery of filling next bitcoin key set page
        //jumps way the fuck deep into recovery code, then pops back to us later if user account isn't locked (flag already set previously).
        //TODOreq: it sets PageY before popping back to us if user account isn't locked via direct call to attemptToLockUserAccountInto_GetAkeyFromPageYofSetNusingUuidPerKeyRequest();

        //populate the fillingNextBitcoinKeySetPage/uuid/pageZ from the locked doc (we are recovery. normal code path generates the 2nd two)
        m_BitcoinKeySetPage_aka_PageY = fillingNextBitcoinKeySetPage_LOCKED_CHECK;
        m_FillingNextBitcoinKeySetPerFillUuid_ForAfterCASswapLockSucceeds = pt.get<std::string>("uuidPerFill");
        m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList = pt.get<std::string>("hugeBitcoinKeyListStartingPage");
        m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList_NON_CHANGING = m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList;

        m_PageZisFromCurrentPageInDbSoCasSwapIncremementCurrentPageWhenIncrementingPageZ_AsOpposedToPageZbeingSeenFromLockedRecoveryInWhichCaseDont = false;

        //the jump into recovery
        bitcoinKeySetN_currentPage_Locked_soDoKeyRangeClaimAndNextPageCreation();
    }
    else //bitcoin key set not locked in fillingNextBitcoinKeySetPage mode, so set PageY using it's contents
    {
        if(m_BitcoinKeySetPage_aka_PageY == "")
        {
            //aren't in get-key recovery code, so we need to get/set PageY
            m_BitcoinKeySetPage_aka_PageY = pt.get<std::string>("currentPage");
        }
        proceedToBitcoinKeySetNgettingAfterLockingUserAccountInto_GetAkeyFromPageYofSetNusingUuidPerKeyRequest_UnlessUserAccountAlreadyLocked(); //method because we jump "[back]" to it after doing the page-fill also, and some other places
    }
}
void AnonymousBitcoinComputingWtGUI::proceedToBitcoinKeySetNgettingAfterLockingUserAccountInto_GetAkeyFromPageYofSetNusingUuidPerKeyRequest_UnlessUserAccountAlreadyLocked()
{
    if(!m_UserAccountBitcoinDoingGettingKeyRecoverySoAccountLocked)
    {
        //CAS-Swap-Lock attmempt a user-specific doc into "attempting to get a key from PageY of setN using UUID-per-key-request"

        //TODOreq: verify we already checked slotAttemptingToBeFilled isn't set. pretty sure we did.

        ptree pt;
        std::istringstream is(m_UserAccountJsonForLockingIntoGettingBitcoinKey);
        read_json(is, pt);

        pt.put("bitcoinState", "GettingKey");
        pt.put("bitcoinStateData", m_BitcoinKeySetIndex_aka_setN); //TODOreq: pt::erase when transitioning to NoKey
        pt.put("bitcoinSetPage", m_BitcoinKeySetPage_aka_PageY); //TODOreq: pt::erase when transitioning to HaveKey -- methinks these bottom two are implicitly erased because i use m_UserAccountJsonForLockingIntoGettingBitcoinKey as basis for unlocking
        pt.put("bitcoinGetKeyUUID", m_PerGetBitcoinKeyUUID); //TODOreq: pt::erase when transitioning to HaveKey

        std::ostringstream jsonDocBuffer;
        write_json(jsonDocBuffer, pt, false);

        store_SETonly_CouchbaseDocumentByKeyWithInputCasBegin("user" + m_CurrentlyLoggedInUsername, jsonDocBuffer.str(), m_UserAccountCASforBitcoinGettingKeyLockingAndUnlocking, StoreCouchbaseDocumentByKeyRequest::SaveOutputCasMode);
        m_WhatTheStoreWithInputCasSavingOutputCasWasFor = USERACCOUNTBITCOINLOCKEDINTOGETTINGKEYMODE;
    }
    else
    {
        //TODOreq (done?): already locked, so jump straight into post-locked code (TODOreq: we use the get'd cas from when we detected it was locked(and accept fail cas-swap obviously))
        userAccountBitcoinGettingKeyLocked_So_GetSavingCASbitcoinKeySetNPageY();
    }
}
void AnonymousBitcoinComputingWtGUI::hugeBitcoinKeyListCurrentPageGetComplete(const string &hugeBitcoinKeyList_CurrentPageJson, u_int64_t cas, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        //TODOreq: handle and notify

        //temp:
        cerr << "hugeBitcoinKeyListCurrentPageGetComplete db error" << endl;

        resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //TODOreq: handle and notify

        //temp:
        cerr << "TOTAL SYSTEM FAILURE hugeBitcoinKeyList_CurrentPage didn't exist and must" << endl;

        resumeRendering();
        return;
    }

    //got hugeBitcoinKeyList_CurrentPage
    //this code path knows that it isn't already locked, so Attempt: bitcoinKeySet[setN]_CurrentPage CAS-swap-locked into "fillingNextBitcoin with UUID-per-fill on pageZ of hugeBitcoinKeyList" mode

    m_PageZisFromCurrentPageInDbSoCasSwapIncremementCurrentPageWhenIncrementingPageZ_AsOpposedToPageZbeingSeenFromLockedRecoveryInWhichCaseDont = true;
    m_CasToUseForSafelyUpdatingHugeBitcoinKeyList_CurrentPageUsingIncrementedPageZ = cas;

    ptree pt;
    std::istringstream is(hugeBitcoinKeyList_CurrentPageJson);
    read_json(is, pt);

    //remember the values put into the cas-swap, so we have them handy if/when the cas-swap succeeds
    m_BitcoinKeySetPage_aka_PageY = boost::lexical_cast<std::string>(m_BitcoinKeySetPage_aka_PageY);
    string uuidSeed = m_CurrentlyLoggedInUsername + uniqueId() + WDateTime::currentDateTime().toString().toUTF8() + boost::lexical_cast<std::string>(m_BitcoinKeySetIndex_aka_setN) + m_BitcoinKeySetPage_aka_PageY;
    m_FillingNextBitcoinKeySetPerFillUuid_ForAfterCASswapLockSucceeds = base64Encode(sha1(uuidSeed));
    m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList = pt.get<std::string>("currentPage");
    m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList_NON_CHANGING = m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList;

    //now put them into json
    ptree pt2;
    pt2.put("fillingNextBitcoinKeySetPage", m_BitcoinKeySetPage_aka_PageY);
    pt2.put("uuidPerFill", m_FillingNextBitcoinKeySetPerFillUuid_ForAfterCASswapLockSucceeds);
    pt2.put("hugeBitcoinKeyListStartingPage", m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList); //TODOreq: recovering from this doesn't segfault etc when that pageZ doesn't exist
    std::ostringstream jsonDocBuffer;
    write_json(jsonDocBuffer, pt2, false);

    store_SETonly_CouchbaseDocumentByKeyWithInputCasBegin("bitcoinKeySet" + m_BitcoinKeySetIndex_aka_setN + "_CurrentPage", jsonDocBuffer.str(), m_BitcoinKeySetCurrentPageCASForFillingNextPageWayLater, StoreCouchbaseDocumentByKeyRequest::SaveOutputCasMode);
    m_WhatTheStoreWithInputCasSavingOutputCasWasFor = LOCKINGBITCOINKEYSETNINTOFILLINGNEXTPAGEMODE;
}
void AnonymousBitcoinComputingWtGUI::getAdSlotFillerThatIsntInAllAdSlotFillersAttemptFinished_soAddItToAllAddSlotFillersAndInitiateSlotFillerAddAtNextIndex(const string &adSlotFillerToExtractNicknameFrom, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        //TODOreq: handle and notify

        cerr << "getAdSlotFillerThatIsntInAllAdSlotFillersAttemptFinished_soAddItToAllAddSlotFillersAndInitiateSlotFillerAddAtNextIndex db error" << endl;
        resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //TODOreq: handle?
        cerr << "TOTAL SYSTEM FAILURE: getAdSlotFillerThatIsntInAllAdSlotFillersAttemptFinished_soAddItToAllAddSlotFillersAndInitiateSlotFillerAddAtNextIndex -- an LCB_ADD to slot filler failed, so we tried to get it to extract nickname from it for updating allAdSlotFillers... but now it doesn't exist?? wtf?" << endl;
        resumeRendering();
        return;
    }

    //get succeeded
    ptree pt;
    std::istringstream is(adSlotFillerToExtractNicknameFrom);
    read_json(is, pt);

    std::string nicknameOfSlotFillerNotInAllAdSlotFillersDoc = pt.get<std::string>("nickname"); //mfw the entire base64 image is get'd just to figure out this nickname :)... but this is recovery code and won't run often

    //json-add nickname to the 'running' allAdSlotFillers doc
    std::string indexWeTriedToAddToBecauseAllSlotFillersDocSaidItWasEmptyButItWasWrongSoHereWeAreRecoveringFromThat = m_RunningAllSlotFillersJsonDoc.get<std::string>("adsCount");
    m_RunningAllSlotFillersJsonDoc.put(indexWeTriedToAddToBecauseAllSlotFillersDocSaidItWasEmptyButItWasWrongSoHereWeAreRecoveringFromThat, nicknameOfSlotFillerNotInAllAdSlotFillersDoc);

    //also increment 'adsCount' because we've just found/recovered one slot filler, and additionally will be using adsCount as our index to add the slot filler AGAIN (the one that initiated this recovery) in the 'next' slot
    indexWeTriedToAddToBecauseAllSlotFillersDocSaidItWasEmptyButItWasWrongSoHereWeAreRecoveringFromThat = boost::lexical_cast<std::string>(boost::lexical_cast<int>(indexWeTriedToAddToBecauseAllSlotFillersDocSaidItWasEmptyButItWasWrongSoHereWeAreRecoveringFromThat) + 1); //the +1 here is important (had:key)!!!!
    m_RunningAllSlotFillersJsonDoc.put("adsCount", indexWeTriedToAddToBecauseAllSlotFillersDocSaidItWasEmptyButItWasWrongSoHereWeAreRecoveringFromThat);

    //now LCB_ADD index+1 again
    tryToAddAdSlotFillerToCouchbase(indexWeTriedToAddToBecauseAllSlotFillersDocSaidItWasEmptyButItWasWrongSoHereWeAreRecoveringFromThat);
}
void AnonymousBitcoinComputingWtGUI::userAccountLockAttemptFinish_IfOkayDoTheActualSlotFillAdd(u_int64_t casFromLockSoWeCanSafelyUnlockLater, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        //TODOreq: 500 internal server error?
        return;
    }
    if(!lcbOpSuccess)
    {
        new WText("It appears you're logged in to more than one location and are trying to make multiple purchases simultaneously. You can only do one at a time", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        return;
    }

    //if we get here, the user account is locked and pointing to the slot we're about to fill

    m_CasFromUserAccountLockSoWeCanSafelyUnlockLater = casFromLockSoWeCanSafelyUnlockLater;
    //TODOreq: unlock user account after successful add
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

    //populate 'slot filler' doc
    ptree pt;
    pt.put("purchaseTimestamp", m_PurchaseTimestampForUseInSlotItselfAndAlsoUpdatingCampaignDocAfterPurchase);
    pt.put("startTimestamp", m_StartTimestampUsedInNewPurchase);
    pt.put("purchasePrice", m_CurrentPriceToUseForBuyingString);
    pt.put("slotFilledWith", m_SlotFillerToUseInBuy);
    std::ostringstream jsonDocBuffer;
    write_json(jsonDocBuffer, pt, false);
    store_ADDbyDefault_WithoutInputCasCouchbaseDocumentByKeyBegin(m_AdSlotAboutToBeFilledIfLockIsSuccessful, jsonDocBuffer.str());
    m_WhatTheStoreWithoutInputCasWasFor = BUYAKAFILLSLOTWITHSLOTFILLERSTOREWITHOUTINPUTCAS;
}
void AnonymousBitcoinComputingWtGUI::userAccountBitcoinLockedIntoGettingKeyAttemptComplete(u_int64_t casFromLockSoWeCanSafelyUnlockLater, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        //TODOreq: handle and notify

        //temp:
        cerr << "userAccountBitcoinLockedIntoGettingKeyAttemptComplete db error" << endl;

        resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        m_AddFundsPlaceholderLayout->addWidget(new WText("Please only use one browser tab/window at a time. Try again")); //TODOreq: make this error the same in all places i use it
        resumeRendering();
        return;
    }

    m_UserAccountCASforBitcoinGettingKeyLockingAndUnlocking = casFromLockSoWeCanSafelyUnlockLater;

    //using a method here because recovery jumps directly to it
    userAccountBitcoinGettingKeyLocked_So_GetSavingCASbitcoinKeySetNPageY();
}
void AnonymousBitcoinComputingWtGUI::userAccountBitcoinGettingKeyLocked_So_GetSavingCASbitcoinKeySetNPageY()
{
    //get bitcoinKeySet[setN]_PageY
    getCouchbaseDocumentByKeySavingCasBegin("bitcoinKeySet" + m_BitcoinKeySetIndex_aka_setN + "_Page" + m_BitcoinKeySetPage_aka_PageY);
    m_WhatTheGetSavingCasWasFor = GETBITCOINKEYSETNACTUALPAGETOSEEIFUUIDONITENOUGHROOM;
}
void AnonymousBitcoinComputingWtGUI::attemptToLockBitcoinKeySetNintoFillingNextPageModeComplete(u_int64_t casFromLockSoWeCanSafelyUnlockAfterNextPageFillComplete, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        //TODOreq: handle and notify

        //temp:
        cerr << "attemptToLockBitcoinKeySetNintoFillingNextPageModeComplete db error" << endl;

        resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //another neighbor/user (or maybe our own recovery alternate tab) beat us to it, no biggy
        m_AddFundsPlaceholderLayout->addWidget(new WText("A recoverable error occured, please try again in a few moments"));
        resumeRendering();
        return;
    }

    m_BitcoinKeySetCurrentPageCASForFillingNextPageWayLater = casFromLockSoWeCanSafelyUnlockAfterNextPageFillComplete;

    //bitcoinKeySet[setN]_CurrentPage now locked, so do the actual next page creation
    bitcoinKeySetN_currentPage_Locked_soDoKeyRangeClaimAndNextPageCreation(); //method because it is also recovery jump point
}
void AnonymousBitcoinComputingWtGUI::bitcoinKeySetN_currentPage_Locked_soDoKeyRangeClaimAndNextPageCreation()
{
    //this method expects m_FillingNextBitcoinKeySetPage_ForAfterCASswapLockSucceeds, m_FillingNextBitcoinKeySetPerFillUuid_ForAfterCASswapLockSucceeds, and m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList to be accurate before calling

    //Get hugeBitcoinKeyList[pageZ] attempt in order to retrieve it's CAS, check for relevant-UUID, and see if room left
    getCouchbaseDocumentByKeySavingCasBegin("hugeBitcoinKeyList_Page" + m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList);
    m_WhatTheGetSavingCasWasFor = GETHUGEBITCOINKEYLISTACTUALPAGEFORANALYZINGANDMAYBECLAIMINGKEYRANGE;
}
void AnonymousBitcoinComputingWtGUI::slotFillAkaPurchaseAddAttemptFinished(bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        //TODOreq: 500 internal server error?
        return;
    }
    if(!lcbOpSuccess)
    {
        //getting beat to the punch
        new WText("Sorry, someone else bought the slot just moments before you...", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);

        //TODOreq: account unlock without debitting, cas-swap-accepting-fail (just like login account locked recovery, which is already coded)

        return;
    }

    //if we get here, the slot fill purchased and we need to make transaction, unlock user account (debitting), the update campaign doc

    //create transaction doc using lcb_add accepting fail -- i can probably re-use this code later (merge into functions), for now KISS
    ptree pt;
    pt.put("buyer", m_CurrentlyLoggedInUsername);
    pt.put("seller", "d3fault"); //TODOreq: implied/deducable from key name...
    pt.put("amount", m_CurrentPriceToUseForBuyingString);
    std::ostringstream transactionBuffer;
    write_json(transactionBuffer, pt, false);
    store_ADDbyDefault_WithoutInputCasCouchbaseDocumentByKeyBegin(string("txd3fault0Slot") + m_AdSlotIndexToBeFilledIfLockIsSuccessful_AndForUseInUpdateCampaignDocAfterPurchase, transactionBuffer.str(), StoreCouchbaseDocumentByKeyRequest::AddMode);
    m_WhatTheStoreWithoutInputCasWasFor = CREATETRANSACTIONDOCSTOREWITHOUTINPUTCAS; //TODOreq: it goes without saying that 'recovery possy' needs it's own set of these, so as not to conflict
    //TODOreq: i need a way of telling the backend that certain adds (like this one) are okay to fail. But really I already need a whole slew of error case handling to be coded into the backend, I guess I'll just do it later? So basically for adds where fails are not ok, we need to have two code paths... but for this one where the add failing is ok, we just pick up with one code path. I think the easiest way of doing this is to return a bool telling whether or not the add succeeded, and to just ignore it if it doesn't matter. But since there's many many ways of failing, maybe I should be passing around the LCB_ERROR itself? So far I've tried to keep front end and back end separate, so idk maybe "bool opTypeFail and bool dbTypeFail", where the first one is relating to cas/add fails and the second is like "500 internal server error" (of course, the backend would have retried backing off exponentially etc before resorting to that)
}
void AnonymousBitcoinComputingWtGUI::transactionDocCreatedSoCasSwapUnlockAcceptingFailUserAccountDebitting(bool dbError)
{
    if(dbError)
    {
        //TODOreq: 500 internal server error?
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
    std::istringstream is(m_UserAccountLockedDuringBuyJson);
    read_json(is, pt);
    //now do the debit of the balance and put it back in the json doc
    double balancePrePurchase = boost::lexical_cast<double>(pt.get<std::string>("balance"));
    balancePrePurchase -= m_CurrentPriceToUseForBuying; //TODOreq: again, just scurred of rounding errors etc. I think as long as I  use 'more precision than needed' (as much as an double provides), I should be ok...
    pt.put("balance", boost::lexical_cast<std::string>(balancePrePurchase));
    //now convert the json doc back to string for couchbase
    std::ostringstream jsonDocWithBalanceDeducted;
    write_json(jsonDocWithBalanceDeducted, pt, false);

    store_SETonly_CouchbaseDocumentByKeyWithInputCasBegin("user" + m_CurrentlyLoggedInUsername, jsonDocWithBalanceDeducted.str(), m_CasFromUserAccountLockSoWeCanSafelyUnlockLater, StoreCouchbaseDocumentByKeyRequest::DiscardOuputCasMode);
    m_WhatTheStoreWithInputCasWasFor = HACKEDIND3FAULTCAMPAIGN0BUYPURCHASSUCCESSFULSOUNLOCKUSERACCOUNTSAFELYUSINGCAS; //TODOreq: handle cas swap UNLOCK failure. can happen and sometimes will, but mostly won't. handle it by doing nothing but continuing the process

    //TO DOnereq(set without cas): should we have done a multi-get to update the campaign doc also? i actually think not since we don't have the cas for that doc (but we could have gotten it~). more importantly i don't know whether or not i need the cas for it when i do the swap. seems safer no doubt.. but really would there be any contesting?? maybe only subsequent buys milliseconds later (probably in error, but maybe the campaign is just liek popular ya know ;-P)... so yea TODOreq do a cas-swap of the campaign doc, makes sure we're only n+1 the last purchased.... and i think maybe do an exponential backoff trying... because it's the same code that runs for when subsequent ones are purchased seconds later? or no... maybe we just do a set without cas because we know that all attempts to buy will fail until that set is complete (because they try to buy n+1 (which is what we just bought (so it will fail safely as beat-to-thepunch)), not n+2 (until the set we're about to do, is done))
    //TODOreq:^^Does the order of user-account-unlock and setting-the-campaign-doc-with-new-last-purchase matter? can we do them asynchronously (AKA HERE IS WHERE WE'D START 'UPDATING'/LCB_SET'ing CAMPAIGN), or do we need to do one before the other and then wait for whichever goes first to complete?


    //TODOreq: need to update the adSpaceSlotsd3fault0 to make the filler that just succeded the most recent purchased
    //TODOreq: dispatch the buy event to our neighbor wt nodes so they can post the buy event to their end users etc (how da fuq? rpc? get-and-subscribe polling model? guh). polling is easiest and really having N[ode-count] hits ever i[N]terval [m]seconds is still not that expensive (get and subscribe also acts as caching layer so that we don't get 10-freaking-thousand hits per second to that same document)
    //TODOreq: definitely doesn't belong here, but a get-and-subscribe / polling method could do a "poll" for the current value and when the poll finishes, the last end-user could have disconnected in that time and we MIGHT think it would be an error but we'd be wrong. in that case we'd just discard/disregard that last polled value (and of course, not do any more polling until at least one end-user connected)
    //TODOreq: ^it goes without saying that 'buy' events do not use the cached value. (NVM:) Hell, even hitting 'buy step 1' should maybe even do a proper get for the actual value (or just nudge cache to do it for him).... BUT then it becomes a DDOS point. since I'm thinking the polling intervals will be like 100-500ms, doing a non-cached get like that on "buy step 1" is probably not necessary (/NVM). But we definitely absolutely need to do it for buy step 2 (duh), and that is NOT a DDOS point because noobs would be giving ya monay each time so i mean yea ddos all ya fuggan want nobs <3

    //TODOreq: even though it seemed like a small optimization to do both the user-account-unlock and updating of campaign doc with a new 'last purchased' slot, that pattern of programming will lead to disaster so i shouldn't do it just in principle. Yes with this one case it wouldn't have led to disaster, but actually it MIGHT have given extreme circumstances. it opens up a race condition: say the 'user account unlock' happens really fast and the 'update campaign doc' happens to take a while. if we gave control back to the user after the 'user account unlock' was finished, they could do some action that would then conflict with the m_WhatThe[blahblahblah]WasFor related to the 'update campaign doc' store. As in, when the 'update campaign doc' finishes and gets posted back to the WApplication, the user's actions could have started something else and then we wouldn't have handled the finishing of the 'update campaign doc' properly (which would mean that we never notify our neighbors of buy event? actually not a problem if we do 'get-and-subscribe-polling' like i think i'm going to do (BUT LIKE I SAID THIS ONE GETS LUCKY, BUT AN OVERALL PATTERN TO AVOID)). So I'm going to do the successive sets synchronously, despite seeing a clear opportunity for optimization
}
void AnonymousBitcoinComputingWtGUI::storeLargeAdImageInCouchbaseDbAttemptComplete(const string &keyToSlotFillerAttemptedToAddTo, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        //TODOreq: handle appropriately (delete image file prolly), notify user

        //temp:
        cerr << "storeLargeAdImageInCouchbaseDbAttemptComplete reported db error" << endl;

        resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //this indicates that the allAdSlotFillers we GET'd was in a bad state (either it just failed to be updated OR they have two tabs open doing two uploads at once etc). we need to do a GET for the one we just LCB_ADD fail'd in order to get the nickname for that slot filler, then we need to put it in our "running all slot fillers doc" and try to LCB_ADD the n+1 slot...... it all needs to loop sexily and after we finally get an LCB_ADD to succeed, we then take our "running all slot fillers doc" (which has been updated (modified, not submitted) 0+ times), add our final LCB_ADD that _just_ succeeded to it at the very end, then cas-swap-accepting-fail the allAdSlotFillers. This design does have the drawback that multiple tabs uploading simultaneously may frequently get the allAdSlotFillers into a bad state, BUT it will always be recovered from on subsequent updates. AND really it is a pretty rare race condition, because it'd have to go in between a sub-millisecond "GET" and an "LCB_ADD" (excluding the durability portion)..... trying to do that intentionally WHILE uploading binary images 'before' the GET/ADD sequence would be difficult, doing it accidentally is possible but not likely.
        //TODOoptional: give user a button "hey where is my most recently uploaded ad" that does the recovery process manually (fuck this idea for now)

        getCouchbaseDocumentByKeyBegin(keyToSlotFillerAttemptedToAddTo); //couchbase needs a "add or get" lcb op :), maybe i should file it as an enhancement
        m_WhatTheGetWasFor = GETNICKNAMEOFADSLOTFILLERNOTINALLADSLOTFILLERSDOCFORADDINGITTOIT_THEN_TRYADDINGTONEXTSLOTFILLERINDEXPLZ; //kind of grossly inefficient to go in and out of the WApplication context so often like this to do 'business' work, but KISS idgaf suck mah dick. "AbcAppDb" was a 2+ year tangent (still incomplete)
        //TODOreq: ^^if the add mentioned towards the end of that fails, we need to get right back here to this code path so that it loops

        //TODOreq: do it again with the index +1'd... unless you end up letting backend do that (in which case, we'd need to pass ourselves the final key chosen [for use in updating allAdSlotFillers])

        //temp:
        //cerr << "storeLargeAdImageInCouchbaseDbAttemptComplete reported lcb op failed" << endl;

        //resumeRendering();
        return;
    }

    //TODOreq: getting here means the add was successful, so record that fact in allAdSlotFillers
    std::string oldAdsCountButAlsoOurIndexOfTheOneJustAdded = m_RunningAllSlotFillersJsonDoc.get<std::string>("adsCount");
    m_RunningAllSlotFillersJsonDoc.put(oldAdsCountButAlsoOurIndexOfTheOneJustAdded, m_UploadNewSlotFiller_NICKNAME->text().toUTF8());

    //now increment adsCount and put it back in xD
    oldAdsCountButAlsoOurIndexOfTheOneJustAdded = boost::lexical_cast<std::string>(boost::lexical_cast<int>(oldAdsCountButAlsoOurIndexOfTheOneJustAdded)+1); //old becomes new :)
    m_RunningAllSlotFillersJsonDoc.put("adsCount", oldAdsCountButAlsoOurIndexOfTheOneJustAdded);


    //TODOreq: cas-swap-accepting-fail the allAdSlotFillers doc
    std::ostringstream updatedAllAdSlotFillersJsonDocBuffer;
    write_json(updatedAllAdSlotFillersJsonDocBuffer, m_RunningAllSlotFillersJsonDoc, false);
    store_SETonly_CouchbaseDocumentByKeyWithInputCasBegin("adSpaceAllSlotFillers" + m_CurrentlyLoggedInUsername, updatedAllAdSlotFillersJsonDocBuffer.str(), m_RunningAllSlotFillersJsonDocCAS, StoreCouchbaseDocumentByKeyRequest::DiscardOuputCasMode);
    m_WhatTheStoreWithInputCasWasFor = UPDATEALLADSLOTFILLERSDOCSINCEWEJUSTCREATEDNEWADSLOTFILLER;
}
void AnonymousBitcoinComputingWtGUI::doneAttemptingBitcoinKeySetNnextPageYcreation(bool dbError)
{
    if(dbError)
    {
        //TODOreq: handle and notify

        //temp:
        cerr << "doneAttemptingBitcoinKeySetNnextPageYcreation reported db error" << endl;

        resumeRendering();
        return;
    }
    /*if(!lcbOpSuccess)
    {
        //dgaf: recovery/neighbor-user would have made the exact same page (enforced by UUIDs)
    }*/

    //bitcoinKeySet[setN]_PageY now created

    //CAS-swap-unlock bitcoinKeySet[setN]_CurrentPage into normal 'current page = Y' mode (fail is tolerated, just like this add failing was)

    //TODOreq: the cas used for unlocking needs to be stored when doing recovery and seeing it's locked, and in the normal case when the lock that we just did completes. two different places/times

    ptree pt;
    pt.put("currentPage", m_BitcoinKeySetPage_aka_PageY);

    std::ostringstream unlockedCurrentPageDoc;
    write_json(unlockedCurrentPageDoc, pt, false);

    store_SETonly_CouchbaseDocumentByKeyWithInputCasBegin("bitcoinKeySet" + m_BitcoinKeySetIndex_aka_setN + "_CurrentPage", unlockedCurrentPageDoc.str(), m_BitcoinKeySetCurrentPageCASForFillingNextPageWayLater, StoreCouchbaseDocumentByKeyRequest::DiscardOuputCasMode);
    m_WhatTheStoreWithInputCasWasFor = SAFELYUNLOCKBITCOINKEYSETNCURRENTPAGEDOCFAILINGTOLERABLY;
}
void AnonymousBitcoinComputingWtGUI::doneUnlockingUserAccountAfterSuccessfulPurchaseSoNowUpdateCampaignDocCasSwapAcceptingFail_SettingOurPurchaseAsLastPurchase(bool dbError)
{
    if(dbError)
    {
        //TODOreq: 500 internal server error
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
    lastPurchasedPt.put("slotIndex", m_AdSlotIndexToBeFilledIfLockIsSuccessful_AndForUseInUpdateCampaignDocAfterPurchase);
    lastPurchasedPt.put("purchaseTimestamp", m_PurchaseTimestampForUseInSlotItselfAndAlsoUpdatingCampaignDocAfterPurchase);
    lastPurchasedPt.put("startTimestamp", m_StartTimestampUsedInNewPurchase);
    lastPurchasedPt.put("purchasePrice", m_CurrentPriceToUseForBuyingString);

    pt.put_child("lastSlotFilledAkaPurchased", lastPurchasedPt);

    //TODOreq: we also need to MAYBE update 'current' and/or 'next', depending on if it's appropriate. the only one we DEFINITELY update is 'last purchased'. if current is expired and there is a next -- holy race condition batman TODOreq (maybe solution is to not list "next" in the doc... and shit i'm actually starting to think maybe i shouldn't even list current. maybe whether or not there is a current is detected on the fly and when needed... by seeing if the (slotIndex-of-the-just-expired-current)+1 exists.. and if it doesn't then shit we don't have a current :-P... yea actually i'm liking that more and more :). simplifies managing the campaign doc too, only two states: no purchase and last purchase (whether it's expired is not that relevant to us, except maybe later when doing a buy announce event (but... polling...?). STILL, I do feel as though I need to keep track of 'current' SOMEWHERE. My thoughts are drifting and although it does make sense to do the 'get n+1' and that's our next if it exists method, who and when exactly would that be done? If it's the "client" of abc (so like the page where i'm fucking dancing naked coding tripping balls on video), who initiates the "ok expired now gimmeh next" and then abc does the n+1 shit,... err i guess what i mean is who has the final say of what n is to begin with? should for example the client die and come back and forget n, how would it figure out it's state again? I GUESS we could just keep doing n+1 or n-1 until we found it, and hell we could even use hella accurate jumping to the middle of all of our slots based on SlotLength, slot0 start time (polled), and the current date time. Would be accurate as fuck list jump with maybe an extra get to correct off by one. SO maybe the abc client does keep track of "n" and then gives it to abc as a parameter a la "hint". Client(dance-vid-page): abc.getNextSlot(n_justExpired /*or n+1, but it doesn't matter who does the incrementing*/), and if it's 0 (state lost somehow) then abc does the list jump mechanism to find it asap. but guh, i haven't worked out any of the details of the abc client (dance-vid-page), so idfk what it's state or code or ANYTHING even looks like on that side of things...
    //^regardless of what i choose, the fact that we can figure out the current slot by doing math with near 100% accuracy (3 gets tops i'd estimate (first for slot 0, second for our guess based on math (so yea 4 gets if you include campaign doc get), third as potential recovery from mistake in math (timezones? idfk...) -- assumes slotLengthHours never changes of course...
    //^^i think i'll cross that bridge when i get there (buy event and 'gimmeh ad plx client request'). dats how da hackas doeet (that being said, i should now consider current and next to NOT be in the campaign json doc (will re-add later if determined i should))

    //if current is expired and there is no next, we fill our latest purchase in as the current
    //if

    std::ostringstream updatedCampaignJsonDocBuffer;
    write_json(updatedCampaignJsonDocBuffer, pt, false);

    //CAS-swap-accepting-fail
    store_SETonly_CouchbaseDocumentByKeyWithInputCasBegin(string("adSpaceSlotsd3fault0"), updatedCampaignJsonDocBuffer.str(), m_HackedInD3faultCampaign0CasForSafelyUpdatingCampaignDocLaterAfterSuccessfulPurchase, StoreCouchbaseDocumentByKeyRequest::DiscardOuputCasMode);
    m_WhatTheStoreWithInputCasWasFor = HACKEDIND3FAULTCAMPAIGN0USERACCOUNTUNLOCKDONESOUPDATECAMPAIGNDOCSETWITHINPUTCAS;
}
void AnonymousBitcoinComputingWtGUI::doneUpdatingCampaignDocSoErrYeaTellUserWeAreCompletelyDoneWithTheSlotFillAkaPurchase(bool dbError)
{
    if(dbError)
    {
        //TODOreq: 500 internal server error
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WText("There was an internal error, but you may have still purchased the slot. Try refreshing in a few minutes to see if you got the slot. Don't worry, if you didn't get the purchase, you won't be charged", m_AdvertisingBuyAdSpaceD3faultCampaign0Widget); //TODOreq: similar errors where appropriated
        return;
    }

    //if we get here, the campaign doc is updated (probably by us, but not necessarily)

    new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
    new WText("You successfully bought the slot for BTC: " + m_CurrentPriceToUseForBuyingString, m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);

    resumeRendering(); //TODOreq: metric fuck tons of error cases where we still want to resume rendering :)

    //TODOoptimization(dumb): getting here means were the driver (unless i merge this function, but yea) so we can now ahead-of-time-a-la-event-driven update the 'get and subscribe' people (at least the ones on this wt node)... but we don't really NEED to...
}
void AnonymousBitcoinComputingWtGUI::doneAttemptingUserAccountLockedRecoveryUnlockWithoutDebitting(bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        new WBreak(m_LoginWidget);
        new WText("500 Internal Server Error, try again in a few minutes", m_LoginWidget);
        resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        new WBreak(m_LoginWidget);
        new WText("Uhh, try logging in again, I'm not sure what just happened...", m_LoginWidget); //seriously, can't wrap my head around this right now... xD (neighbor login beat them to the recovery.... but what should i _DO_??? it is perhaps NOTHING, but i can't guarantee that the user-account isn't now locked towards something else (and still in failed state (or something confusing fuck it))
        resumeRendering();
        return;
    }

    //account locked recovery complete, make it look like a regular old login
    doLoginTasks();
}
void AnonymousBitcoinComputingWtGUI::doneAttemptingToUnlockUserAccountFromBitcoinGettingKeyToBitcoinHaveKey(bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        //TODOreq: handle and notify

        //temp:
        cerr << "doneAttemptingToUnlockUserAccountFromBitcoinGettingKeyToBitcoinHaveKey db error" << endl;

        resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //could have been a slot fill (takes presedence) or a recovery neighbor simply beat us
        m_AddFundsPlaceholderLayout->addWidget(new WText("Are you using multiple browser tabs/windows? Don't do that. Retry getting the key")); //TODOreq: i guess in this case we DON'T want to hide "Get Key" button? Or we re-enable it or idfk...
        resumeRendering();
        return;
    }

    //GettingKey -> HaveKey transition completed successfully, present key to user
    presentBitcoinKeyForPaymentsToUser(m_BitcoinKeyToGiveToUserOncePerKeyRequestUuidIsOnABitcoinKeySetPage);
    resumeRendering();
}
void AnonymousBitcoinComputingWtGUI::doneAttemptingToUpdateBitcoinKeySetViaCASswapAkaClaimingAKey(bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        //TODOreq: handle, idkf how lawl

        //temp:
        cerr << "doneAttemptingToUpdateBitcoinKeySetViaCASswapAkaClaimingAKey db error" << endl;

        resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //try again (but a neighbor might have succeeded, so check UUIDs after re-getting (and of course, make sure enough space))
        userAccountBitcoinGettingKeyLocked_So_GetSavingCASbitcoinKeySetNPageY();
    }
    else //cas swap succeeded, no need to re-get it
    {
        //jump to dat spot that does user account unlocking (GettingKey -> HaveKey)
        unlockUserAccountSafelyFromBitcoinGettingKeyBecauseShitWeGotAfuckingKey_NoSweatIfBeatenToIt();
    }
}
void AnonymousBitcoinComputingWtGUI::doneAttemptingHugeBitcoinKeyListKeyRangeClaim(bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        //TODOreq: handle and notify

        //temp:
        cerr << "doneAttemptingHugeBitcoinKeyListKeyRangeClaim db error" << endl;

        resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //jump back to the pre-get code path
        bitcoinKeySetN_currentPage_Locked_soDoKeyRangeClaimAndNextPageCreation();
    }
    else
    {
        //cas swap succeeded, so we know our uuid got in
        uuidPerRefillIsSeenOnHugeBitcoinListSoProceedWithActualNextPageFill();
    }
}
void AnonymousBitcoinComputingWtGUI::doneAttemptingToUnlockBitcoinKeySetN_CurrentPage(bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        //TODOreq: handle and notify

        //temp:
        cerr << "doneAttemptingToUnlockBitcoinKeySetN_CurrentPage db error" << endl;

        resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //since unlock failed, jump to a point that probes the lock status (might be unlocked by neighbor, but might be locked to NEXT one now in a rare race condition)
        getCouchbaseDocumentByKeySavingCasBegin("bitcoinKeySet" + m_BitcoinKeySetIndex_aka_setN + "_CurrentPage"); //this 'get' and enum is copy/paste from the end of checkNotAttemptingToFillAkaPurchaseSlotThenTransitionIntoGettingBitcoinKeyState
        m_WhatTheGetSavingCasWasFor = GETBITCOINKEYSETNCURRENTPAGETOSEEWHATPAGEITISONANDIFITISLOCKED;

        //OLD:
        //we COULD have detected that a neighbor (not necessarily recovery) beat us to the punch here, BUT it would have been erroneous to have used such detection as a signal that we can stop. The page fill request could have been filled by any other users/wt-nodes doing get-key requests. Just because we got beat to the page-fill, doesn't mean that we got beat by someone processing the same per-key-request-uuid as us... SO we need to continue on, even if yes in fact it was hte same per-key-request-uuid (we'll detect that later (TODOreq) and then stop
    }
    else
    {
        //jump out of page filling mode and to a point where fillingNextPage is not set, but we don't know if we're in recovery or not so where we land will check that and act accordingly
        proceedToBitcoinKeySetNgettingAfterLockingUserAccountInto_GetAkeyFromPageYofSetNusingUuidPerKeyRequest_UnlessUserAccountAlreadyLocked();
    }
}
void AnonymousBitcoinComputingWtGUI::doneChangingHugeBitcoinKeyListCurrentPage(bool dbError)
{
    if(dbError)
    {
        //TODOreq: handle and notify

        //temp:
        cerr << "doneChangingHugeBitcoinKeyListCurrentPage db error" << endl;

        resumeRendering();
        return;
    }
    //lcbOpFail very likely, means nothing. in fact we don't even need to wait for this operation to complete, but i have no way of saying 'dont-respond' in my wt->couchbase communication, so fuck it :). there are probably tons of other cases wehre i don't need to wait, but KISS (sanity > *)
    bitcoinKeySetN_currentPage_Locked_soDoKeyRangeClaimAndNextPageCreation();
    //old/error: proceedToBitcoinKeySetNgettingAfterLockingUserAccountInto_GetAkeyFromPageYofSetNusingUuidPerKeyRequest_UnlessUserAccountAlreadyLocked();
}
void AnonymousBitcoinComputingWtGUI::doneReLockingBitcoinKeySetN_CurrentPage_withNewFromPageZvalue(bool dbError)
{
    if(dbError)
    {
        //TODOreq: handle and notify

        //temp:
        cerr << "doneReLockingBitcoinKeySetN_CurrentPage_withNewFromPageZvalue db error" << endl;

        resumeRendering();
        return;
    }
    //lcbOpFail very likely, means nothing.

    showOutOfBitcoinKeysErrorToUserInAddFundsPlaceholderLayout();
}
void AnonymousBitcoinComputingWtGUI::doneAttemptingCredittingConfirmedBitcoinBalanceForCurrentPaymentKeyCasSwapUserAccount(bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        //TODOreq: handle and notify

        //temp:
        cerr << "doneAttemptingCredittingConfirmedBitcoinBalanceForCurrentPaymentKeyCasSwapUserAccount db error" << endl;

        resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //TODOreq: loop back around to the same get/get-cas (with exponential backoff) and store-with-cas (getting back to here), or give error message saying try again in a few moments? Going with safer option of retry momentarily message for now... but I have a hunch it is fine and dandy to just go right back to the getSavingCas (re-using the enum too!) because strict sanitization is done there
        m_AddFundsPlaceholderLayout->addWidget(new WText("Please try again in a few moments"));
        resumeRendering();
        return;
    }

    //successful credit of user-account balance && changing of bitcoinState back to "NoKey" :-D

    m_AddFundsPlaceholderLayout->addWidget(new WText("Your account has been creditted BTC: + " + boost::lexical_cast<std::string>(m_ConfirmedBitcoinBalanceToBeCredittedWhenDoneButtonClicked) + ". Do NOT send any more bitcoins to address: " + m_CurrentBitcoinKeyForPayments)); //TODOreq: enable "get bitcoin key" button again, make check-for-pending/confirmed/done buttons go away (delete?).

    m_ConfirmedBitcoinBalanceToBeCredittedWhenDoneButtonClicked = 0.0; //probably pointless, but will make me sleep better at night
    m_CurrentBitcoinKeyForPayments = ""; //ditto


    //TODOreq: also probably a good idea to show their total balance somewhere... I'm thinking the logout widget where it says their name (esp since we already have the user-account doc at that point)

    resumeRendering();
}
void AnonymousBitcoinComputingWtGUI::showOutOfBitcoinKeysErrorToUserInAddFundsPlaceholderLayout()
{
    //TO DOnereq(yes): so the set user-account and bitcoin key set both stay locked until the manual filling of more keys? methinks yes, but still seems strange from this point of view... and i was considering saying "no unlock em!" initially (it probably wouldn't matter, but keeping them locked is an optimization (and laziness wins :-P). just need to make sure it's ok that they remain locked is the point

    //TODOreq: automated/machine notification
    m_AddFundsPlaceholderLayout->addWidget(new WText("The server ran out of bitcoin keys and requires a manual refill by the administrator. If you are within shouting distance of him (or her, but lol there are no girls on the internet), say this: \"OOGA BOOGA I AM THE HUGE BITCOIN KEY REFILL NOTIFICATION OUTSOURCED TO ONE OF YOUR SLAVE COCKSUCKERS\". I'll know what it means."));
    resumeRendering();
}
void AnonymousBitcoinComputingWtGUI::doneAttemptingToUpdateAllAdSlotFillersDocSinceWeJustCreatedANewAdSlotFiller(bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        //TODOreq: handle, idkf how lawl

        //temp:
        cerr << "doneAttemptingToUpdateAllAdSlotFillersDocSinceWeJustCreatedANewAdSlotFiller db error" << endl;

        resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //cas swap fail
        //TODOreq: handle? this is what i was attempting to write might be a non-recoverable fault (not necessarily, but this is where we MIGHT need to do more processing). If they are adding two images simultaneously on two different tabs it would be semi-likely to get here (still quite rare). BUT that common case is handled fine when a 3rd image is uploaded. What does get me confused is if 3 images are uploaded simultaneously... it MIGHT put me in a state of non-recoverable fault (which would cause the app itself to segfault when iterating all ad slot fillers doc for populating the combo box in the buy step 2 population stage)

        //TODOreq: should i still display their image to them? i mean the LCB_ADD did succeed for us to get here... so idfk...
        //^^makes sense that as soon as the image is received from the file upload, i put it in a WFileResource with 'this' as parent, regardless of the db stuff. i also need to make sure i delete the file only ever by deleting that WFileResource, like when the image is too big or something

        //TODOreq: the obvious/expensive (and more work ;-P) answer is to say that if we get _HERE_ we just do an allAdSlotFillers doc verification/recovery process (even though we supposedly/probably just did it!), idfk need to smoke a bowl and meditate on this shit

        cerr << "doneAttemptingToUpdateAllAdSlotFillersDocSinceWeJustCreatedANewAdSlotFiller lcb op fail, may or may not be an error read the comments near this error string xD idfk. MAYBE/probably in recoverable state" << endl;

        resumeRendering();
        return;
    }

    //cas-swap-update succeeded

    //so now show them the image that we already have on the heap (and on the filesystem).... now hmm which to show....

    pair<string,string> guessedExtensionAndMimeType = FileDeletingFileResource::guessExtensionAndMimeType(m_AdImageUploadClientFilename);
    FileDeletingFileResource *adImagePreviewResource = new FileDeletingFileResource("image/" + guessedExtensionAndMimeType.second, m_AdImageUploadFileLocation, guessedExtensionAndMimeType.first, m_AccountWidget); //semi-old (now using m_AccountWidget instead of 'this'): 'this' is set as parent for last resort cleanup of the WResource, but that doesn't account for it's underlying buffer. I also might want to delete this resource if they upload a 2nd/3rd/etc image, BUT TODOreq there are threading issues because the WImage/WResource is served concurrently I believe (or is that only true for static resources?). Perhaps changing internal paths would be a good time to delete all the images (or in WResource::handleRequest itself, since they'll only be used once anyways..), idfk. There is also the issue of me trying to re-use the same byte buffer for a 2nd upload (which is backing a resource for a first upload that hasn't yet finished streaming back / previewing back to them)
    WImage *adImagePreview = new WImage(adImagePreviewResource, m_UploadNewSlotFiller_HOVERTEXT->text()); //NOPE (ownership of image changes when we give it to the WAnchor): so our file is deleted when the image is (parent deletes all children first, and the image must stop using the resource before we delete it (thus, the resource should be the parent))
    adImagePreview->resize(ABC_MAX_AD_SLOT_FILLER_IMAGE_WIDTH_PIXELS, ABC_MAX_AD_SLOT_FILLER_IMAGE_HEIGHT_PIXELS);
    WAnchor *adImageAnchor = new WAnchor(WLink(WLink::Url, m_UploadNewSlotFiller_URL->text().toUTF8()), adImagePreview);


    //difference between these two?
    adImagePreview->setToolTip(m_UploadNewSlotFiller_HOVERTEXT->text());
    adImageAnchor->setToolTip(m_UploadNewSlotFiller_HOVERTEXT->text());

    m_AdImageUploadResultsVLayout->addWidget(adImageAnchor);
    m_AdImageUploadResultsVLayout->addWidget(new WBreak());
    m_AdImageUploadResultsVLayout->addWidget(new WBreak());

    m_UploadNewSlotFiller_HOVERTEXT->setText("");
    m_UploadNewSlotFiller_NICKNAME->setText("");
    m_UploadNewSlotFiller_URL->setText("");
    //TODOreq: also set up the WFileUpload for the next image, should they want to upload another

    resumeRendering();
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
    case ONLOGINACCOUNTLOCKEDRECOVERYDOESSLOTEXISTCHECK:
    {
        continueRecoveringLockedAccountAtLoginAttempt(couchbaseDocument, lcbOpSuccess, dbError);
    }
        break;
    case GETNICKNAMEOFADSLOTFILLERNOTINALLADSLOTFILLERSDOCFORADDINGITTOIT_THEN_TRYADDINGTONEXTSLOTFILLERINDEXPLZ:
    {
        getAdSlotFillerThatIsntInAllAdSlotFillersAttemptFinished_soAddItToAllAddSlotFillersAndInitiateSlotFillerAddAtNextIndex(couchbaseDocument, lcbOpSuccess, dbError); //'slot filler add' = buyer creating a new ad; 'slot fill add' = buyer buying ad slot using created ad. xD too fucking subtle of a difference (but i'm so amped right now i can grasp the difference for the rest of this month long hack marathon)...
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
    case HACKEDIND3FAULTCAMPAIGN0GET:
    {
        finishShowingAdvertisingBuyAdSpaceD3faultCampaign0Widget(couchbaseDocument, cas); //TODOreq: tempted to pass lcbOpsSuccess and dbError into here, but this is ultimately going to change when i implement get-and-subscribe-via-polling so i'm not sure they still apply (not sure they don't apply also!)
    }
        break;
    case HACKEDIND3FAULTCAMPAIGN0BUYSTEP2aVERIFYBALANCEANDGETCASFORSWAPLOCKGET:
    {
        verifyUserHasSufficientFundsAndThatTheirAccountIsntAlreadyLockedAndThenStartTryingToLockItIfItIsntAlreadyLocked(couchbaseDocument, cas, lcbOpSuccess, dbError);
    }
        break;
    case GETUSERACCOUNTFORGOINGINTOGETTINGBITCOINKEYMODE:
    {
        checkNotAttemptingToFillAkaPurchaseSlotThenTransitionIntoGettingBitcoinKeyState(couchbaseDocument, cas, lcbOpSuccess, dbError);
    }
        break;
    case ADDFUNDSBUTTONCLICKEDSODETERMINEBITCOINSTATEBUTDONTLETTHEMPROCEEDIFLOCKEDATTEMPTINGTOFILLAKAPURCHASESLOT:
    {
        determineBitcoinStateButDontLetThemProceedForwardIfLockedAttemptingToFillAkaPurchaseSlot(couchbaseDocument, cas, lcbOpSuccess, dbError);
    }
        break;
    case GETBITCOINKEYSETNCURRENTPAGETOSEEWHATPAGEITISONANDIFITISLOCKED:
    {
        analyzeBitcoinKeySetN_CurrentPageDocToSeeWhatPageItIsOnAndIfItIsLocked(couchbaseDocument, cas, lcbOpSuccess, dbError);
    }
        break;
    case GETBITCOINKEYSETNACTUALPAGETOSEEIFUUIDONITENOUGHROOM:
    {
        gotBitcoinKeySetNpageYSoAnalyzeItForUUIDandEnoughRoomEtc(couchbaseDocument, cas, lcbOpSuccess, dbError);
    }
        break;
    case GETBITCOINKEYSETNPAGEYANDIFITEXISTSLOOPAROUNDCHECKINGUUIDBUTIFNOTEXISTMAKEITEXISTBITCH:
    {
        getBitcoinKeySetNPageYAttemptFinishedSoCheckItIfItExistsAndMakeItIfItDont(couchbaseDocument, cas, lcbOpSuccess, dbError);
    }
        break;
    case GETHUGEBITCOINKEYLISTCURRENTPAGE:
    {
        hugeBitcoinKeyListCurrentPageGetComplete(couchbaseDocument, cas, lcbOpSuccess, dbError);
    }
        break;
    case GETHUGEBITCOINKEYLISTACTUALPAGEFORANALYZINGANDMAYBECLAIMINGKEYRANGE:
    {
        getHugeBitcoinKeyListActualPageAttemptCompleted(couchbaseDocument, cas, lcbOpSuccess, dbError);
    }
        break;
    case CREDITCONFIRMEDBITCOINAMOUNTAFTERANALYZINGUSERACCOUNT:
    {
        creditConfirmedBitcoinAmountAfterAnalyzingUserAccount(couchbaseDocument, cas, lcbOpSuccess, dbError);
    }
        break;
    case ALLADSLOTFILLERSTODETERMINENEXTINDEXANDTOUPDATEITAFTERADDINGAFILLERGETSAVINGCAS:
    {
        determineNextSlotFillerIndexAndThenAddSlotFillerToIt(couchbaseDocument, cas, lcbOpSuccess, dbError);
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
        finishShowingAdvertisingBuyAdSpaceD3faultCampaign0Widget(couchbaseDocument, cas); //TODOreq: calling this from two places now, need to.. err.. fix that
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
        storeLargeAdImageInCouchbaseDbAttemptComplete(keyToCouchbaseDocument, lcbOpSuccess, dbError);
    }
        break;
    case BITCOINKEYSETNPAGEYCREATIONVIALCBADD:
    {
        doneAttemptingBitcoinKeySetNnextPageYcreation(dbError);
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
    case LOGINACCOUNTRECOVERYUNLOCKINGWITHOUTDEBITTINGUSERACCOUNT:
    {
        doneAttemptingUserAccountLockedRecoveryUnlockWithoutDebitting(lcbOpSuccess, dbError);
    }
        break;
    case UNLOCKUSERACCOUNTFROMBITCOINGETTINGKEYTOBITCOINHAVEKEY:
    {
        doneAttemptingToUnlockUserAccountFromBitcoinGettingKeyToBitcoinHaveKey(lcbOpSuccess, dbError);
    }
        break;
    case CLAIMBITCOINKEYONBITCOINKEYSETVIACASSWAP:
    {
        doneAttemptingToUpdateBitcoinKeySetViaCASswapAkaClaimingAKey(lcbOpSuccess, dbError);
    }
        break;
    case HUGEBITCOINKEYLISTKEYRANGECLAIMATTEMPT:
    {
        doneAttemptingHugeBitcoinKeyListKeyRangeClaim(lcbOpSuccess, dbError);
    }
        break;
    case SAFELYUNLOCKBITCOINKEYSETNCURRENTPAGEDOCFAILINGTOLERABLY:
    {
        doneAttemptingToUnlockBitcoinKeySetN_CurrentPage(lcbOpSuccess, dbError);
    }
        break;
    case HUGEBITCOINKEYLISTPAGECHANGE:
    {
        doneChangingHugeBitcoinKeyListCurrentPage(dbError);
    }
        break;
    case RELOCKBITCOINKEYSETN_CURRENTPAGETONONEXISTENTPAGEZASNECESSARYOPTIMIZATION:
    {
        doneReLockingBitcoinKeySetN_CurrentPage_withNewFromPageZvalue(dbError);
    }
        break;
    case CREDITCONFIRMEDBITCOINBALANCEFORCURRENTPAYMENTKEYCASSWAPUSERACCOUNT:
    {
        doneAttemptingCredittingConfirmedBitcoinBalanceForCurrentPaymentKeyCasSwapUserAccount(lcbOpSuccess, dbError);
    }
        break;
    case UPDATEALLADSLOTFILLERSDOCSINCEWEJUSTCREATEDNEWADSLOTFILLER:
    {
        doneAttemptingToUpdateAllAdSlotFillersDocSinceWeJustCreatedANewAdSlotFiller(lcbOpSuccess, dbError);
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
        userAccountBitcoinLockedIntoGettingKeyAttemptComplete(outputCas, lcbOpSuccess, dbError);
    }
        break;
    case LOCKINGBITCOINKEYSETNINTOFILLINGNEXTPAGEMODE:
    {
        attemptToLockBitcoinKeySetNintoFillingNextPageModeComplete(outputCas, lcbOpSuccess, dbError);
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

            getAndSubscribeCouchbaseDocumentByKeySavingCas("adSpaceSlotsd3fault0", GetCouchbaseDocumentByKeyRequest::GetAndSubscribeUnsubscribeMode); //TODOreq: to be future proof for use with other subscriptions i'd have to call a method passing in m_CurrentlySubscribedTo in order to get the key to pass in here (easy but lazy)

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
    //TODOreq: sanitize username at least (my json encoder doesn't like periods, for starters). since password is being b64 encoded we can probably skip sanitization (famous last words?)

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
    jsonDoc.put("balance", "0.0");
    jsonDoc.put("passwordHash", base64PasswordSaltHashed);
    jsonDoc.put("passwordSalt", base64Salt);
    jsonDoc.put("bitcoinState", "NoKey");
    //string'ify json
    std::ostringstream jsonDocBuffer;
    write_json(jsonDocBuffer, jsonDoc, false);
    std::string jsonString = jsonDocBuffer.str();

    store_ADDbyDefault_WithoutInputCasCouchbaseDocumentByKeyBegin("user" + username, jsonString);
    m_WhatTheStoreWithoutInputCasWasFor = REGISTERATTEMPTSTOREWITHOUTINPUTCAS;
}
void AnonymousBitcoinComputingWtGUI::handleLoginButtonClicked()
{
    //TODOreq: sanitize. keep in mind username is part of lots of keys, so maybe make it like 20 characters and keep in mind during key/doc design to make sure you don't make a key that is > 230 characters long (250 is max key length in couchbase)

    //TODOreq: it might make sense to clear the line edit's in the login widget, and to save the login credentials as member variables, and to also clear the password member variable (memset) to zero after it has been compared with the db hash (or even just after it has been hashed (so just make the hash a member xD))
    std::string username = m_LoginUsernameLineEdit->text().toUTF8();
    deferRendering(); //2-layered defer/resume because we want to stay deferred until possible login recovery completes
    getCouchbaseDocumentByKeySavingCasBegin("user" + username);
    m_WhatTheGetSavingCasWasFor = LOGINATTEMPTGET;
}
void AnonymousBitcoinComputingWtGUI::loginIfInputHashedEqualsDbInfo(const std::string &userProfileCouchbaseDocAsJson, u_int64_t casOnlyUsedWhenDoingRecoveryAtLogin, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        //TODOreq: 500 internal server error

        //temp:
        cerr << "loginIfInputHashedEqualsDbInfo db error" << endl;

        resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //if you change the code here in this block, change it in the login recovery code path as well (copy paste job)
        //^^??

        usernameOrPasswordYouProvidedIsIncorrect();
        resumeRendering();
        return;
    }

    //if we get here, the username exists

    ptree pt;
    std::istringstream is(userProfileCouchbaseDocAsJson);
    read_json(is, pt);

    std::string passwordHashInBase64FromDb = pt.get<std::string>("passwordHash");
    std::string passwordSaltInBase64FromDb = pt.get<std::string>("passwordSalt");

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
            getAndSubscribeCouchbaseDocumentByKeySavingCas("adSpaceSlotsd3fault0", GetCouchbaseDocumentByKeyRequest::GetAndSubscribeChangeSessionIdMode); //TODOreq: see handleInternalPath's comment about making key dynamic (ez)
        }

        m_CurrentlyLoggedInUsername = m_LoginUsernameLineEdit->text().toUTF8(); //TODOreq(depends on how much security deferRending gives me): do we need to keep rendering deferred until we capture this (or perhaps capture it earlier (when it's used to LCB_GET)?)? Maybe they could 'change usernames' at precise moment to give them access to account that isn't theirs. idk [wt] tbh, but sounds plausible. should be captured as m_UsernameToAttemptToLoginAs (because this one is reserved for post-login), and then use simple assignment HERE

        //TODOreq: do this on key not found fail (and all other relevant errors) also (if only there was a generator that let you see if there was any kind of error and then let you drill down).
        m_LoginUsernameLineEdit->setText("");
        m_LoginPasswordLineEdit->setText("");

        std::string slotToAttemptToFillAkaPurchase_ACCOUNT_LOCKED_TEST = pt.get<std::string>("slotToAttemptToFillAkaPurchase", "n");
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
            m_AccountLockedRecoveryWhatTheUserWasTryingToFillTheSlotWithHack = pt.get<std::string>("slotToAttemptToFillAkaPurchaseItWith"); //need this after the GET for comparing
            getCouchbaseDocumentByKeyBegin(slotToAttemptToFillAkaPurchase_ACCOUNT_LOCKED_TEST);
            m_WhatTheGetWasFor = ONLOGINACCOUNTLOCKEDRECOVERYDOESSLOTEXISTCHECK;
            m_UserAccountLockedJsonToMaybeUseInAccountRecoveryAtLogin = userProfileCouchbaseDocAsJson;
            m_CasFromUserAccountLockedAndStuckLockedButErrRecordedDuringRecoveryProcessAfterLoginOrSomethingLoLWutIamHighButActuallyNotNeedMoneyToGetHighGuhLifeLoLSoErrLemmeTellYouAboutMyDay = casOnlyUsedWhenDoingRecoveryAtLogin;
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
void AnonymousBitcoinComputingWtGUI::usernameOrPasswordYouProvidedIsIncorrect()
{
    new WBreak(m_LoginWidget);
    new WText("The username or password you provided is incorrect", m_LoginWidget); //TODOreq: don't add multiple because it'll look retarded, BUT if we only have one how do we tell them that it failed the second time? 'highlighting' animation makes sense, as does clearing the message when they start typing again. also TODOreq: make message go away after user logs out (failed login attempt -> successful login attempt -> logout)
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
    new WText("Hello, " + m_CurrentlyLoggedInUsername, m_LogoutWidget);
    WPushButton *logoutButton = new WPushButton("Log Out", m_LogoutWidget);
    logoutButton->clicked().connect(this, &AnonymousBitcoinComputingWtGUI::handleLogoutButtonClicked);
    m_LoginLogoutStackWidget->setCurrentWidget(m_LogoutWidget);

    if(m_LinkToAccount)
    {
        m_LinksVLayout->removeWidget(m_LinkToAccount);
        delete m_LinkToAccount;
    }
    m_LinkToAccount = new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ACCOUNT), ABC_ANCHOR_TEXTS_ACCOUNT);
    m_LinksVLayout->addWidget(m_LinkToAccount);

    m_LoggedIn = true;
    resumeRendering();
}
void AnonymousBitcoinComputingWtGUI::handleLogoutButtonClicked()
{
    if(!m_LoggedIn)
        return;

    if(m_AccountWidget)
    {
        delete m_AccountWidget;
        m_AccountWidget = 0;
    }
    if(m_LinkToAccount) //probably not necessary since it's only ever created when we're logged in
    {
        delete m_LinkToAccount;
        m_LinkToAccount = 0;
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
