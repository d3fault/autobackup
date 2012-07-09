#ifndef RPCBANKSERVERIMPL_H
#define RPCBANKSERVERIMPL_H

#include <QThread>

#include "irpcbankserver.h"

#include "bitcoinhelper.h"

class RpcBankServerImpl : public IRpcBankServer
{
    Q_OBJECT
public:
    explicit RpcBankServerImpl(QObject *mandatoryParent = 0);
    void takeOwnershipOfAllBroadcastDispensers(IRpcBankServerClientsHelper *rpcBankServerClientsHelper);
    void moveBackendBusinessObjectsToTheirOwnThreadsAndStartThem();
private:
    QThread *m_BitcoinThread;
    BitcoinHelper *m_Bitcoin;
public slots:
    void init();
    void start();
    void stop();

    void createBankAccount(CreateBankAccountMessage *createBankAccountMessage);
};

#endif // RPCBANKSERVERIMPL_H
