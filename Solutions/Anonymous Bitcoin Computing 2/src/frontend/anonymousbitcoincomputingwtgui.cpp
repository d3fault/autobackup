#include "anonymousbitcoincomputingwtgui.h"

#include "../GetCouchbaseDocumentByKeyRequest.h"

//internal paths
#define ABC_INTERNAL_PATH_HOME "/home" //if home has any "sub" paths, then our hack to handle clean urls logic will break (should use internalPathMatches instead)
#define ABC_INTERNAL_PATH_ADS "/advertising"
#define ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE ABC_INTERNAL_PATH_ADS \
                                    "/buy-ad-space"
//hardcoded internal paths, would be dynamic ideally
#define ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE_D3FAULT ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE \
                                        "/d3fault"
#define ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE_D3FAULT_CAMPAIGN_0 ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE \
                                            "/0"

#define ABC_ANCHOR_TEXTS_PATH_HOME "Home"
#define ABC_ANCHOR_TEXTS_PATH_ADS "Advertising"
#define ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE "Buy Ad Space"
#define ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE_D3FAULT "d3fault"
#define ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE_D3FAULT_CAMPAIGN_0 "Campaign 0"

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
        new WText("You Are Here: " + WString(ABC_ANCHOR_TEXTS_PATH_ADS), m_AdvertisingWidget);
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
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS), ABC_ANCHOR_TEXTS_PATH_ADS, m_AdvertisingBuyAdSpaceWidget);
        new WBreak(m_AdvertisingBuyAdSpaceWidget);
        new WText("You Are Here: " + WString(ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE), m_AdvertisingBuyAdSpaceWidget);
        new WBreak(m_AdvertisingBuyAdSpaceWidget);
        new WBreak(m_AdvertisingBuyAdSpaceWidget);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE_D3FAULT), ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE_D3FAULT, m_AdvertisingBuyAdSpaceWidget);
    }
    m_MainStack->setCurrentWidget(m_AdvertisingBuyAdSpaceWidget);
}
void AnonymousBitcoinComputingWtGUI::showAdvertisingBuyAdSpaceD3faultWidget()
{
    if(!m_AdvertisingBuyAdSpaceD3faultWidget)
    {
        m_AdvertisingBuyAdSpaceD3faultWidget = new WContainerWidget(m_MainStack);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_HOME), ABC_ANCHOR_TEXTS_PATH_HOME, m_AdvertisingBuyAdSpaceD3faultWidget);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS), ABC_ANCHOR_TEXTS_PATH_ADS, m_AdvertisingBuyAdSpaceD3faultWidget);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE), ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE, m_AdvertisingBuyAdSpaceD3faultWidget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultWidget);
        new WText("You Are Here: " + WString(ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE_D3FAULT), m_AdvertisingBuyAdSpaceD3faultWidget);
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
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS), ABC_ANCHOR_TEXTS_PATH_ADS, m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE), ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE, m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE_D3FAULT), ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE_D3FAULT, m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WText("You Are Here: " + WString(ABC_ANCHOR_TEXTS_PATH_ADS_BUY_AD_SPACE_D3FAULT_CAMPAIGN_0), m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        new WBreak(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);

        //still dunno if i should block or do it async? instincts tell me async, but "SUB MILLISECOND LATENCY" tells me async might actually be wasteful/slower??? The obvious answer is "benchmark it xD" (FUCK FUCK FUCK FUCK FUCK THAT, async it is (because even if slower, it still allows us to scale bettarer))
        beginGetCouchbaseDocumentByKey("adsBuyAdSpaceD3fault0");
        deferRendering(); //does calling this before m_MainStack->setCurrentWidget fuck shit up? I'd imagine not...
    }
    m_MainStack->setCurrentWidget(m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
}
void AnonymousBitcoinComputingWtGUI::beginGetCouchbaseDocumentByKey(const std::string &keyToCouchbaseDocument)
{
    std::ostringstream getCouchbaseDocumentByKeyRequestSerialized;
    GetCouchbaseDocumentByKeyRequest getCouchbaseDocumentByKeyRequest(sessionId(), this, keyToCouchbaseDocument);

    {
        boost::archive::text_oarchive serializer(getCouchbaseDocumentByKeyRequestSerialized);
        serializer << getCouchbaseDocumentByKeyRequest;
    }

    std::string getCouchbaseDocumentByKeyRequesSerializedString = getCouchbaseDocumentByKeyRequestSerialized.str();

    size_t getCouchbaseDocumentByKeyRequesSerializedStringLength = getCouchbaseDocumentByKeyRequesSerializedString.length();
    if(getCouchbaseDocumentByKeyRequesSerializedStringLength > WT_TO_COUCHBASE_GET_MAX_MESSAGE_SIZE) //assumes 1 character is 1 byte... i think this is right because it worked down below for our c_str() passed in, which i'm pretty damn sure is 1ch=1by
    {
        //TODOreq: handle serialized buffer over maximum message size properly
        return;
    }

    ++m_CurrentGetMessageQueueIndex;
    if(m_CurrentGetMessageQueueIndex == NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES)
    {
        m_CurrentGetMessageQueueIndex = 0;
    }
    unsigned short lockedMutexIndex = m_CurrentGetMessageQueueIndex;
    unsigned short veryLastMutexIndexToBlockLock = (m_CurrentGetMessageQueueIndex == 0 ? (NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES-1) : (m_CurrentGetMessageQueueIndex-1));
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

    m_GetWtMessageQueues[lockedMutexIndex]->send(getCouchbaseDocumentByKeyRequesSerializedString.c_str(), getCouchbaseDocumentByKeyRequesSerializedStringLength, 0);

    event_active(m_GetEventCallbacksForWt[lockedMutexIndex], EV_READ|EV_WRITE, 0);
    m_GetMutexArray[lockedMutexIndex].unlock();
}
void AnonymousBitcoinComputingWtGUI::getCouchbaseDocumentByKeyFinished(const std::string &keyToCouchbaseDocument, const std::string &couchbaseDocument)
{
    //this hack makes me giggle like a little school girl, tee hee
    if(keyToCouchbaseDocument == "adsBuyAdSpaceD3fault0")
    {
        resumeRendering();
        new WText(WString(couchbaseDocument), m_AdvertisingBuyAdSpaceD3faultCampaign0Widget);
        if(environment().ajax())
        {
            triggerUpdate(); //this and enableUpdates are probably not needed if i'm always defer/resuming
        }
    }
}
void AnonymousBitcoinComputingWtGUI::handleInternalPathChanged(const std::string &newInternalPath)
{
    if(newInternalPath == "" || newInternalPath == "/" || internalPathMatches(ABC_INTERNAL_PATH_HOME))
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
}
void AnonymousBitcoinComputingWtGUI::handleLoggedIn()
{
    changeSessionId();
}
AnonymousBitcoinComputingWtGUI::AnonymousBitcoinComputingWtGUI(const WEnvironment &myEnv)
    : WApplication(myEnv), m_MainStack(new WStackedWidget()), m_HomeWidget(0), m_AddMessageQueuesRandomIntDistribution(0, NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES - 1), m_GetMessageQueuesRandomIntDistribution(0, NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES - 1)
{
    setTitle("Anonymous Bitcoin Computing");
    WAnimation slideInFromBottom(WAnimation::SlideInFromBottom, WAnimation::EaseOut, 250); //If this took any longer than 2 lines of code (<3 Wt), I wouldn't do it
    m_MainStack->setTransitionAnimation(slideInFromBottom, true);

    internalPathChanged().connect(this, &AnonymousBitcoinComputingWtGUI::handleInternalPathChanged);

    //hack to handle clean urls, idk why wt doesn't do this for us...
    const std::string &cleanUrlInternalPath = internalPath();
    if(cleanUrlInternalPath != "" && cleanUrlInternalPath != "/" && cleanUrlInternalPath != ABC_INTERNAL_PATH_HOME)
    {
        handleInternalPathChanged(cleanUrlInternalPath);
    }
    else
    {
        showHomeWidget();
    }

    m_RandomNumberGenerator.seed((int)rawUniqueId());
    m_CurrentAddMessageQueueIndex = m_AddMessageQueuesRandomIntDistribution(m_RandomNumberGenerator);
    m_CurrentGetMessageQueueIndex = m_GetMessageQueuesRandomIntDistribution(m_RandomNumberGenerator);

    if(myEnv.ajax())
    {
        enableUpdates(true);
    }
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
