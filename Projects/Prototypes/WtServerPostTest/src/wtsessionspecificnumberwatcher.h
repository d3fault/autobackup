#ifndef WTSESSIONSPECIFICNUMBERWATCHER_H
#define WTSESSIONSPECIFICNUMBERWATCHER_H

#include <QObject>
#include <QMutex>
#include <QList>

#include <Wt/WServer>
using namespace Wt;

typedef boost::function<void (int)> ServerPostCallback;

class QtAwareWtApp;

class WtSessionSpecificNumberWatcher : public QObject
{
    Q_OBJECT
public:
    explicit WtSessionSpecificNumberWatcher(std::string wtSessionId, WServer &server, ServerPostCallback callback);
    void watchNumber(int numberToWatch);
private:
    QMutex *m_NumberWatchingListMutex;
    QList<int> m_NumbersBeingWatched;
    //QtAwareWtApp *m_Wt;
    std::string m_WtSessionId;
    Wt::WServer &m_WtServer;
    ServerPostCallback m_WtSessionCallback;
Q_SIGNALS:
    //void numberSeeen(); <--against my instincts and prior learnings, i can't use a signal :(. i have to use Server.Post() to propagate it to the right thread/context. weird/shitty imo
public Q_SLOTS:
    void handleGeneratedNumber(int number);
};

#endif // WTSESSIONSPECIFICNUMBERWATCHER_H
