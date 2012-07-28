#include "rpcbankserverhelper.h"

#include "ssltcpclientandbankserverprotocolknower.h"

RpcBankServerHelper::RpcBankServerHelper(IRpcBankServerClientBusiness *rpcBankServerClient)
    : m_RpcBankServerClient(rpcBankServerClient)
{
    m_Transporter = new SslTcpClientAndBankServerProtocolKnower();

    m_ActionDispensers = new RpcBankServerActionDispensers(m_Transporter);
    m_BroadcastDispensers = new RpcBankServerBroadcastDispensers(m_Transporter);

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
}
void RpcBankServerHelper::daisyChainInitStartStopConnections()
{
    connect(m_RpcBankServerClient, SIGNAL(initialized()), m_Transporter, SLOT(init()));
    connect(m_RpcBankServerClient, SIGNAL(started()), m_Transporter, SLOT(start()));
    connect(m_Transporter, SIGNAL(stopped()), m_RpcBankServerClient, SLOT(stop()));
}
void RpcBankServerHelper::init()
{
    QMetaObject::invokeMethod(m_RpcBankServerClient, "init", Qt::QueuedConnection);
}
void RpcBankServerHelper::start()
{
    QMetaObject::invokeMethod(m_RpcBankServerClient, "start", Qt::QueuedConnection);
}
void RpcBankServerHelper::stop()
{
    QMetaObject::invokeMethod(m_Transporter, "stop", Qt::QueuedConnection);
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
