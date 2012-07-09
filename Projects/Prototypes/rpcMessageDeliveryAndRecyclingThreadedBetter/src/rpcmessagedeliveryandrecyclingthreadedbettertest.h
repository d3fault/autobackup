#ifndef RPCMESSAGEDELIVERYANDRECYCLINGTHREADEDBETTERTEST_H
#define RPCMESSAGEDELIVERYANDRECYCLINGTHREADEDBETTERTEST_H

#include "idebuggablestartablestoppablebackend.h"
#include "rpcbankserverimpl.h"
#include "rpcbankserverclientshelperimpl.h"

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
    RpcBankServerImpl *m_Business;
    RpcBankServerClientsHelperImpl *m_RpcBankServerClientsHelperImpl;
};

#endif // RPCMESSAGEDELIVERYANDRECYCLINGTHREADEDBETTERTEST_H
