#include "rpcbankserverclientshelper.h"

#include "ssltcpserverandbankserverprotocolknower.h"

RpcBankServerClientsHelper::RpcBankServerClientsHelper(IRpcBankServerBusiness *rpcBankServer)
    : m_RpcBankServer(rpcBankServer)
{
    m_Transporter = new SslTcpServerAndBankServerProtocolKnower();

    //as opposed to, on the rpc client: m_Transporter = new SslTcpSocketAndBankServerClientProtocolKnower
    //IBankServerProtocolKnower is a layer of separation between the protocol and the network, just like before
    //IBankServerClientProtocolKnower exists too, serves the same purpose... but is comprised of different signatures and slots instead of signals vice versa etc. they APPEAR similar but are not.

    m_ActionDispensers = new RpcBankServerActionDispensers(m_Transporter);
    m_BroadcastDispensers = new RpcBankServerBroadcastDispensers(m_Transporter);

    m_RpcBankServer->setBroadcastDispensers(m_BroadcastDispensers);
    m_Transporter->setActionDispensers(m_ActionDispensers);

    m_RpcBankServer->instructBackendObjectsToClaimRelevantDispensers();

    if(!m_BroadcastDispensers->everyDispenserIsCreated())
        return; //TODOreq: return false. even though shit, it's called from a constructor. maybe just set up an initFailed flag and throw an error?

    m_Transporter->takeOwnershipOfActionsAndSetupDelivery();

    m_RpcBankServer->moveBackendBusinessObjectsToTheirOwnThreadsAndStartTheThreads();

    //transporter is OUR backend business object, similar to above call
    moveTransporterToItsOwnThreadAndStartTheThread();

    moveBusinessToItsOwnThreadAndStartTheThread(); //similar to above call though not the exact same. this is the business object itself now finally, not it's backend objects

    actualRpcConnections();
    daisyChainInitStartStopConnections();
}
void RpcBankServerClientsHelper::actualRpcConnections()
{
    connect(m_Transporter, SIGNAL(createBankAccount(CreateBankAccountMessage*)), m_RpcBankServer, SLOT(createBankAccount(CreateBankAccountMessage*)));
    connect(m_Transporter, SIGNAL(getAddFundsKey(GetAddFundsKeyMessage*)), m_RpcBankServer, SLOT(getAddFundsKey(GetAddFundsKeyMessage*)));
}
void RpcBankServerClientsHelper::daisyChainInitStartStopConnections()
{
    //daisy-chain connections
    //init
    connect(m_RpcBankServer, SIGNAL(initialized()), m_Transporter, SLOT(init()));
    connect(m_Transporter, SIGNAL(initialized()), this, SIGNAL(initialized()));

    //start
    connect(m_RpcBankServer, SIGNAL(started()), m_Transporter, SLOT(start()));
    connect(m_Transporter, SIGNAL(started()), this, SIGNAL(started()));

    //stop -- reverse order
    connect(m_Transporter, SIGNAL(stopped()), m_RpcBankServer, SLOT(stop()));
    connect(m_RpcBankServer, SIGNAL(stopped()), this, SIGNAL(stopped()));
}
void RpcBankServerClientsHelper::init()
{
    QMetaObject::invokeMethod(m_RpcBankServer, "init", Qt::QueuedConnection);
}
void RpcBankServerClientsHelper::start()
{
    QMetaObject::invokeMethod(m_RpcBankServer, "start", Qt::QueuedConnection);
}
void RpcBankServerClientsHelper::stop()
{
    QMetaObject::invokeMethod(m_Transporter, "stop", Qt::QueuedConnection);
}
void RpcBankServerClientsHelper::moveTransporterToItsOwnThreadAndStartTheThread()
{
    m_TransporterThread = new QThread();
    m_Transporter->moveToThread(m_TransporterThread);
    m_TransporterThread->start();
}
void RpcBankServerClientsHelper::moveBusinessToItsOwnThreadAndStartTheThread()
{
    m_BusinessThread = new QThread();
    m_RpcBankServer->moveToThread(m_BusinessThread);
    m_BusinessThread->start();
}
