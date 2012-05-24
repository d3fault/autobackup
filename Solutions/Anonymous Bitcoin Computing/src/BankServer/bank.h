#ifndef BANK_H
#define BANK_H

#include <QObject>

#include "../RpcBankServer/ibank-rpc-output.h"

class Bank : public IBank
{
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
