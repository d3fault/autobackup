#ifndef BITCOINPOLLER_H
#define BITCOINPOLLER_H

#include <QObject>
#include <QTimer>
#include <QHashIterator>
#include <QHash>

#include "bitcoinhelper.h"

class BitcoinPoller : public QObject
{
    Q_OBJECT
public:
    struct DetailsAssociatedWithKey //it might be smarter to just add QString key to this and pass it around with all my signals/slots. i seem to pass these things around a lot (though not the IsPending one)
    {
        QString AppId;
        QString UserName;
        bool IsPending; //false for awaiting, true for pending. we just remove it when it's confirmed. no longer need to poll
        //TODO: QDateTime TimeInserted; //compare with current time, see if it's been longer than a #define MAX_POLLING_TIME .. if so, remove it, stop polling it, dgaf bout it. (the gui should allow the user to manually poll a key......... BUT MAYBE NOT, if we ever gave the key to another user or something there could be disastrious implications. need to understand bitcoin better. i could be way off on this but it's something to keep in mind... or at least worth writing down)
    };
    explicit BitcoinPoller(int pollingIntervalInSeconds, QObject *parent = 0);
private:
    QTimer *m_PollingTimer;
    QHash<QString, DetailsAssociatedWithKey*> m_CurrentlyPollingKeysHash;
    QHash<QString, DetailsAssociatedWithKey*> m_NewKeysHash; //since we can't do a .insert on the hash, we'll maintain 2 lists and then just concatenate them on every timeout
    QMutableHashIterator<QString, DetailsAssociatedWithKey*> *m_KeyIterator; //mutable so we can REMOVE them from the hash while we're iterating. for example, when a key becomes confirmed
    bool m_CurrentlyPolling;
signals:
    void d(const QString &);
    void pendingAmountDetected(QString appId, QString userName, QString key, double pendingAmount);
    void confirmedAmountDetected(QString appId, QString userName, QString key, double confirmedAmount);
public slots:
    void pollThisKeyAndNotifyUsOfUpdates(QString appId, QString userName, QString key);
private slots:
    void handlePollingTimerTimedOut();
    void pollOneKey();
};

#endif // BITCOINPOLLER_H
