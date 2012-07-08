#include "rpcbusinessimpl.h"

RpcBusinessImpl::RpcBusinessImpl(QObject *parent) :
    IRpcBusiness(parent)
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
void RpcBusinessImpl::setParentForEveryBroadcastDispenser(IRpcClientsHelper *clientsHelper)
{
    clientsHelper->setBroadcastDispenserParentForPendingBalanceAddedMessages(m_Bitcoin);
    //etc
}
void RpcBusinessImpl::organizeBackendThreads()
{
    m_BitcoinThread = new QThread();

    m_Bitcoin->moveToThread(m_BitcoinThread);
    //etc
}
void RpcBusinessImpl::startBackendThreads()
{
    m_BitcoinThread->start();
    //etc
}
void RpcBusinessImpl::init()
{
    QMetaObject::invokeMethod(m_Bitcoin, "init", Qt::QueuedConnection);
}
void RpcBusinessImpl::start()
{
    QMetaObject::invokeMethod(m_Bitcoin, "start", Qt::QueuedConnection);
}
void RpcBusinessImpl::stop()
{
    QMetaObject::invokeMethod(m_Bitcoin, "stop", Qt::QueuedConnection);
}
