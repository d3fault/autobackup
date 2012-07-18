#ifndef BANKSERVER_H
#define BANKSERVER_H

#define RPC_BANK_SERVER_CODE // we define this because we _ARE_ the rpc server. this makes us parse our messages correctly using QDataStream

#include <QThread>

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
    QThread *m_BitcoinThread;
public slots:
    void init();
    void start();
    void stop();

    void createBankAccount(CreateBankAccountMessage *createBankAccountMessage);
    void getAddFundsKey(GetAddFundsKeyMessage *getAddFundsKeyMessage);    
};

#endif // BANKSERVER_H
