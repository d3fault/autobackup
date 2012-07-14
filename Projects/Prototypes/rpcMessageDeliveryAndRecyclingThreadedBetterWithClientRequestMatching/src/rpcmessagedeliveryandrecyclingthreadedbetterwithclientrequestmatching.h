#ifndef RPCMESSAGEDELIVERYANDRECYCLINGTHREADEDBETTERWITHREQUESTMATCHINGTEST_H
#define RPCMESSAGEDELIVERYANDRECYCLINGTHREADEDBETTERWITHREQUESTMATCHINGTEST_H

#include "idebuggablestartablestoppablebackend.h"
#include "rpcbankserverimpl.h"
#include "rpcbankserverclientshelper.h"

class rpcMessageDeliveryAndRecyclingThreadedBetterWithRequestMatchingTest :
public IDebuggableStartableStoppableBackend
{
    Q_OBJECT
public:
    explicit
rpcMessageDeliveryAndRecyclingThreadedBetterWithRequestMatchingTest();
public slots:
    void init();
    void start();
    void stop();
private:
    RpcBankServerImpl *m_Business;
    RpcBankServerClientsHelper *m_RpcBankServerClientsHelper;
};

#endif // RPCMESSAGEDELIVERYANDRECYCLINGTHREADEDBETTERWITHREQUESTMATCHINGTEST_H
