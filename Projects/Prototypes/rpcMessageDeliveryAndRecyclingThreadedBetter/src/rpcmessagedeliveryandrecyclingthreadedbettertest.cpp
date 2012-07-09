#include "rpcmessagedeliveryandrecyclingthreadedbettertest.h"

rpcMessageDeliveryAndRecyclingThreadedBetterTest::rpcMessageDeliveryAndRecyclingThreadedBetterTest()
{
    m_Business = new RpcBankServerImpl();
    m_RpcBankServerClientsHelperImpl = new RpcBankServerClientsHelperImpl(m_Business); //attaches itself to IRpcBusinessImpl signals and takes ownership of it. pretty sure i can still connect to slots on m_Business etc... but could also add public methods to the controller to send commands to m_Business. not sure if it matters, can't think of where i'd use it (because i'm definitely having init/start/stop be controller methods)

    //daisy-chaining init'ing, starting, and stopping
    connect(m_RpcBankServerClientsHelperImpl, SIGNAL(initialized()), this, SIGNAL(initialized()));
    connect(m_RpcBankServerClientsHelperImpl, SIGNAL(started()), this, SIGNAL(started()));
    connect(m_RpcBankServerClientsHelperImpl, SIGNAL(stopped()), this, SIGNAL(stopped()));

    /*connect(m_Business, SIGNAL(initialized()), m_RpcBusinessController, SLOT(init()));
    connect(m_Business, SIGNAL(started()), m_RpcBusinessController, SLOT(start()));
    connect(m_RpcBusinessController, SIGNAL(stopped()), m_Business, SLOT(stop()));*/
}
void rpcMessageDeliveryAndRecyclingThreadedBetterTest::init()
{
    //init is called by the GUI in a Queue'd connection after it moves 'this' (us) onto our own thread. we have to make sure we have the dispenser stuff set up before that. by passing the backend object to the front end, we ensure the backend is instantiated first. during that instantiation is where we do all the fancy organizing of dispenser threads (including starting them... which i think is a bit hacky. only because they start before m_Business and m_ClientsHelper threads are started)

    //QMetaObject::invokeMethod(m_Business, "init", Qt::QueuedConnection);
    ////QMetaObject::invokeMethod(m_ClientsHelper, "init", Qt::QueuedConnection);

    m_RpcBankServerClientsHelperImpl->init();
}
void rpcMessageDeliveryAndRecyclingThreadedBetterTest::start()
{
    //QMetaObject::invokeMethod(m_Business, "start", Qt::QueuedConnection);
    ////QMetaObject::invokeMethod(m_ClientsHelper, "start", Qt::QueuedConnection);

    m_RpcBankServerClientsHelperImpl->start();
}
void rpcMessageDeliveryAndRecyclingThreadedBetterTest::stop()
{
    //start in the reverse order we start. might need to make these blockingQueued so they don't step on each other's toes. same with start maybe. possibly init. TODOreq: find out
    //QMetaObject::invokeMethod(m_RpcBusinessController, "stop", Qt::QueuedConnection);
    ////QMetaObject::invokeMethod(m_Business, "stop", Qt::QueuedConnection);

    m_RpcBankServerClientsHelperImpl->stop();
}
