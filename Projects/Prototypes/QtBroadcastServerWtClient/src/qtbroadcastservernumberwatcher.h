#ifndef QTBROADCASTSERVERNUMBERWATCHER_H
#define QTBROADCASTSERVERNUMBERWATCHER_H

#include <Wt/WServer>

#include <QObject>
#include <QDebug>
#include <QMetaType>
#include <QThread>
#include <QHash>
#include <QHashIterator>
#include <QMutableHashIterator>
#include <QList>

#include <boost/function.hpp>

#include "myrandomnumbergenerator.h"

typedef boost::function<void (int)> WtClientCallback; //int1 = number seen, int2 = times it has been seen

class QtBroadcastServerNumberWatcher : public QObject
{
    Q_OBJECT
public:
    class QtBroadcastServerClient
    {};
    static QtBroadcastServerNumberWatcher* Instance();
private:
    struct UsersAndTheirNumbersBeingWatched
    {
        std::string m_SessionId;
        WtClientCallback m_Callback;
        QList<int> *m_NumbersBeingWatched;
    };
    QtBroadcastServerNumberWatcher();
    static QtBroadcastServerNumberWatcher *m_pInstance;
    QHash<QtBroadcastServerClient*, UsersAndTheirNumbersBeingWatched*> *m_UsersAndTheirNumbersBeingWatched;
    QThread *m_RandomNumberGeneratorThread;
    MyRandomNumberGenerator *m_RandomNumberGenerator;
public slots:
    void init();
    void stop();
    void connectToQtBroadcastServer(QtBroadcastServerNumberWatcher::QtBroadcastServerClient *client, std::string sessionId, WtClientCallback callback);
    void watchForNum(QtBroadcastServerNumberWatcher::QtBroadcastServerClient* client, int numberToWatch); //additional numbers to watch.. but we specify our first one in the connect() call
    void disconnectFromQtBroadcastServer(QtBroadcastServerNumberWatcher::QtBroadcastServerClient* client);
private slots:
    void handleRandomNumberGenerated(int num);
};

#endif // QTBROADCASTSERVERNUMBERWATCHER_H
