#ifndef RPCMESSAGEDELIVERYANDRECYCLINGTHREADEDBETTERTEST_H
#define RPCMESSAGEDELIVERYANDRECYCLINGTHREADEDBETTERTEST_H

#include "idebuggablestartablestoppablebackend.h"
#include "rpcbankserverimpl.h"
#include "rpcbankserverclientshelper.h"

class rpcMessageDeliveryAndRecyclingThreadedBetterTest : public IDebuggableStartableStoppableBackend
{
    Q_OBJECT
public:
    explicit rpcMessageDeliveryAndRecyclingThreadedBetterTest();
public slots:
    void init();
    void start();
    void stop();

    void simulateCreateBankAccount();
private:
    RpcBankServerImpl *m_Business;
    RpcBankServerClientsHelper *m_RpcBankServerClientsHelper;
};

#endif // RPCMESSAGEDELIVERYANDRECYCLINGTHREADEDBETTERTEST_H
