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

    //helpers, which call our private slot equivalent using blocking queued
    AppDbResult NowViewingAbcBalanceAddFunds(QString username, CallbackInfo callbackInfo);
    AppDbResult AddFundsRequest(QString username, CallbackInfo callbackInfo);
private:
    AppDbHelper();
    static AppDbHelper *m_pInstance;

    QThread *m_SocketThread;
    AppDbSocket *m_AppDb; //the real helper

    QCache<QString,UserAddFundsPageValues> cachedUserAddFundsPageValues;
    QHash<AppDbHelperClient*,ViewUserIsAtSoWeKnowWhatValuesToUpdate> userViewHash;
signals:
public slots:
    void init();
};

#endif // APPDBHELPER_H
