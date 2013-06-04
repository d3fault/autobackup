#include "rpcbankserverhelper.h"

#include "ssltcpclientandbankserverprotocolknower.h"

RpcBankServerHelper::RpcBankServerHelper(IRpcBankServerClientBusiness *rpcBankServerClient)
    : m_RpcBankServerClient(rpcBankServerClient)
{
    m_Transporter = new SslTcpClientAndBankServerProtocolKnower();

    m_ActionDispensers = new RpcBankServerClientActionDispensers(m_Transporter);
    m_BroadcastDispensers = new RpcBankServerClientBroadcastDispensers(m_Transporter); //we (client) don't need a destination, but since it's shared code with the server we just deal with it and make our destination forward to recycling

    m_RpcBankServerClient->setActionDispensers(m_ActionDispensers);
    m_Transporter->setBroadcastDispensers(m_BroadcastDispensers);

    m_RpcBankServerClient->instructBackendObjectsToClaimRelevantDispensers();

    if(!m_ActionDispensers->everyDispenserIsCreated())
        return; //TODOreq fail properly, see RpcBankServerClientsHelper on server

    m_Transporter->takeOwnershipOfBroadcastsAndSetupDelivery();

    m_RpcBankServerClient->moveBackendBusinessObjectsToTheirOwnThreadsAndStartTheThreads();

    moveTransporterToItsOwnThreadAndStartTheThread();

    moveBusinessToItsOwnThreadAndStartTheThread();

    m_RpcBankServerClient->connectRpcBankServerSignalsToBankServerClientImplSlots(m_Transporter);
    daisyChainInitStartStopConnections();

    connect(m_Transporter, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
}
void RpcBankServerHelper::daisyChainInitStartStopConnections()
{
    //daisy-chain connections
    //init
    connect(m_RpcBankServerClient, SIGNAL(initialized()), m_Transporter, SLOT(init()));
    connect(m_Transporter, SIGNAL(initialized()), this, SIGNAL(initialized()));

    //start
    connect(m_RpcBankServerClient, SIGNAL(started()), m_Transporter, SLOT(start()));
    connect(m_Transporter, SIGNAL(started()), this, SIGNAL(started()));

    //stop
    connect(m_Transporter, SIGNAL(stopped()), m_RpcBankServerClient, SLOT(stop()));
    connect(m_RpcBankServerClient, SIGNAL(stopped()), this, SIGNAL(stopped()));
}
void RpcBankServerHelper::init()
{
    //old:
    //emit d("RpcBankServerHelper received init message");
    //QMetaObject::invokeMethod(m_RpcBankServerClient, "init", Qt::QueuedConnection);

    //TODOreq:

    //then
    emit initialized();

    //TODOreq: same redo from old->new for start/stop as well
}
void RpcBankServerHelper::start()
{
    //emit d("RpcBankServerHelper received start message");
    //QMetaObject::invokeMethod(m_RpcBankServerClient, "start", Qt::QueuedConnection);
}
void RpcBankServerHelper::stop()
{
    //emit d("RpcBankServerHelper received stop message");
    //QMetaObject::invokeMethod(m_Transporter, "stop", Qt::QueuedConnection);
}
void RpcBankServerHelper::moveTransporterToItsOwnThreadAndStartTheThread()
{
    m_TransporterThread = new QThread();
    m_Transporter->moveToThread(m_TransporterThread);
    m_TransporterThread->start();
}
void RpcBankServerHelper::moveBusinessToItsOwnThreadAndStartTheThread()
{
    m_BusinessThread = new QThread();
    m_RpcBankServerClient->moveToThread(m_BusinessThread);
    m_BusinessThread->start();
}
void RpcBankServerHelper::initialize(MultiServerClientAbstractionArgs multiServerClientAbstractionArgs)
{
    emit d("RpcBankServerHelper received initialize message");
    //TODOreq: appears to make sense that the .initailize call below should return a bool and be checked before setting m_Initialized to true...
    m_MultiServerAbstraction.initialize(multiServerClientAbstractionArgs);
    m_Initialized = true;
    emitInitializedSignalIfReady();
}
void RpcBankServerHelper::handleDoneClaimingActionDispensers()
{
    emitInitializedSignalIfReady();
}
