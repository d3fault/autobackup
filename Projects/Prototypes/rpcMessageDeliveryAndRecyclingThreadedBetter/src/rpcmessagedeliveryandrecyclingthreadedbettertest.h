#ifndef RPCMESSAGEDELIVERYANDRECYCLINGTHREADEDBETTERTEST_H
#define RPCMESSAGEDELIVERYANDRECYCLINGTHREADEDBETTERTEST_H

#include "idebuggablestartablestoppablebackend.h"
#include "rpcbusinessimpl.h"
#include "rpcbusinesscontrollerimpl.h"

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
    RpcBusinessControllerImpl *m_RpcBusinessController;
};

#endif // RPCMESSAGEDELIVERYANDRECYCLINGTHREADEDBETTERTEST_H
