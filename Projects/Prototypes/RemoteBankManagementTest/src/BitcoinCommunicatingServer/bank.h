#ifndef BANK_H
#define BANK_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QList>
#include <QHash>

#include "appclienthelper.h"
#include "bankdb.h"
#include "bitcoinhelper.h"

//#define TIME_IN_BETWEEN_EACH_POLL 600000 //10 minutes for impl... actually maybe this is too high. maybe 10 seconds IS what we want for the impl? could be anything really... 1 minute sounds nice...
#define TIME_IN_BETWEEN_EACH_POLL 10000 //10 seconds for debugging

class Bank : public QObject
{
    Q_OBJECT
public:
    struct AppIdAndUsernameStruct
    {
        QString AppId;
        QString UserName;
    };
    explicit Bank(QObject *parent = 0);
private:
    QThread *m_ServerThread;
    AppClientHelper *m_Clients; //plural because the server can handle multiple connections
    BankDb m_Db;
    QTimer *m_PollingTimer;
    bool m_CurrentlyProcessingPollingLists;

    QList<QString> m_ListOfNewKeysToAddToAwaitingOnNextTimeout;
    QList<QString> m_ListOfNewKeysToAddToPendingOnNextTimeout;
    QList<QString> m_ListOfAwaitingKeysToPoll; //no payment has been seen yet. once we do, we move it into m_ListOfPendingKeysToPoll
    QList<QString> m_ListOfPendingKeysToPoll; //we see a payment on the bitcoin network, but it has yet to be included in a block. no confirmations yet
    int m_CurrentIndexInListOfAwaitingKeysToPoll;
    int m_CurrentIndexInListOfPendingKeysToPoll;

    void reEnableTimerIfBothListsAreDone();

    QHash<QString /*key*/, AppIdAndUsernameStruct> m_AppIdAndUserNameByKey;
signals:
    void d(const QString &);
    void userAdded(const QString &appId, const QString &userName);
    void addFundsKeyGenerated(const QString &appId, const QString &userName, const QString &newKey);
    void pendingAmountDetected(QString appId, QString userName, QString key, double pendingAmount);
    void confirmedAmountDetected(QString appId, QString userName, QString key, double confirmedAmount);
public slots:
    void start();
    void pollOneAwaitingKey();
    void pollOnePendingKey();
private slots:
    void handleAddUserRequested(const QString &appId, const QString &userName);
    void handleAddFundsKeyRequested(const QString &appId, const QString &userName);
    void handlePollingTimerTimedOut();
};

#endif // BANK_H
