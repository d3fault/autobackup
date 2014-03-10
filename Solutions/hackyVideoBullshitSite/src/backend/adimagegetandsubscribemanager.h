#ifndef ADIMAGEGETANDSUBSCRIBEMANAGER_H
#define ADIMAGEGETANDSUBSCRIBEMANAGER_H

#include <string>

#include <Wt/WServer> //for WServer::post (aww shit TODOoptional don't let this subscription manager depend on wt, use a "respond" pure virtual or some such like in Abc2)

#include <QObject>
#include <QNetworkAccessManager>
#include <QHash>

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
    bool m_Stopping;
public slots:
    void initializeAndStart();
    void getAndSubscribe(AdImageSubscriberIdentifier *adImageSubscriberIdentifier, string sessionId, GetAndSubscriptionUpdateCallbackType getAndSubscriptionUpdateCallback);
    void unsubscribe(AdImageSubscriberIdentifier *adImageSubscriberIdentifier);
    void beginStopping();
    void finishStopping();
private slots:
    void handleNetworkRequestRepliedTo(QNetworkReply *reply);
    void updateSubscribers();
};

#endif // ADIMAGEGETANDSUBSCRIBEMANAGER_H
