#ifndef APPDBHELPER_H
#define APPDBHELPER_H

#include <QObject>
#include <QThread>
#include <QCache>
#include <QHash>

struct AppDbResult
{
    AppDbResult(QString returnString1 = QString(), QString returnString2 = QString(), QString returnString3 = QString(), double returnDouble1 = -1.0, returnDouble2 = -1.0, returnDouble3 = -1.0)
        : NotInCacheWeWillNotifyYou(true), ReturnString1(returnString1), ReturnString2(returnString2), ReturnString3(returnString3), ReturnDouble1(returnDouble1), ReturnDouble2(returnDouble2), ReturnDouble3( returnDouble3)
    {}

    bool NotInCacheWeWillNotifyYou;
    QString ReturnString1;
    QString ReturnString2;
    QString ReturnString3;
    double ReturnDouble1;
    double ReturnDouble2;
    double ReturnDouble3;
};

//AppDbHelper is a cache, a notifier, and a connection manager (although it does not actually handle the connection itself)
class AppDbHelper : public QObject
{
    Q_OBJECT
public:
    class AppDbHelperClient
    {};
    static AppDbHelper* Instance();

    AppDbResult NowViewingAbcBalanceAddFunds(QString username, CallbackInfo callbackInfo);
    AddFundsRequestResult AddFundsRequest(QString username, CallbackInfo callbackInfo);
private:
    AppDbHelper();
    static AppDbHelper *m_pInstance;

    QThread *m_SocketThread;
    AppDbSocket *m_AppDb; //the real helper. ssl socket (manager)

    //is QCache appropriate? should I abstract the cache into it's own class? it might making modifying it easier in the future too
    //basically it kind of conflicts with my 'push all updates' theory. AppDb pushes all updates to every Wt-Front End (which is what we're coding. this QCache is a part of that). in doing so, we have to remove old items from the QCache as well...
    //any new updates pushed to us would be considered hot anyways by nature (they most likely will want to be seen)... in combination with values that are simply 'GET'd frequently. they will both be MRU in the cache. notifications that we don't use and old values that aren't GET'd anymore will leave the cache. i guess this is acceptable.
    QCache<QString,UserAddFundsPageValues> cachedUserAddFundsPageValues;
    QHash<AppDbHelperClient*,ViewUserIsAtSoWeKnowWhatValuesToUpdate> userViewHash;
signals:
public slots:
    void init();
};

#endif // APPDBHELPER_H
