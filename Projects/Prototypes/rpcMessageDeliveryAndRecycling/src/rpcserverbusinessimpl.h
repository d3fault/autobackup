#ifndef RPCSERVERBUSINESSIMPL_H
#define RPCSERVERBUSINESSIMPL_H

#include <QObject>
#include <QThread>

#include "irpcserverimpl.h"
#include "bitcoinhelperandbroadcastmessagedispenseruser.h"

class RpcServerBusinessImpl : public IRpcServerImpl
{
    Q_OBJECT
private:
    QThread *m_BitcoinThread;
    BitcoinHelperAndBroadcastMessageDispenserUser *m_Bitcoin;
public slots:
    void init();
    void createBankAccount(CreateBankAccountMessage *createBankAccountMessage);
};

#endif // RPCSERVERBUSINESSIMPL_H
