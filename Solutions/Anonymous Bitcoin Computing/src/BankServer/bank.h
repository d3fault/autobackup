#ifndef BANK_H
#define BANK_H

#include <QObject>
#include <QThread>

#include "bankdbhelper.h"
#include "../RpcBankServer/ibankserver.h"

class Bank : public IBankServer
{
private:
    BankDbHelper *m_Db;
    QThread *m_DbThread;
signals:
    //void initialized();
    //void d(const QString &);
    //void bankAccountCreated(const QString &username); //no point in re-declaring an implemented signal. just emit it.

public slots:
    void init();

    void createBankAccount(const QString &username);
    /*void handleBankAccountCreationRequested(const QString &username);
    void handleBalanceTransferRequested(const QString &username, double amount);*/
};

#endif // BANK_H
