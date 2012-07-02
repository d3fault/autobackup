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
    explicit RpcServerBusinessImpl(QObject *parent = 0);
private:
    QThread *m_BitcoinThread;
    BitcoinHelperAndBroadcastMessageDispenserUser *m_Bitcoin;
signals:

public slots:
    void init();
    void createBankAccount(CreateBankAccountMessage *createBankAccountMessage);
};

#endif // RPCSERVERBUSINESSIMPL_H
