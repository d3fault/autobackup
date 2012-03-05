#ifndef BANK_H
#define BANK_H

#include <QObject>
#include <QThread>

#include "appclienthelper.h"
#include "bankdb.h"
#include "bitcoinhelper.h"

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
signals:
    void d(const QString &);
    void userAdded(const QString &appId, const QString &userName);
    void addFundsKeyGenerated(const QString &appId, const QString &userName, const QString &newKey);
public slots:
    void start();
private slots:
    void handleAddUserRequested(const QString &appId, const QString &userName);
    void handleAddFundsKeyRequested(const QString &appId, const QString &userName);
};

#endif // BANK_H
