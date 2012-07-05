#ifndef RPCSERVERBUSINESSIMPL_H
#define RPCSERVERBUSINESSIMPL_H

#include <QObject>
#include <QThread>

#include "irpcserverimpl.h"
#include "bitcoinhelperandbroadcastmessagedispenseruser.h"

class RpcServerBusinessImpl : public IRpcServerImpl
{
    Q_OBJECT
public:
    RpcServerBusinessImpl(QObject *parent = 0);
    void pushBroadcastDispensersToAppropriateBusinessThreads();
private:    
    QThread *m_BitcoinThread;
    BitcoinHelperAndBroadcastMessageDispenserUser *m_Bitcoin;
public slots:
    void init();
    void createBankAccount(CreateBankAccountMessage *createBankAccountMessage);
};

#endif // RPCSERVERBUSINESSIMPL_H
