#include "rpcmessagedeliveryandrecyclingthreadedbettertest.h"

rpcMessageDeliveryAndRecyclingThreadedBetterTest::rpcMessageDeliveryAndRecyclingThreadedBetterTest()
{
    instantiateObjects();
    organizeThreads(); //electing of messageDispenser owners etc (etc means just regular thread stuff too)
    startThreads();

    //daisy-chaining init'ing, starting, and stopping
    connect(m_Business, SIGNAL(initialized()), m_ClientsHelper, SLOT(init()));
    connect(m_Business, SIGNAL(started()), m_ClientsHelper, SLOT(start()));
    connect(m_ClientsHelper, SIGNAL(stopped()), m_Business, SLOT(stop()));
}
void rpcMessageDeliveryAndRecyclingThreadedBetterTest::init()
{
    //init is called by the GUI in a Queue'd connection after it moves 'this' (us) onto our own thread. we have to make sure we have the dispenser stuff set up before that. by passing the backend object to the front end, we ensure the backend is instantiated first. during that instantiation is where we do all the fancy organizing of dispenser threads (including starting them... which i think is a bit hacky. only because they start before m_Business and m_ClientsHelper threads are started)

    QMetaObject::invokeMethod(m_Business, "init", Qt::QueuedConnection);
    //QMetaObject::invokeMethod(m_ClientsHelper, "init", Qt::QueuedConnection);
}
void rpcMessageDeliveryAndRecyclingThreadedBetterTest::start()
{
    QMetaObject::invokeMethod(m_Business, "start", Qt::QueuedConnection);
    //QMetaObject::invokeMethod(m_ClientsHelper, "start", Qt::QueuedConnection);
}
void rpcMessageDeliveryAndRecyclingThreadedBetterTest::stop()
{
    //start in the reverse order we start. might need to make these blockingQueued so they don't step on each other's toes. same with start maybe. possibly init. TODOreq: find out
    QMetaObject::invokeMethod(m_ClientsHelper, "stop", Qt::QueuedConnection);
    //QMetaObject::invokeMethod(m_Business, "stop", Qt::QueuedConnection);
}
void rpcMessageDeliveryAndRecyclingThreadedBetterTest::instantiateObjects()
{
    m_Business = new RpcBusinessImpl();
    m_ClientsHelper = new RpcClientsHelperImpl(m_Business); //attaches itself to IRpcBusinessImpl signals
}
void rpcMessageDeliveryAndRecyclingThreadedBetterTest::organizeThreads()
{
    //next line must be before the .moveToThreads below
    m_Business->organizeThreads(m_ClientsHelper);

    m_BusinessThread = new QThread();
    m_Business->moveToThread(m_BusinessThread);

    m_ClientsHelperThread = new QThread();
    m_ClientsHelper->moveToThread(m_ClientsHelperThread);
}
void rpcMessageDeliveryAndRecyclingThreadedBetterTest::startThreads()
{
    m_BusinessThread->start();
    m_ClientsHelperThread->start();

    //bitcoin's thread was started in the call to m_Business->organizeThreads(m_ClientsHelper). i thought about doing it right here as a QMetaObject::invokeMethod... but that just seems easier and cleaner. i also could have done a synchronous call just before starting m_BusinessThread right here... but thought that is ugly.
}
