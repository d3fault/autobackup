#include "rpcbankserverimpl.h"

RpcBankServerImpl::RpcBankServerImpl(QObject *parent) :
    IRpcBankServer(parent)
{
    m_Bitcoin = new BitcoinHelper();

    //daisy-chain initailization
    connect(m_Bitcoin, SIGNAL(initialized()), this, SIGNAL(initialized()));
    //etc

    //daisy-chain starting
    connect(m_Bitcoin, SIGNAL(started()), this, SIGNAL(started()));
    //etc

    //daisy-chain stopping
    connect(m_Bitcoin, SIGNAL(stopped()), this, SIGNAL(stopped()));
    //etc

    //TODO: the above "etc"s would requires a splitting of the connect, extending/modifying the daisy-chain
}
void RpcBankServerImpl::takeOwnershipOfAllBroadcastDispensers(IRpcBankServerClientsHelper *rpcBankServerClientsHelper)
{
    m_Bitcoin->takeOwnershipOfApplicableBroadcastDispensers(rpcBankServerClientsHelper);
    //etc

    //in this method it is the rpc generator user's responsibility to assign a parent for every broadcast. they ask themself: 'who (what object/thread) will be using this message dispenser?'
    //they only have to guarantee that it hasn't been .moveToThread'd yet.. but we provide a pure virtual to encourage that (and call it for them)
    //we also check their work, but only making sure that every broadcast dispenser has a parent assigned
}
void RpcBankServerImpl::moveBackendBusinessObjectsToTheirOwnThreadsAndStartThem()
{
    m_BitcoinThread = new QThread();
    m_Bitcoin->moveToThread(m_BitcoinThread);
    m_BitcoinThread->start();
    //etc
}
void RpcBankServerImpl::init()
{
    QMetaObject::invokeMethod(m_Bitcoin, "init", Qt::QueuedConnection);
}
void RpcBankServerImpl::start()
{
    QMetaObject::invokeMethod(m_Bitcoin, "start", Qt::QueuedConnection);
}
void RpcBankServerImpl::stop()
{
    QMetaObject::invokeMethod(m_Bitcoin, "stop", Qt::QueuedConnection);
}
void RpcBankServerImpl::createBankAccount(CreateBankAccountMessage *createBankAccountMessage)
{
}
