#include "rpcbankserverhelper.h"

RpcBankServerHelper::RpcBankServerHelper(QObject *parent)
    : IAcceptRpcBankServerClientActionDeliveries_AND_IEmitBroadcastsForSignalRelayHack(parent), m_RpcBankServerClientProtocolKnowerFactory(this), m_MultiServerClientAbstraction(&m_RpcBankServerClientProtocolKnowerFactory, this)
{
    RpcBankServerClientProtocolKnowerFactory::setSignalRelayHackEmitter(this);
    //m_Transporter = new SslTcpClientAndBankServerProtocolKnower();

    //m_ActionDispensers = new RpcBankServerClientActionDispensers(m_Transporter);
    //m_BroadcastDispensers = new RpcBankServerClientBroadcastDispensers(m_Transporter); //we (client) don't need a destination, but since it's shared code with the server we just deal with it and make our destination forward to recycling

    m_ActionDispensers = new RpcBankServerClientActionDispensers(this);

    //m_BroadcastDispensers = new RpcBankServerClientBroadcastDispensers(this);
    //TODOreq: not sure where Broadcast Dispensers gets new'd because I'm not sure where ActionDispensers gets new'd in the server equivalent of this code...

#if 0
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
#endif
}
#if 0
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
#endif
void RpcBankServerHelper::initialize(MultiServerClientsAbstractionArgs multiServerClientAbstractionArgs)
{
    emit d("RpcBankServerHelper received initialize message");
    //TODOreq: appears to make sense that the .initailize call below should return a bool and be checked before setting m_Initialized to true...
    m_MultiServerClientAbstraction.initializeAndStartConnections(multiServerClientAbstractionArgs); //TODOreq: can't start here because we don't know if all action dispensers are claimed yet (emitInitializedSignalIfReady)!!!! Fuck man I need to sleep on this design and meh this merge is going to take forever but at least most of it is relatively easy... TODO LEFT OFF
    m_Initialized = true;
    emitInitializedSignalIfReady();
}
void RpcBankServerHelper::handleBusinessDoneClaimingActionDispensersAndConnectingToBroadcastSignals()
{
    emitInitializedSignalIfReady();
}
void RpcBankServerHelper::startAllThatHaveBeenInitialized()
{
    emit d("RpcBankServerHelper received start message");
    m_MultiServerClientAbstraction.start();
    emit started();
}
void RpcBankServerHelper::stopAll()
{
    emit d("RpcBankServerHelper received stop message");
    m_MultiServerClientAbstraction.stop();
    emit stopped();
}
