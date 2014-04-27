#ifndef ADIMAGEGETANDSUBSCRIBEMANAGER_H
#define ADIMAGEGETANDSUBSCRIBEMANAGER_H

#include <string>

#include <Wt/WServer> //for WServer::post (aww shit TODOoptional don't let this subscription manager depend on wt, use a "respond" pure virtual or some such like in Abc2)

#include <QObject>
#include <QNetworkAccessManager>
#include <QHash>
#include <QHash>
#include <QHashIterator>

#include <boost/function.hpp>

using namespace std;

namespace Wt
{
class WResource;
}

class AdImageWResource;

typedef boost::function<void (Wt::WResource* /*image*/, std::string /*url*/, std::string /*altAndHover*/)> GetAndSubscriptionUpdateCallbackType;

//could incorporate "HackyVideoBullshit" into name, but it can (and might, if I pursue expanding Abc2 (zzzzz)) be used with tons of other apps... so...
class AdImageGetAndSubscribeManager : public QObject
{
    Q_OBJECT
public:
    class AdImageSubscriberIdentifier {};
    AdImageGetAndSubscribeManager(QObject *parent = 0);
    ~AdImageGetAndSubscribeManager();
private:
    struct AdImageSubscriberSessionInfo
    {
        string SessionId;
        GetAndSubscriptionUpdateCallbackType GetAndSubscriptionUpdateCallback;
    };
    QHash<AdImageSubscriberIdentifier*, AdImageSubscriberSessionInfo*> m_Subscribers;
    AdImageWResource *m_CurrentAdImage;
    AdImageWResource *m_YesterdaysAdImage;
    string m_CurrentAdUrl;
    string m_CurrentAdAltAndHover;
    long long m_CurrentAdExpirationDateTime;
    bool m_Stopping;
    bool m_CurrentlyShowingNoAdPlaceholder;
    QNetworkAccessManager* m_NetworkAccessManager;
    QHashIterator<AdImageSubscriberIdentifier*, AdImageSubscriberSessionInfo*> *m_UpdateSubscribersHashIterator;
    QHash<AdImageSubscriberIdentifier*, AdImageSubscriberSessionInfo*> m_QueuedSubscriptionRequests;
    QList<AdImageSubscriberIdentifier*> m_QueuedUnsubscriptionRequests;

    void startHttpRequestForNextAdSlot();
    inline std::string base64decodeStdString(const std::string &input)
    {
        QByteArray b64decodedByteArray = QByteArray::fromBase64(QByteArray::fromRawData(input.c_str(), input.length()));
        string ret(b64decodedByteArray.constData(), b64decodedByteArray.length());
        return ret;
    }
public slots:
    void getAndSubscribe(AdImageGetAndSubscribeManager::AdImageSubscriberIdentifier *adImageSubscriberIdentifier, std::string sessionId, GetAndSubscriptionUpdateCallbackType getAndSubscriptionUpdateCallback);
    void unsubscribe(AdImageGetAndSubscribeManager::AdImageSubscriberIdentifier *adImageSubscriberIdentifier);
    void beginStopping();
private slots:
    void handleNetworkRequestRepliedTo(QNetworkReply *reply);
    void updateSubscribers();
    void expireTimerTimedOut();
    void updateTenAndScheduleA30msTimeoutUntilAllEmpty();
};

#endif // ADIMAGEGETANDSUBSCRIBEMANAGER_H
