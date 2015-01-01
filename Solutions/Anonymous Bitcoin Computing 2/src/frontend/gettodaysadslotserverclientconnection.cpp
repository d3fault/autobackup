#include "gettodaysadslotserverclientconnection.h"

#include <QtNetwork/QTcpSocket>
#include <QUrl>
#include <QUrlQuery>
#include <QDateTime>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "abc2couchbaseandjsonkeydefines.h"
#include "validatorsandinputfilters/lettersnumbersonlyregexpvalidatorandinputfilter.h"
#include "../frontend2backendRequests/getcouchbasedocumentbykeyrequest.h"
#include "../frontend2backendRequests/storecouchbasedocumentbykeyrequest.h"

#define GetTodaysAdSlotServerClientConnection_FIRST_NO_CACHE_YET_SLOT_INDEX "0"
#define GetTodaysAdSlotServerClientConnection_SECONDS_BEFORE_ACTUAL_EXPIRE_DATETIME_TO_SERVE_THE_NEXT_AD_SLOT 30
#define GetTodaysAdSlotServerClientConnection_INVALID_API_KEY_MESSAGE "Invalid API Key"
#define GetTodaysAdSlotServerClientConnection_MAX_API_REQUESTS_PER_AD_SLOT_DURATION_EXCLUDING_NO_AD_RESPONSES_OFC 50 /* if changing, change below string as well. 50 seems ample: low enough so that it would trigger if the ad campaign owner was somehow hitting the API url on every one of their website visitors (and yet still parsing json? unlikely lolol), yet high enough that a developer dicking around with the API won't run out of test runs. TO DOnereq: exclude 'no ad' responses! there is no 'push' atm, so 5 min polling has to be used :( TODOoptimization: a user should be able to query the status of ALL their ad campaigns with a single api query (more efficient on both ends) */
#define GetTodaysAdSlotServerClientConnection_MAX_API_REQUESTS_PER_AD_SLOT_DURATION_EXCLUDING_NO_AD_RESPONSES_OFC_STR "50"

using namespace boost::property_tree;

//TODOoptional: optional, but a damn good idea: provide a javascript snippet and a corresponding php script. The javascript snippet calls the php script, parses the json results, and embeds the image and hover etc. The php script itself is responsible for caching (it would be kinda inefficient because two simultaneous hits have the potential of triggering two API requests (I guess I'd rely on the atomicity of file creation -- but then wat do if the atomic open-for-writing (known temp filename) fails? there's no way to 'let me know when the api request finishes'. man php fucking blows)). The php script does the api request when needed (and can do LAZY (more efficient!) polling: only when a user visits the page the ad is displayed on (or when polling even matters: the ad is NOT displayed on xD)
//^^ADDITIONALLY, cron job php/shell/etc that run a few seconds in advance (~10), and corresponding php to set up the <img><hover> -- this one is for php devs (since they'd be calling a php function), the above one is for website noobs (they embed the js (gl) and paste the php file)
//^^^And for 1337 d00dz, I'll give a Qt impl (already have one in hvbs). If I'm bored enough, a boost impl...
AbcApiGetCouchbaseDocumentByKeyRequestResponder GetTodaysAdSlotServerClientConnection::s_GetResponder;
AbcApiStoreCouchbaseDocumentByKeyRequestResponder GetTodaysAdSlotServerClientConnection::s_StoreResponder;
queue<GetCouchbaseDocumentByKeyRequest*> *GetTodaysAdSlotServerClientConnection::s_BackendGetQueue = NULL;
queue<StoreCouchbaseDocumentByKeyRequest*> *GetTodaysAdSlotServerClientConnection::s_BackendStoreQueue = NULL;
struct event *GetTodaysAdSlotServerClientConnection::s_BackendGetQueueEvent = NULL;
struct event *GetTodaysAdSlotServerClientConnection::s_BackendStoreQueueEvent = NULL;

void GetTodaysAdSlotServerClientConnection::setBackendGetQueue(queue<GetCouchbaseDocumentByKeyRequest *> *backendQueue)
{
    s_BackendGetQueue = backendQueue;
}
void GetTodaysAdSlotServerClientConnection::setBackendStoreQueue(queue<StoreCouchbaseDocumentByKeyRequest *> *backendQueue)
{
    s_BackendStoreQueue = backendQueue;
}
void GetTodaysAdSlotServerClientConnection::setBackendGetQueueEvent(struct event *backendQueueEvent)
{
    s_BackendGetQueueEvent = backendQueueEvent;
}
void GetTodaysAdSlotServerClientConnection::setBackendStoreQueueEvent(struct event *backendQueueEvent)
{
    s_BackendStoreQueueEvent = backendQueueEvent;
}
GetTodaysAdSlotServerClientConnection::GetTodaysAdSlotServerClientConnection(QTcpSocket *clientSocket, QObject *parent)
    : QObject(parent)
    , m_ClientStream(clientSocket)
    , m_Stage(GetAdCampaignCurrentSlotCacheIfExists)
    , m_BackendRequestPending(false)
    , m_WantsToBeDeletedLater(false)
{
    m_ClientStream.setAutoDetectUnicode(true);
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(handleClientReadyRead()));
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(deleteLaterIfNoBackendRequestPendingOrWhenThatBackendRequestFinishes()));
    connect(clientSocket, SIGNAL(destroyed()), this, SLOT(deleteLater())); //hack because when setting the tcp socket as parent of 'this', it won't compile... complains it isn't a qobject. wtf? i tested in a barebones app and it compiled just fine...
}
void GetTodaysAdSlotServerClientConnection::beginDbGet(const string &keyToGet, GetTodaysAdSlotServerClientConnection::GetTodaysAdSlotServerClientConnectionStageEnum stageToSet)
{
    GetCouchbaseDocumentByKeyRequest *getCouchbaseRequest = new GetCouchbaseDocumentByKeyRequest(&s_GetResponder, "", this, keyToGet);
    while(!s_BackendGetQueue->push(getCouchbaseRequest)) ;
    event_active(s_BackendGetQueueEvent, EV_READ|EV_WRITE, 0);
    m_BackendRequestPending = true;
    m_Stage = stageToSet;
}
void GetTodaysAdSlotServerClientConnection::beginDbStore(const string &keyToStore, const string &documentToStore, GetTodaysAdSlotServerClientConnection::GetTodaysAdSlotServerClientConnectionStageEnum stageToSet)
{
    StoreCouchbaseDocumentByKeyRequest *storeCouchbaseRequest = new StoreCouchbaseDocumentByKeyRequest(&s_StoreResponder, "", this, keyToStore, documentToStore, StoreCouchbaseDocumentByKeyRequest::SetNoCasMode);
    while(!s_BackendStoreQueue->push(storeCouchbaseRequest)) ;
    event_active(s_BackendStoreQueueEvent, EV_READ|EV_WRITE, 0);
    m_BackendRequestPending = true;
    m_Stage = stageToSet;
}
void GetTodaysAdSlotServerClientConnection::sendResponseAndCloseSocket(const QString &response)
{
    m_ClientStream << "HTTP/1.0 200 OK\r\nContent-Type: application/json\r\nConnection: close\r\nContent-Length: " << QString::number(response.length()) << "\r\n"
                      << "\r\n"
                      << response;
    m_ClientStream.device()->close();
}
void GetTodaysAdSlotServerClientConnection::continueAtJustFinishedAttemptingToGetAdCampaignCurrentSlotCache(const std::string &couchbaseDocument, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        sendResponseAndCloseSocket(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_NO_AD_PLACEHOLDER);
        return;
    }

    m_FirstSlotIndexToTry = GetTodaysAdSlotServerClientConnection_FIRST_NO_CACHE_YET_SLOT_INDEX;
    m_FoundAdCampaignCurrentSlotCacheDoc = lcbOpSuccess;
    if(!lcbOpSuccess)
    {
         //the campaign slot index cache does not exist. get the campaign doc itself, JUST for slotLengthHours (from then on we store it in the cache doc)
        m_NumApiRequestsForThisAdSlotSoFar = 1; //this will have been our first, which is what we want to store in the cache doc later
        beginDbGet(adSpaceCampaignKey(m_CampaignOwnerRequested, m_CampaignIndexRequested), GetAdCampaignDocItselfToGetSlotLengthHours);
        return;
    }
    else
    {
        //the campaign slot index cache exists
        ptree pt;
        std::istringstream is(couchbaseDocument);
        read_json(is, pt);

        m_NumApiRequestsForThisAdSlotSoFar = pt.get<int>(JSON_AD_SPACE_CAMPAIGN_SLOT_CACHE_NUM_API_REQUESTS_FOR_CURRENT_SLOT);
#if 0 //TO DOnereq: do this LATER. yes we have the num api requests value at this point, but we don't know whether the 'currentSlot' the cache points to is todays or not [yet]
        int apiRequestsForThisAdSlotSoFar = pt.get<int>(JSON_AD_SPACE_CAMPAIGN_SLOT_CACHE_NUM_API_REQUESTS_FOR_CURRENT_SLOT);
        if(apiRequestsForThisAdSlotSoFar > GetTodaysAdSlotServerClientConnection_MAX_API_REQUESTS_PER_AD_SLOT_DURATION_EXCLUDING_NO_AD_RESPONSES_OFC)
        { }
#endif

        m_FirstSlotIndexToTry = pt.get<string>(JSON_AD_SPACE_CAMPAIGN_SLOT_CACHE_CURRENT_SLOT);
        m_SlotLengthHours = pt.get<int>(JSON_AD_SPACE_CAMPAIGN_SLOT_LENGTH_HOURS);
        m_TheCorretApiKey = pt.get<string>(JSON_USER_ACCOUNT_API_KEY);
        haveSlotLengthHoursAndApiKeyAndKnowFirstSlotIndexToTrySoTry();
        return;
    }
}
void GetTodaysAdSlotServerClientConnection::continueAtJustFinishedAttemptingToGetAdCampaign(const std::string &couchbaseDocument, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        sendResponseAndCloseSocket(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_NO_AD_PLACEHOLDER);
        return;
    }
    if(!lcbOpSuccess)
    {
        sendResponseAndCloseSocket(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_CUSTOM_ERROR("There is no ad campaign at that index, or that is an invalid user"));
        return;
    }

    //got campaign doc

    ptree pt;
    std::istringstream is(couchbaseDocument);
    read_json(is, pt);
    m_SlotLengthHours = pt.get<int>(JSON_AD_SPACE_CAMPAIGN_SLOT_LENGTH_HOURS);

    //oops, it's in profile not campaign doc xD. m_TheCorretApiKey = pt.get<string>(JSON_USER_ACCOUNT_API_KEY); //TODOoptional: _COULD_ store it in campaign, but too lazy to implement that
    beginDbGet(userAccountKey(m_CampaignOwnerRequested), GetAdCampaignOwnersProfileJustToGetApiKey);
    return;

    //oops, no api key yet! haveSlotLengthHoursAndApiKeyAndKnowFirstSlotIndexToTrySoTry();
}
void GetTodaysAdSlotServerClientConnection::continueAtJustFinishedAttemptingToGetCampaignOwnersProfileDoc(const string &couchbaseDocument, bool lcbOpSuccess, bool dbError)
{
    if(dbError || !lcbOpSuccess) //TODOoptional: lcb op fail might mean total system failure, but it might just mean they requested a user that doesn't exist. idgaf atm
    {
        sendResponseAndCloseSocket(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_NO_AD_PLACEHOLDER);
        return;
    }

    //got seller's profile doc

    ptree pt;
    std::istringstream is(couchbaseDocument);
    read_json(is, pt);

    m_TheCorretApiKey = pt.get<string>(JSON_USER_ACCOUNT_API_KEY);
    haveSlotLengthHoursAndApiKeyAndKnowFirstSlotIndexToTrySoTry(); //first slot index still at initialized value of "0"
}
void GetTodaysAdSlotServerClientConnection::haveSlotLengthHoursAndApiKeyAndKnowFirstSlotIndexToTrySoTry()
{
    if(m_ApiKeyRequested != m_TheCorretApiKey)
    {
        sendResponseAndCloseSocket(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_CUSTOM_ERROR(GetTodaysAdSlotServerClientConnection_INVALID_API_KEY_MESSAGE));
        return;
    }
    m_CurrentSlotIndexToTry = m_FirstSlotIndexToTry;
    tryToGetCurrentSlotIndex();
}
void GetTodaysAdSlotServerClientConnection::tryToGetCurrentSlotIndex()
{
    beginDbGet(adSpaceCampaignSlotKey(m_CampaignOwnerRequested, m_CampaignIndexRequested, m_CurrentSlotIndexToTry), GetAdCampaignSlot);
}
void GetTodaysAdSlotServerClientConnection::continueAtJustFinishedAttemptingToGetAdCampaignSlot(const string &couchbaseDocument, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        sendResponseAndCloseSocket(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_NO_AD_PLACEHOLDER);
        return;
    }
    if(!lcbOpSuccess)
    {
        sendResponseAndCloseSocket(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_CUSTOM_ERROR("Todays ad slot was not purchased")); //TODOoptional: other similar more descriptive error
        return;
    }

    //slot at that index exists

    //is it the current datetime range? could be expired, in which case we +1 slot index and try again
    ptree pt;
    std::istringstream is(couchbaseDocument);
    read_json(is, pt);

    //my dick is long long
    long long startDateTime = boost::lexical_cast<long long>(pt.get<string>(JSON_AD_SPACE_CAMPAIGN_SLOT_START_TIMESTAMP));
    m_CurrentAdSlotExpireDateTime = (startDateTime+(3600*m_SlotLengthHours));
    long long currentDateTime = static_cast<long long>(QDateTime::currentMSecsSinceEpoch()/1000) + GetTodaysAdSlotServerClientConnection_SECONDS_BEFORE_ACTUAL_EXPIRE_DATETIME_TO_SERVE_THE_NEXT_AD_SLOT;
    if(currentDateTime >= startDateTime && (currentDateTime < m_CurrentAdSlotExpireDateTime))
    {
        //woot, found todays slot :)

        //first, check they haven't used up all their api requests for the current slot
        if(m_FoundAdCampaignCurrentSlotCacheDoc && (m_FirstSlotIndexToTry == m_CurrentSlotIndexToTry))
        {
            ++m_NumApiRequestsForThisAdSlotSoFar;
            if(m_NumApiRequestsForThisAdSlotSoFar > GetTodaysAdSlotServerClientConnection_MAX_API_REQUESTS_PER_AD_SLOT_DURATION_EXCLUDING_NO_AD_RESPONSES_OFC)
            {
                //all api requests exhausted
                sendResponseAndCloseSocket(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_CUSTOM_ERROR("You have used up all of your API requests for the current ad slot duration. Wait until the current ad expires and you'll get " GetTodaysAdSlotServerClientConnection_MAX_API_REQUESTS_PER_AD_SLOT_DURATION_EXCLUDING_NO_AD_RESPONSES_OFC_STR " more. You should have saved the results of the API Request. " ABC_HUMAN_READABLE_NAME_PLX " does not serve ads directly to end users (doing so is an invasion of your users' privacy)."));
                return;
            }
        }

        //now we look up the ad slot filler to get the hover/url/image
        beginDbGet(pt.get<string>(JSON_AD_SPACE_CAMPAIGN_SLOT_FILLED_WITH), GetAdCampaignSlotFiller);
        return;
    }
    else
    {
        //expired, so +1 and loop back around
        m_CurrentSlotIndexToTry = boost::lexical_cast<std::string>(boost::lexical_cast<int>(m_CurrentSlotIndexToTry)+1);
        tryToGetCurrentSlotIndex();
        return;
    }

}
void GetTodaysAdSlotServerClientConnection::continueAtJustFinishedAttemptingToGetAdCampaignSlotFiller(const string &couchbaseDocument, bool lcbOpSuccess, bool dbError)
{
    if(dbError || !lcbOpSuccess) //TODOoptional: lcb op fail is TOTAL SYSTEM FAILURE, ad slot filler should always exist
    {
        sendResponseAndCloseSocket(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_NO_AD_PLACEHOLDER);
        return;
    }

    //have ad slot filler

    ptree pt;
    std::istringstream is2(couchbaseDocument);
    read_json(is2, pt);

    ptree todaysFilledAdSlot;
    todaysFilledAdSlot.put(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_ERROR_KEY, JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_ERROR_NOERROR_VALUE);
    todaysFilledAdSlot.put(JSON_SLOT_FILLER_URL, pt.get<string>(JSON_SLOT_FILLER_URL));
    todaysFilledAdSlot.put(JSON_SLOT_FILLER_HOVERTEXT, pt.get<string>(JSON_SLOT_FILLER_HOVERTEXT)); //TODOoptional: using json keys in two different docs, fuck it. same with slot length hours
    todaysFilledAdSlot.put(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_EXPIRATION_DATETIME, boost::lexical_cast<string>(m_CurrentAdSlotExpireDateTime));
    todaysFilledAdSlot.put(JSON_SLOT_FILLER_IMAGE_GUESSED_EXTENSION, pt.get<string>(JSON_SLOT_FILLER_IMAGE_GUESSED_EXTENSION));
    todaysFilledAdSlot.put(JSON_SLOT_FILLER_IMAGEB64, pt.get<string>(JSON_SLOT_FILLER_IMAGEB64));
    std::ostringstream todaysAdSlotJsonBuffer;
    write_json(todaysAdSlotJsonBuffer, todaysFilledAdSlot, false); //TODOoptional: we could format it as xml if the user requests it in the query. ptree already has an xml output so it'd be really freakin' easy...
    const std::string &todaysAdSlotJsonStdStr = todaysAdSlotJsonBuffer.str();
    QString todaysAdSlotJson = QString::fromStdString(todaysAdSlotJsonStdStr);

    sendResponseAndCloseSocket(todaysAdSlotJson);

    //store the current slot index in the cache so subsequent api requests, and also 'tomorrows' ad slot, will be uber fast
#if 0 //OLD (now that there's an API key, we have to update the cache doc on every hit to keep track of their num api requests):
    if(m_CurrentSlotIndexToTry != m_FirstSlotIndexToTry || (m_CurrentSlotIndexToTry == GetTodaysAdSlotServerClientConnection_FIRST_NO_CACHE_YET_SLOT_INDEX && !m_FoundAdCampaignCurrentSlotCacheDoc)) //only update the cache if it needs to be updated...
    {
#endif
        //TODOoptimization: makes sense to put the expiration date time in the cache doc too, so we don't have to get it to check if it's expired
        ptree pt2;
        //pt3.put(JSON_AD_SPACE_CAMPAIGN_SLOT_CACHE_CURRENT_SLOT, boost::lexical_cast<string>(boost::lexical_cast<int>(slotIndexToTry)+1));
        pt2.put(JSON_AD_SPACE_CAMPAIGN_SLOT_CACHE_CURRENT_SLOT, m_CurrentSlotIndexToTry); //^if we only request once per day then it makes sense to set the cache to "tomorrow" (+1), but if we request it multiple times per day then it makes sense to set it to "today" (no +1). leaving as "today" since it's safer and I still haven't finalized this shit
        pt2.put(JSON_AD_SPACE_CAMPAIGN_SLOT_LENGTH_HOURS, boost::lexical_cast<std::string>(m_SlotLengthHours));
        pt2.put(JSON_USER_ACCOUNT_API_KEY, m_TheCorretApiKey);
        pt2.put(JSON_AD_SPACE_CAMPAIGN_SLOT_CACHE_NUM_API_REQUESTS_FOR_CURRENT_SLOT, boost::lexical_cast<std::string>(m_NumApiRequestsForThisAdSlotSoFar));
        std::ostringstream newCacheJsonBuffer;
        write_json(newCacheJsonBuffer, pt2, false);
        beginDbStore(adSpaceCampaignSlotCacheKey(m_CampaignOwnerRequested, m_CampaignIndexRequested), newCacheJsonBuffer.str(), CreateOrUpdateAdCampaignCurrentSlotCache);
#if 0
    }
#endif
}
void GetTodaysAdSlotServerClientConnection::backendDbGetCallback(std::string couchbaseDocument, bool lcbOpSuccess, bool dbError)
{
    m_BackendRequestPending = false;
    if(m_WantsToBeDeletedLater) //they disconnected before our async db hit returned
    {
        m_ClientStream.device()->deleteLater(); //double delete later doesn't matter
        return;
    }

    //normal backend callback processing
    switch(m_Stage)
    {
    case GetAdCampaignCurrentSlotCacheIfExists:
        continueAtJustFinishedAttemptingToGetAdCampaignCurrentSlotCache(couchbaseDocument, lcbOpSuccess, dbError);
        break;
    case GetAdCampaignDocItselfToGetSlotLengthHours:
        continueAtJustFinishedAttemptingToGetAdCampaign(couchbaseDocument, lcbOpSuccess, dbError);
        break;
    case GetAdCampaignOwnersProfileJustToGetApiKey:
        continueAtJustFinishedAttemptingToGetCampaignOwnersProfileDoc(couchbaseDocument, lcbOpSuccess, dbError);
        break;
    case GetAdCampaignSlot:
        continueAtJustFinishedAttemptingToGetAdCampaignSlot(couchbaseDocument, lcbOpSuccess, dbError);
        break;
    case GetAdCampaignSlotFiller:
        continueAtJustFinishedAttemptingToGetAdCampaignSlotFiller(couchbaseDocument, lcbOpSuccess, dbError);
        break;
    default:
    //case CreateOrUpdateAdCampaignCurrentSlotCache:
        break;
    }
}
void GetTodaysAdSlotServerClientConnection::backendDbStoreCallback(bool lcbOpSuccess, bool dbError)
{
    (void)lcbOpSuccess;
    (void)dbError;
    m_BackendRequestPending = false;
    if(m_WantsToBeDeletedLater) //they disconnected before our async db hit returned
    {
        m_ClientStream.device()->deleteLater(); //double delete later doesn't matter
        return;
    }

    //normal backend callback processing
    switch(m_Stage)
    {
    case CreateOrUpdateAdCampaignCurrentSlotCache:
        //do nothing. close was already called on the socket, which means that the deleteLater above is likely to be called instead of this code path becaue the socket could have already disconnected. if it didn't disconnect yet, it will soon and then deleteLater would be called
        //since it doesn't hurt:
        deleteLaterIfNoBackendRequestPendingOrWhenThatBackendRequestFinishes();
        break;
    default:
        break;
    }
}
void GetTodaysAdSlotServerClientConnection::handleClientReadyRead()
{
    if(m_WantsToBeDeletedLater && !m_BackendRequestPending)
    {
        m_ClientStream.device()->deleteLater();
        return;
    }
    if(m_BackendRequestPending)
    {
        m_ClientStream.device()->readAll(); //we only deal with one request, so anything else just gets read and ignored
        return;
    }

    if(m_ClientStream.device()->canReadLine())
    {
        QString currentLine = m_ClientStream.readLine();
        if(currentLine.startsWith("GET "))
        {
            QStringList getLineParts = currentLine.split(" ");
            if(getLineParts.size() > 1)
            {
                QString encodedUrlPath = getLineParts.at(1);
                if(encodedUrlPath.startsWith("/"))
                {
                    QUrl theUrl = QUrl::fromUserInput("http://dummydomain.com" + encodedUrlPath);
                    if(theUrl.isValid())
                    {
                        if(theUrl.path() == "/current-ad.json")
                        {
                            QUrlQuery theUrlQuery(theUrl);
                            QString userStr = theUrlQuery.queryItemValue("user");
                            QString indexStr = theUrlQuery.queryItemValue("index");
                            QString apiKeyStr = theUrlQuery.queryItemValue(JSON_USER_ACCOUNT_API_KEY);
                            if((!indexStr.isEmpty()) && (!userStr.isEmpty()) && (!apiKeyStr.isEmpty()))
                            {
                                bool parseSuccess = false;
                                int x = indexStr.toInt(&parseSuccess);
                                if(parseSuccess && x > -1)
                                {
                                    LettersNumbersOnlyRegExpValidatorAndInputFilter lettersNumbersOnlyValidator;
                                    m_CampaignOwnerRequested = userStr.toStdString(); //sure we might be setting these to values that are 'illegal', but unless they both validate, we never even get to a place where we use them
                                    m_CampaignIndexRequested = indexStr.toStdString();
                                    m_ApiKeyRequested = apiKeyStr.toStdString();
                                    if(lettersNumbersOnlyValidator.validate(m_CampaignOwnerRequested).state() == LettersNumbersOnlyRegExpValidatorAndInputFilter::Valid)
                                    {
                                        if(lettersNumbersOnlyValidator.validate(m_ApiKeyRequested).state() == LettersNumbersOnlyRegExpValidatorAndInputFilter::Valid)
                                        {
                                            //schedule backend request
                                            beginDbGet(adSpaceCampaignSlotCacheKey(m_CampaignOwnerRequested, m_CampaignIndexRequested), GetAdCampaignCurrentSlotCacheIfExists);
                                            disconnect(m_ClientStream.device(), SIGNAL(readyRead()), this, SLOT(handleClientReadyRead())); //we got what we wanted
                                        }
                                        else
                                        {
                                            sendResponseAndCloseSocket(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_CUSTOM_ERROR(GetTodaysAdSlotServerClientConnection_INVALID_API_KEY_MESSAGE));
                                            return;
                                        }
                                    }
                                    else
                                    {
                                        sendResponseAndCloseSocket(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_CUSTOM_ERROR("Invalid HTTP GET URL Parameter 'user'"));
                                        return;
                                    }
                                }
                                else
                                {
                                    sendResponseAndCloseSocket(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_CUSTOM_ERROR("Invalid HTTP GET URL Parameter 'index'"));
                                    return;
                                }
                            }
                            else
                            {
                                sendResponseAndCloseSocket(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_CUSTOM_ERROR("Missing HTTP GET URL Parameters ('user' and/or 'index' and/or '" JSON_USER_ACCOUNT_API_KEY "')"));
                                return;
                            }
                        }
                        else
                        {
                            sendResponseAndCloseSocket(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_CUSTOM_ERROR("Invalid HTTP GET URL. Did you mean '/current-ad.json'?"));
                            return;
                        }
                    }
                    else
                    {
                        sendResponseAndCloseSocket(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_CUSTOM_ERROR("Invalid HTTP GET URL"));
                        return;
                    }
                }
                else
                {
                    sendResponseAndCloseSocket(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_CUSTOM_ERROR("Invalid HTTP GET URL"));
                    return;
                }
            }
            else
            {
                sendResponseAndCloseSocket(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_CUSTOM_ERROR("Invalid HTTP Method Parameters"));
                return;
            }
        }
        else
        {
            sendResponseAndCloseSocket(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_CUSTOM_ERROR("Invalid HTTP Method"));
            return;
        }

        if(m_ClientStream.device()->bytesAvailable() > 0)
        {
            m_ClientStream.device()->readAll();  //we only deal with one line, so anything else just gets read and ignored
        }
        if(!m_BackendRequestPending) //the line must have sucked
            sendResponseAndCloseSocket(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_NO_AD_PLACEHOLDER);
    }
}
void GetTodaysAdSlotServerClientConnection::deleteLaterIfNoBackendRequestPendingOrWhenThatBackendRequestFinishes()
{
    m_WantsToBeDeletedLater = true;
    if(!m_BackendRequestPending)
    {
        m_ClientStream.device()->deleteLater();
    }
}
