#ifndef RPCMESSAGEDELIVERYANDRECYCLINGTHREADEDBETTERTEST_H
#define RPCMESSAGEDELIVERYANDRECYCLINGTHREADEDBETTERTEST_H

#include <QThread>

#include "idebuggablestartablestoppablebackend.h"
#include "rpcbusinessimpl.h"
#include "rpcclientshelperimpl.h"

class rpcMessageDeliveryAndRecyclingThreadedBetterTest : public IDebuggableStartableStoppableBackend
{
    Q_OBJECT
public:
    explicit rpcMessageDeliveryAndRecyclingThreadedBetterTest();
public slots:
    void init();
    void start();
    void stop();
private:
    RpcBusinessImpl *m_Business;
    RpcClientsHelperImpl *m_ClientsHelper;

    QThread *m_BusinessThread;
    QThread *m_ClientsHelperThread;

    void instantiateObjects();
    void organizeThreads();
    void startThreads();
};

#endif // RPCMESSAGEDELIVERYANDRECYCLINGTHREADEDBETTERTEST_H
