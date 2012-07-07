#ifndef RPCBUSINESSIMPL_H
#define RPCBUSINESSIMPL_H

#include <QThread>

#include "irpcbusiness.h"

#include "bitcoinhelper.h"

class RpcBusinessImpl : public IRpcBusiness
{
    Q_OBJECT
public:
    explicit RpcBusinessImpl(QObject *parent = 0);
private:
    QThread *m_BitcoinThread;
    BitcoinHelper *m_Bitcoin;
protected:
    void setParentForEveryBroadcastDispenser(IRpcClientsHelper *clientsHelper);
    void organizeThreadsImpl();
    void startBackendImplThreads();
public slots:
    void init();
    void start();
    void stop();
};

#endif // RPCBUSINESSIMPL_H
