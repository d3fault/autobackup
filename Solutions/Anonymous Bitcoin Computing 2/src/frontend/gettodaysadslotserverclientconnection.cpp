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

#define GetTodaysAdSlotServerClientConnection_FIRST_NO_CACHE_YET_SLOT_INDEX "0"
#define GetTodaysAdSlotServerClientConnection_SECONDS_BEFORE_ACTUAL_EXPIRE_DATETIME_TO_SERVE_THE_NEXT_AD_SLOT 30

using namespace boost::property_tree;

AbcApiGetCouchbaseDocumentByKeyRequestResponder GetTodaysAdSlotServerClientConnection::s_Responder;
queue<GetCouchbaseDocumentByKeyRequest*> *GetTodaysAdSlotServerClientConnection::s_BackendQueue = NULL;
struct event *GetTodaysAdSlotServerClientConnection::s_BackendQueueEvent = NULL;

void GetTodaysAdSlotServerClientConnection::setBackendQueue(queue<GetCouchbaseDocumentByKeyRequest *> *backendQueue)
{
    s_BackendQueue = backendQueue;
}
void GetTodaysAdSlotServerClientConnection::setBackendQueueEvent(struct event *backendQueueEvent)
{
    s_BackendQueueEvent = backendQueueEvent;
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
}
GetTodaysAdSlotServerClientConnection::~GetTodaysAdSlotServerClientConnection()
{
    m_ClientStream.device()->deleteLater();
}
void GetTodaysAdSlotServerClientConnection::beginDbCall(const string &keyToGet, GetTodaysAdSlotServerClientConnection::GetTodaysAdSlotServerClientConnectionStageEnum stageToSet)
{
    GetCouchbaseDocumentByKeyRequest *campaignCurrentSlotCacheDocRequest = new GetCouchbaseDocumentByKeyRequest(&s_Responder, "", this, keyToGet);
    while(!s_BackendQueue->push(campaignCurrentSlotCacheDocRequest)) ;
    event_active(s_BackendQueueEvent, EV_READ|EV_WRITE, 0);
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
        beginDbCall(adSpaceCampaignKey(m_CampaignOwnerRequested, m_CampaignIndexRequested), GetAdCampaignDocItselfJustToGetSlotLengthHours);
        return;
    }
    else
    {
        //the campaign slot index cache exists
        ptree pt;
        std::istringstream is(couchbaseDocument);
        read_json(is, pt);

        m_FirstSlotIndexToTry = pt.get<string>(JSON_AD_SPACE_CAMPAIGN_SLOT_CACHE_CURRENT_SLOT);
        m_SlotLengthHours = pt.get<int>(JSON_AD_SPACE_CAMPAIGN_SLOT_LENGTH_HOURS);
        haveSlotLengthHoursAndKnowFirstSlotIndexToTrySoTry();
        return;
    }
}
void GetTodaysAdSlotServerClientConnection::continueAtJustFinishedAttemptingToGetAdCampaign(const std::string &couchbaseDocument, bool lcbOpSuccess, bool dbError)
{
    if(dbError || !lcbOpSuccess)
    {
        sendResponseAndCloseSocket(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_NO_AD_PLACEHOLDER);
        return;
    }

    //got campaign doc

    ptree pt;
    std::istringstream is(couchbaseDocument);
    read_json(is, pt);
    m_SlotLengthHours = pt.get<int>(JSON_AD_SPACE_CAMPAIGN_SLOT_LENGTH_HOURS);
    haveSlotLengthHoursAndKnowFirstSlotIndexToTrySoTry(); //first slot index still at initialized value of "0"
}
void GetTodaysAdSlotServerClientConnection::haveSlotLengthHoursAndKnowFirstSlotIndexToTrySoTry()
{
    m_CurrentSlotIndexToTry = m_FirstSlotIndexToTry;
    tryToGetCurrentSlotIndex();
}
void GetTodaysAdSlotServerClientConnection::tryToGetCurrentSlotIndex()
{
    beginDbCall(adSpaceCampaignSlotKey(m_CampaignOwnerRequested, m_CampaignIndexRequested, m_CurrentSlotIndexToTry), GetAdCampaignSlot);
}
void GetTodaysAdSlotServerClientConnection::continueAtJustFinishedAttemptingToGetAdCampaignSlot(const string &couchbaseDocument, bool lcbOpSuccess, bool dbError)
{
    if(dbError || !lcbOpSuccess)
    {
        sendResponseAndCloseSocket(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_NO_AD_PLACEHOLDER);
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

        //now we look up the ad slot filler to get the hover/url/image
        beginDbCall(pt.get<string>(JSON_AD_SPACE_CAMPAIGN_SLOT_FILLED_WITH), GetAdCampaignSlotFiller);
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
    if(dbError || !lcbOpSuccess)
    {
        sendResponseAndCloseSocket(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_NO_AD_PLACEHOLDER);
        return;
    }

    //have ad slot filler

    ptree pt;
    std::istringstream is2(couchbaseDocument);
    read_json(is2, pt);

    ptree todaysFilledAdSlot;
    todaysFilledAdSlot.put(JSON_SLOT_FILLER_HOVERTEXT, pt.get<string>(JSON_SLOT_FILLER_HOVERTEXT)); //TODOoptional: using json keys in two different docs, fuck it. same with slot length hours
    todaysFilledAdSlot.put(JSON_SLOT_FILLER_URL, pt.get<string>(JSON_SLOT_FILLER_URL));
    todaysFilledAdSlot.put(JSON_SLOT_FILLER_IMAGEB64, pt.get<string>(JSON_SLOT_FILLER_IMAGEB64));
    todaysFilledAdSlot.put(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_EXPIRATION_DATETIME, boost::lexical_cast<string>(m_CurrentAdSlotExpireDateTime));
    todaysFilledAdSlot.put(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_ERROR_KEY, JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_ERROR_NOERROR_VALUE);
    std::ostringstream todaysAdSlotJsonBuffer;
    write_json(todaysAdSlotJsonBuffer, todaysFilledAdSlot, false);
    const std::string &todaysAdSlotJsonStdStr = todaysAdSlotJsonBuffer.str();
    QString todaysAdSlotJson = QString::fromStdString(todaysAdSlotJsonStdStr);

    sendResponseAndCloseSocket(todaysAdSlotJson);

    //store the this+1 in the cache so tomorrow will be uber fast
    if(m_CurrentSlotIndexToTry != m_FirstSlotIndexToTry || (m_CurrentSlotIndexToTry == GetTodaysAdSlotServerClientConnection_FIRST_NO_CACHE_YET_SLOT_INDEX && !m_FoundAdCampaignCurrentSlotCacheDoc)) //only update the cache if it needs to be updated...
    {
        ptree pt2;
        //pt3.put(JSON_AD_SPACE_CAMPAIGN_SLOT_CACHE_CURRENT_SLOT, boost::lexical_cast<string>(boost::lexical_cast<int>(slotIndexToTry)+1));
        pt2.put(JSON_AD_SPACE_CAMPAIGN_SLOT_CACHE_CURRENT_SLOT, m_CurrentSlotIndexToTry); //^if we only request once per day then it makes sense to set the cache to "tomorrow" (+1), but if we request it multiple times per day then it makes sense to set it to "today" (no +1). leaving as "today" since it's safer and I still haven't finalized this shit
        pt2.put(JSON_AD_SPACE_CAMPAIGN_SLOT_LENGTH_HOURS, boost::lexical_cast<std::string>(m_SlotLengthHours));
        std::ostringstream newCacheJsonBuffer;
        write_json(newCacheJsonBuffer, pt2, false);
        //TODO LEFT OFF: beginDbCall( , CreateOrUpdateAdCampaignCurrentSlotCache);
        //OLD: couchbaseStoreRequestWithExponentialBackoff(adSpaceCampaignSlotCacheKey(campaignOwnerUsername, campaignIndex), newCacheJsonBuffer.str(), LCB_SET, 0, "cache index updating"); //we don't care if the cache index updating fails, because [see this doc to understand why]
    }
}
void GetTodaysAdSlotServerClientConnection::backendDbGetCallback(std::string couchbaseDocument, bool lcbOpSuccess, bool dbError)
{
    m_BackendRequestPending = false;
    if(m_WantsToBeDeletedLater) //they disconnected before our async db hit returned
    {
        deleteLater(); //double delete later doesn't matter
        return;
    }

    //normal backend callback processing
    switch(m_Stage)
    {
    case GetAdCampaignCurrentSlotCacheIfExists:
        continueAtJustFinishedAttemptingToGetAdCampaignCurrentSlotCache(couchbaseDocument, lcbOpSuccess, dbError);
        break;
    case GetAdCampaignDocItselfJustToGetSlotLengthHours:
        continueAtJustFinishedAttemptingToGetAdCampaign(couchbaseDocument, lcbOpSuccess, dbError);
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
void GetTodaysAdSlotServerClientConnection::backendDbStoreCallback(string couchbaseDocument, bool lcbOpSuccess, bool dbError)
{
    m_BackendRequestPending = false;
    if(m_WantsToBeDeletedLater) //they disconnected before our async db hit returned
    {
        deleteLater(); //double delete later doesn't matter
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
        deleteLater();
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
                        if(theUrl.path() == "/todays-ad.json")
                        {
                            QUrlQuery theUrlQuery(theUrl);
                            QString userStr = theUrlQuery.queryItemValue("user");
                            QString indexStr = theUrlQuery.queryItemValue("index");
                            if((!indexStr.isEmpty()) && (!userStr.isEmpty()))
                            {
                                bool parseSuccess = false;
                                int x = indexStr.toInt(&parseSuccess);
                                if(parseSuccess && x > -1)
                                {
                                    LettersNumbersOnlyRegExpValidatorAndInputFilter usernameValidator;
                                    m_CampaignOwnerRequested = userStr.toStdString(); //sure we might be setting these to values that are 'illegal', but unless they both validate, we never even get to a place where we use them
                                    m_CampaignIndexRequested = indexStr.toStdString();
                                    if(usernameValidator.validate(m_CampaignOwnerRequested).state() == LettersNumbersOnlyRegExpValidatorAndInputFilter::Valid)
                                    {
                                        //schedule backend request
                                        beginDbCall(adSpaceCampaignSlotCacheKey(m_CampaignOwnerRequested, m_CampaignIndexRequested), GetAdCampaignCurrentSlotCacheIfExists);
                                        disconnect(m_ClientStream.device(), SIGNAL(readyRead()), this, SLOT(handleClientReadyRead())); //we got what we wanted
                                    }
                                }
                            }
                        }
                    }
                }
            }
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
        deleteLater();
    }
}
