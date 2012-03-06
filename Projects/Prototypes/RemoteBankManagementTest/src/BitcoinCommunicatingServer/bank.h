#ifndef BANK_H
#define BANK_H

#include <QObject>
#include <QThread>
#include <QTimer>

#include "appclienthelper.h"
#include "bankdb.h"
#include "bitcoinhelper.h"

#define TIME_IN_BETWEEN_EACH_POLL 10000

class Bank : public QObject
{
    Q_OBJECT
public:
    explicit Bank(QObject *parent = 0);
private:
    QThread *m_ServerThread;
    AppClientHelper *m_Clients; //plural because the server can handle multiple connections
    BankDb m_Db;
    BitcoinHelper m_Bitcoin;
    QTimer *m_PollingTimer;
    bool m_CurrentlyProcessingPollingList;
signals:
    void d(const QString &);
    void userAdded(const QString &appId, const QString &userName);
    void addFundsKeyGenerated(const QString &appId, const QString &userName, const QString &newKey);
public slots:
    void start();
private slots:
    void handleAddUserRequested(const QString &appId, const QString &userName);
    void handleAddFundsKeyRequested(const QString &appId, const QString &userName);
    void handlePollingTimerTimedOut();
};

#endif // BANK_H
