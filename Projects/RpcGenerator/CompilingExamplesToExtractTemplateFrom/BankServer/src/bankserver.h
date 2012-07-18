#ifndef BANKSERVER_H
#define BANKSERVER_H

#include "../../RpcBankServerAndClientShared/irpcbankserver.h"

#include "bitcoinhelper.h"

class BankServer : public IRpcBankServer
{
    Q_OBJECT
public:
    explicit BankServer();
    void instructBackendObjectsToClaimRelevantDispensers();
    void moveBackendBusinessObjectsToTheirOwnThreadsAndStartTheThreads();
private:
    BitcoinHelper *m_Bitcoin;
public slots:
    void createBankAccount(CreateBankAccountMessage *createBankAccountMessage);
    void getAddFundsKey(GetAddFundsKeyMessage *getAddFundsKeyMessage);
    void init();
    void start();
    void stop();
};

#endif // BANKSERVER_H
