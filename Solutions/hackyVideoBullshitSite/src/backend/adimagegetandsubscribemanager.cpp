#include "adimagegetandsubscribemanager.h"

#include <QNetworkReply>
#include <QDateTime>
#include <QTimer>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "abc2couchbaseandjsonkeydefines.h"
#include "adimagewresource.h"
#include "nonanimatedimagheaderchecker.h"

using namespace Wt;
using namespace boost::property_tree;

//class originally intended to only be for ad image backend, now transform[ing|ed] into video segment 'pusher' (perhaps i should push that functionality into an object that this class hasA. still, even in that case it would be proper'er to rename this class to just HackyVideoBullshitBackend. maybe even should push  the ad image get and subscribe functionality into an object, for modularity reasons
AdImageGetAndSubscribeManager::AdImageGetAndSubscribeManager(QObject *parent)
    : QObject(parent), m_CurrentAdImage(0), m_YesterdaysAdImage(0), m_CurrentAdUrl("n"), m_CurrentAdExpirationDateTime(0), m_Stopping(false), m_CurrentlyShowingNoAdPlaceholder(true), m_NetworkAccessManager(0), m_UpdateSubscribersHashIterator(0)
{
    qRegisterMetaType<GetAndSubscriptionUpdateCallbackType>("GetAndSubscriptionUpdateCallbackType"); //because invokeObject sends these as Q_ARG's
    qRegisterMetaType<AdImageGetAndSubscribeManager::AdImageSubscriberIdentifier*>("AdImageGetAndSubscribeManager::AdImageSubscriberIdentifier*");
    qRegisterMetaType<std::string>("std::string");
}
AdImageGetAndSubscribeManager::~AdImageGetAndSubscribeManager()
{
    //moved this to finishStopping because finishStopping is called on the thread moved to, whereas destructor is called from main thread :-/
}
void AdImageGetAndSubscribeManager::startHttpRequestForNextAdSlot()
{
    m_NetworkAccessManager->get(QNetworkRequest(QUrl("http://anonymousbitcoincomputing.com/getTodaysAdSlot"))); //TODOreq: maybe a shared secret url or special 'token', and definitely want SSL so no MITM lawl (even self signed cert is better than none)
}
void AdImageGetAndSubscribeManager::initializeAndStart()
{
    //do the first GET for today's ad
    if(!m_NetworkAccessManager)
    {
        m_NetworkAccessManager = new QNetworkAccessManager(this);
        connect(m_NetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleNetworkRequestRepliedTo(QNetworkReply*)));
    }
    startHttpRequestForNextAdSlot();

    //TODOreq: set up timer to expire 30 seconds before ad image change

    //TODOreq: spread out WServer::post'ing over a 5 minute span (perhaps also explain this to ad slot buyer on Abc2). as an additional optimization (efficiency/speed one), we can have a "max time between each WServer::post", for when there aren't very many users watching (or just on that server). 10 viewers shouldn't use the "over 5 minutes" bandwidth optimization, nah mean?

    m_Stopping = false;
}
void AdImageGetAndSubscribeManager::getAndSubscribe(AdImageGetAndSubscribeManager::AdImageSubscriberIdentifier *adImageSubscriberIdentifier, std::string sessionId, GetAndSubscriptionUpdateCallbackType getAndSubscriptionUpdateCallback)
{
    if(m_Stopping)
        return;

    //optimization to do the 'get' now before the subscribe below, but hardly makes any difference...
    WServer::instance()->post(sessionId, boost::bind(getAndSubscriptionUpdateCallback, m_CurrentAdImage, m_CurrentAdUrl, m_CurrentAdAltAndHover)); //TODOreq: make sure this is populated [with at least a default value] right away (so perhaps a blocking/queued initialize call?), and make sure you use synchronous http get (but actually async get might be fine (the m_CurrentAdImage needs to be modified in 'one method' however), just a race condition where they see an image for a brief moment ya know (race condition exists NO MATTER WHAT though, and isn't 'bad'))

    AdImageSubscriberSessionInfo *sessionInfo = new AdImageSubscriberSessionInfo();
    sessionInfo->SessionId = sessionId;
    sessionInfo->GetAndSubscriptionUpdateCallback = getAndSubscriptionUpdateCallback;
    m_Subscribers.insert(adImageSubscriberIdentifier, sessionInfo); //TODOoptional: won't need it for HackyVideoBullshitSite, but if future proofing this it may be wise to first check if it exists and to not double add it (would leak memory of old session info). The same check kind of exists for unsubscribe
}
//TODOforever: just like IRL junk mail, unsubscribing may still yield a few more subscription updates before the unsubscribe is processed. Subscribers need to be able to handle this (in HackyVideoBullshitSite I handle this easily, because WServer::post handles delete case and I only ever unsubscribe in the destructor)
void AdImageGetAndSubscribeManager::unsubscribe(AdImageGetAndSubscribeManager::AdImageSubscriberIdentifier *adImageSubscriberIdentifier)
{
    AdImageSubscriberSessionInfo *sessionInfo = m_Subscribers.take(adImageSubscriberIdentifier);
    delete sessionInfo; //TODOreq: destructor should iterate/delete too? really depends on my shutdown procedure and future proofing etc (fuck noobs imo)
}
void AdImageGetAndSubscribeManager::beginStopping()
{
    m_Stopping = true;
}
void AdImageGetAndSubscribeManager::finishStopping()
{
    if(m_CurrentAdImage)
    {
        delete m_CurrentAdImage;
        m_CurrentAdImage = 0;
    }
    if(m_YesterdaysAdImage)
    {
        delete m_YesterdaysAdImage;
        m_YesterdaysAdImage = 0;
    }

    QHashIterator<AdImageSubscriberIdentifier*, AdImageSubscriberSessionInfo*> it(m_Subscribers);
    while(it.hasNext())
    {
        it.next();
        AdImageSubscriberSessionInfo *sessionInfo = it.value();
        delete sessionInfo;
    }
    m_Subscribers.clear();
}
void AdImageGetAndSubscribeManager::handleNetworkRequestRepliedTo(QNetworkReply *reply) //TODOreq: errors, such as timeout, use "no ad" placeholder and of course schedule a retry in a little
{
    //read the ad image json and b64decode it
    bool someError = (reply->error() != QNetworkReply::NoError);
    ptree pt;
    if(!someError)
    {
        QTextStream replyTextStream(reply);
        QString replyQString = replyTextStream.readAll();
        const string replyStdString = replyQString.toStdString();
        istringstream is(replyStdString);
        read_json(is, pt);
        const string error = pt.get<string>(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_ERROR_KEY);
        someError = (error == JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_ERROR_YESERROR_VALUE);
    }

    if(someError)
    {
        //"no ad" placeholder
        if(!m_CurrentlyShowingNoAdPlaceholder) //don't 'double' show it
        {
            m_CurrentlyShowingNoAdPlaceholder = true;

            //update subscribers to "no ad" placeholder, a global/public resource with hella long cache expiration date...
            m_CurrentAdUrl = "n"; //hack/lazy
            QMetaObject::invokeMethod(this, "updateSubscribers", Qt::QueuedConnection);
        }

        QTimer::singleShot(((5*60)*1000) /*5 mins*/, Qt::VeryCoarseTimer, this, SLOT(expireTimerTimedOut()));
        reply->deleteLater();
        return;
    }
    //else if "NOERROR" (/lazy)
    m_CurrentlyShowingNoAdPlaceholder = false;

    const string imageBase64 = pt.get<string>(JSON_SLOT_FILLER_IMAGEB64);
#if 0
    QByteArray imageByteArray = QByteArray::fromBase64(QByteArray::fromRawData(imageBase64.c_str(), imageBase64.length()));
    QString imageQString(imageByteArray);
    const string imageStdString = imageQString.toStdString();
#else
    const string imageStdString = base64decodeStdString(imageBase64);
#endif
    if(m_YesterdaysAdImage)
    {
        delete m_YesterdaysAdImage; //we keep an ad image around for an extra day, which pretty much assures us that there won't be any requests still asking for it (since it's a concurrent resource)
    }
    m_YesterdaysAdImage = m_CurrentAdImage; //there may still be requests for it happening right this very moment, so we hold off on deleting it (i still don't understand how beingDeleted() works xD)
    std::string mimeTypeHalfAndFileExtension = "jpeg";
    if(imageStdString.length() > 1) //we already do this check before accepting it in Abc2, but eh don't want to allow this app to segfault based on that assumption
    {
        const char* imageCStr = imageStdString.c_str();
        char imageHeader[2] = {imageCStr[0], imageCStr[1]};
        mimeTypeHalfAndFileExtension = NonAnimatedImageHeaderChecker::guessImageFormatFromHeaderMagic(imageHeader);

    }
    m_CurrentAdImage = new AdImageWResource(imageStdString, "image/" + mimeTypeHalfAndFileExtension, "image." + mimeTypeHalfAndFileExtension, WResource::Inline);

    m_CurrentAdUrl = base64decodeStdString(pt.get<string>(JSON_SLOT_FILLER_URL));
    m_CurrentAdAltAndHover = base64decodeStdString(pt.get<string>(JSON_SLOT_FILLER_HOVERTEXT));
    m_CurrentAdExpirationDateTime = boost::lexical_cast<long long>(pt.get<string>(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_EXPIRATION_DATETIME));

    QMetaObject::invokeMethod(this, "updateSubscribers", Qt::QueuedConnection); //free dat stack memory from above first kthx :-P

    long long expireDateTimeMSecs = (m_CurrentAdExpirationDateTime*1000)-30;
    QTimer::singleShot(static_cast<int>(expireDateTimeMSecs - QDateTime::currentMSecsSinceEpoch()), Qt::VeryCoarseTimer, this, SLOT(expireTimerTimedOut()));

    //TODOreq: expire time needs to have been put in and extracted from the json. TODOreqoptimization: if two "no ad" placeholders in a row (thinking 5 min expiration dates), the second one does not transfer image bytes or even WServer::post

    reply->deleteLater();
}
void AdImageGetAndSubscribeManager::updateSubscribers()
{
    //the "spread out over 5 mins" thing is going to be a tiny bit of a pain in the ass (nothing i can't handle) to implement, because an unsubscribe while "sleeping" would modify the hash and could make us skip notifying a subscriber. hanging onto the iterator might work since modifications to the hash make a copy of it (and i guess we'd have to 'delete' the iterator to free our stale (but we want it to be stale as per the first sentence of this comment) copy of the hash after we're finished). Also need to make sure we don't start ANOTHER updateSubscribers call before out prior one finishes, but that shouldn't be too hard (spread out over "4:20 seconds" instead of "5", since 5 is the "no ad" poll interval and it would be initiated at 4:30))

    //as a precautionary measure, we check to see if a subscription update 'over 5 mins' is already active. if it is, we just return. this should never happen but you never know...
    if(!m_UpdateSubscribersHashIterator)
        return;

    m_UpdateSubscribersHashIterator = new QHashIterator<AdImageSubscriberIdentifier*, AdImageSubscriberSessionInfo*>(m_Subscribers); //increments reference count, so if a new subscriber or unsubscribe, m_Subscribers does a COW and doesn't affect our list of updaters. new subscribers are handled instantly in "subscribe", and unsubscribers are handled by WServer::post knowing when a WApplication has been destroyed

    //update 10 every .3 seconds, the first 10 right now :P
    updateTenAndScheduleA30msTimeoutUntilAllEmpty();

#if 0
    QHashIterator<AdImageSubscriberIdentifier*, AdImageSubscriberSessionInfo*> it(m_Subscribers);
    while(it.hasNext())
    {
        it.next();
        AdImageSubscriberSessionInfo *sessionInfo = it.value();
        WServer::instance()->post(sessionInfo->SessionId, boost::bind(sessionInfo->GetAndSubscriptionUpdateCallback, m_CurrentAdImage, m_CurrentAdUrl, m_CurrentAdAltAndHover));
    }
#endif
}
void AdImageGetAndSubscribeManager::expireTimerTimedOut()
{
    startHttpRequestForNextAdSlot();
}
void AdImageGetAndSubscribeManager::updateTenAndScheduleA30msTimeoutUntilAllEmpty() //could easily dynamic the 10 and .3 seconds, but dgaf
{
    for(int i = 0; i < 10; ++i)
    {
        if(!m_UpdateSubscribersHashIterator->hasNext())
        {
            //done updating subscribers
            delete m_UpdateSubscribersHashIterator;
            m_UpdateSubscribersHashIterator = 0;
            return;
        }

        m_UpdateSubscribersHashIterator->next();
        AdImageSubscriberSessionInfo *sessionInfo = m_UpdateSubscribersHashIterator->value();
        WServer::instance()->post(sessionInfo->SessionId, boost::bind(sessionInfo->GetAndSubscriptionUpdateCallback, m_CurrentAdImage, m_CurrentAdUrl, m_CurrentAdAltAndHover));
    }

    //done with this 10, schedule timer calling self .3 seconds later
    QTimer::singleShot(300, Qt::CoarseTimer, this, SLOT(updateTenAndScheduleA30msTimeoutUntilAllEmpty()));
}
