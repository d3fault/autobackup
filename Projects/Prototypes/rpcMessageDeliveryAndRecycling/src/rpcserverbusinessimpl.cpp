#include "rpcserverbusinessimpl.h"

RpcServerBusinessImpl::RpcServerBusinessImpl(QObject *parent) :
    QObject(parent)
{

}
void RpcServerBusinessImpl::createBankAccount(CreateBankAccountMessage *createBankAccountMessage)
{
    //do some shit in the db

    //if it works, just deliver. TODOreq: failed reasons cases
    createBankAccountMessage->deliver(); //triggers the response
}
void RpcServerBusinessImpl::init()
{
    m_Bitcoin = new BitcoinHelperAndBroadcastMessageDispenserUser(this->broadcastDispensers());
    m_BitcoinThread = new QThread();
    m_Bitcoin->moveToThread(m_BitcoinThread);
    m_BitcoinThread->start();

    //meh, this is ugly but we know the thread isn't busy doing something else
    m_Bitcoin->moveBroadcastDispensersToThread(m_BitcoinThread);


    QMetaObject::invokeMethod(m_Bitcoin, "startDebugTimer", Qt::QueuedConnection); //normally would have this schedule like once every 10 seconds max or whatever that formula was
}
