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
#define ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE_D3FAULT_CAMPAIGN_0 "Campaign 0"

AnonymousBitcoinComputingWtGUI::AnonymousBitcoinComputingWtGUI(const WEnvironment &myEnv)
    : WApplication(myEnv), m_HeaderHlayout(new WHBoxLayout()), m_MainVLayout(new WVBoxLayout(root())), m_LoginLogoutStackWidget(new WStackedWidget()), m_LoginWidget(new WContainerWidget(m_LoginLogoutStackWidget)), m_LoginUsernameLineEdit(0), m_LoginPasswordLineEdit(0), m_LogoutWidget(0), m_MainStack(new WStackedWidget()), m_HomeWidget(0), m_AdvertisingWidget(0), m_AdvertisingBuyAdSpaceWidget(0), m_RegisterWidget(0), /*m_RegisterSuccessfulWidget(0),*/ m_AdvertisingBuyAdSpaceD3faultWidget(0), m_AdvertisingBuyAdSpaceD3faultCampaign0Widget(0), m_AddMessageQueuesRandomIntDistribution(0, NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES - 1), m_GetMessageQueuesRandomIntDistribution(0, NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES - 1), m_WhatTheGetWasFor(INITIALINVALIDNULLGET), m_LoggedIn(false)
{
    m_RandomNumberGenerator.seed((int)rawUniqueId());
    m_CurrentAddMessageQueueIndex = m_AddMessageQueuesRandomIntDistribution(m_RandomNumberGenerator);
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
//outputSha1 must be char output[20] (passed in by reference) or else we'll go out of bounds :)
std::string AnonymousBitcoinComputingWtGUI::sha1string(const string &inputString)
{
    const void *inputStringBuffer = (const void*)inputString.c_str();

    boost::uuids::detail::sha1 sha1er;
    sha1er.process_bytes(inputStringBuffer, inputString.length()); //password

    unsigned int outputHashUintArray[5];
    sha1er.get_digest(outputHashUintArray);

    char outputHashCharArray[20];
    for(int i = 0; i < 5; ++i)
    {
        const char *tmp = reinterpret_cast<char*>(outputHashUintArray);
        outputHashCharArray[i*4] = tmp[i*4+3];
        outputHashCharArray[i*4+1] = tmp[i*4+2];
        outputHashCharArray[i*4+2] = tmp[i*4+1];
        outputHashCharArray[i*4+3] = tmp[i*4];
    }
    return std::string(outputHashCharArray);
}
string AnonymousBitcoinComputingWtGUI::toBase64(const string &inputString)
{
    unsigned int base64PadCharCount = (3-inputString.length()%3)%3;
    std::string ret(BinaryToBase64BoostTypedef(inputString.begin()), BinaryToBase64BoostTypedef(inputString.end())); //wtf is this doing? why do i pass in the beginning and end into the b64 function being called twice??? It makes sense, yea, if you suck at designing APIs. TODOreq: OH SHIT DOESN'T THAT MEAN THAT IT USES INTERNAL STRUCTURE/STATE AND IS THEREFORE NOT THREAD SAFE FFFFFFFFFFFFFFFFFFFF oh well just mutex is needed so fuck it (woo hoo scalable as fuck distributed database.... except stupid ass fucking code requiring mutexes making it [almost] irrelevant (extreme exaggeration and not really true, but still)
    ret.append(base64PadCharCount, '=');
    return ret; //the thing i hate about C++ is wondering whether or not returning a value works, is wastefully 'copying', returns a reference to a now dead copy, so on and so forth. pointers solve this, but then i feel like i'm needlessly deref'ing all the damn time. also qt's implicit sharing mechanism backfires if you ever try to modify the value being implicitly shared (and you DON'T want a copy to be made)
}
void AnonymousBitcoinComputingWtGUI::buildGui()
{
    setTitle("Anonymous Bitcoin Computing");
    WAnimation slideInFromBottom(WAnimation::SlideInFromBottom, WAnimation::EaseOut, 250); //If this took any longer than 2 lines of code (<3 Wt), I wouldn't do it
    m_MainStack->setTransitionAnimation(slideInFromBottom, true);

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
    m_MainVLayout->addWidget(m_MainStack, 0, Wt::AlignTop | Wt::AlignLeft);
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
void AnonymousBitcoinComputingWtGUI::showAdvertisingBuyAdSpaceD3faultCampaign0Widget()
{
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
        beginGetCouchbaseDocumentByKey("adsBuyAdSpaceD3fault0");
        m_WhatTheGetWasFor = HACKEDIND3FAULTCAMPAIGN0GET;
    }
    m_MainStack->setCurrentWidget(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
}
void AnonymousBitcoinComputingWtGUI::beginGetCouchbaseDocumentByKey(const std::string &keyToCouchbaseDocument)
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
void AnonymousBitcoinComputingWtGUI::beginAddCouchbaseDocumentByKey(const string &keyToCouchbaseDocument, const string &couchbaseDocument)
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
            new WText(WString(couchbaseDocument), m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        }
        break;
        case LOGINATTEMPTGET:
        {
            loginIfInputHashedEqualsDbInfo(couchbaseDocument);
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
                    showAdvertisingBuyAdSpaceD3faultCampaign0Widget();
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
    std::string salt = sha1string(username + uniqueId() + "saltplx");
    //base64 salt for storage in json/couchbase
    std::string base64Salt = toBase64(salt);
    //hash password using base64'd salt
    std::string passwordSaltHashed = sha1string(passwordPlainText + base64Salt);
    //base64 hash for storage in json/couchbase
    std::string base64PasswordSaltHashed = toBase64(passwordSaltHashed);
    //json'ify
    ptree jsonDoc;
    jsonDoc.put("passwordHash", base64PasswordSaltHashed);
    jsonDoc.put("passwordSalt", base64Salt);
    jsonDoc.put("balance", "0.0");
    //string'ify json
    std::ostringstream jsonDocBuffer;
    write_json(jsonDocBuffer, jsonDoc, false);
    std::string jsonString = jsonDocBuffer.str();

    beginAddCouchbaseDocumentByKey("user" + username, jsonString);
    m_WhatTheAddWasFor = REGISTERATTEMPTADD;
}
void AnonymousBitcoinComputingWtGUI::handleLoginButtonClicked()
{
    //TODOreq: sanitize. keep in mind username is part of lots of keys, so maybe make it like 20 characters and keep in mind during key/doc design to make sure you don't make a key that is > 230 characters long (250 is max key length in couchbase)

    //TODOreq: it might make sense to clear the line edit's in the login widget, and to save the login credentials as member variables, and to also clear the password member variable (memset) to zero after it has been compared with the db hash (or even just after it has been hashed (so just make the hash a member xD))
    std::string username = m_LoginUsernameLineEdit->text().toUTF8();
    beginGetCouchbaseDocumentByKey("user" + username);
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
    std::string passwordHashFromUserInput = sha1string(passwordFromUserInput + passwordSaltInBase64FromDb);

    //hmm i COULD implement fromBase64, but why not just toBase64 the user input instead? saves me a tiny bit of effort [for now :-P]
    std::string passwordHashBase64FromUserInput = toBase64(passwordHashFromUserInput);

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
