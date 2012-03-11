#ifndef BANK_H
#define BANK_H

#include <QObject>
#include <QThread>

#include "appclienthelper.h"
#include "bankdb.h"
#include "bitcoinhelper.h"
#include "bitcoinpoller.h"

class Bank : public QObject
{
    Q_OBJECT
public:
    explicit Bank(QObject *parent = 0);
private:
    QThread *m_ServerThread;
    AppClientHelper *m_Clients; //plural because the server can handle multiple connections
    BankDb *m_Db;
    BitcoinPoller *m_BitcoinPoller;

    /*
    bool m_CurrentlyProcessingPollingLists;

    QList<QString> m_ListOfNewKeysToAddToAwaitingOnNextTimeout;
    QList<QString> m_ListOfNewKeysToAddToPendingOnNextTimeout;
    QList<QString> m_ListOfAwaitingKeysToPoll; //no payment has been seen yet. once we do, we move it into m_ListOfPendingKeysToPoll
    QList<QString> m_ListOfPendingKeysToPoll; //we see a payment on the bitcoin network, but it has yet to be included in a block. no confirmations yet
    int m_CurrentIndexInListOfAwaitingKeysToPoll;
    int m_CurrentIndexInListOfPendingKeysToPoll;

    void reEnableTimerIfBothListsAreDone();

    QHash<QString, AppIdAndUsernameStruct> m_AppIdAndUserNameByKey;
    */
signals:
    void d(const QString &);
    void userAdded(const QString &appId, const QString &userName);
    void addFundsKeyGenerated(const QString &appId, const QString &userName, const QString &newKey);
    void pendingAmountReceived(QString appId, QString username, QString key, double pendingAmount);
    void confirmedAmountReceived(QString appId, QString username, QString key, double confirmedAmount);
public slots:
    void start();
    //void pollOneAwaitingKey();
    //void pollOnePendingKey();
private slots:
    void handleAddUserRequested(const QString &appId, const QString &userName);
    void handleAddFundsKeyRequested(const QString &appId, const QString &userName);
    void handlePendingAmountDetected(QString appId,QString username,QString key, double pendingAmount);
    void handleConfirmedAmountDetected(QString appId,QString username,QString key,double confirmedAmount);
    //void handlePollingTimerTimedOut();
};

#endif // BANK_H
