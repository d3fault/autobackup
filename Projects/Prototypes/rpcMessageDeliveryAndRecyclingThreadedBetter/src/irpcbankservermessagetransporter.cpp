#include "irpcbankservermessagetransporter.h"

#include "rpcbankserverclientshelper.h"
#include "messageDispensers/actions/createbankaccountmessagedispenser.h"

IRpcBankServerMessageTransporter::IRpcBankServerMessageTransporter()
{ }
void IRpcBankServerMessageTransporter::takeOwnershipOfAllActionDispensers(RpcBankServerClientsHelper *rpcBankServerClientsHelper)
{
    m_CreateBankAccountMessageDispenser = rpcBankServerClientsHelper->takeOwnershipOfCreateBankAccountMessageDispenserRiggedForDelivery(this);
    //we could also do dispenser->setDestinationObject(this)... but takeOwnership will do it for us to remain uniform with broadcast dispensers and to simplify code generation
    //not that it matters, just a mental note: for actions, the dispenser owner and the message destination are both this same (this)

    connect(m_CreateBankAccountMessageDispenser, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
}
void IRpcBankServerMessageTransporter::createBankAccountCompleted()
{
    CreateBankAccountMessage *cbam = static_cast<CreateBankAccountMessage*>(sender());
    //write to network

    emit d(QString("create bank account completed (") + cbam->Username + QString("), we should be on network thread: ") + QString::number(QThread::currentThreadId()));


    cbam->doneWithMessage();
}
void IRpcBankServerMessageTransporter::createBankAccountFailedUsernameAlreadyExists()
{
    emit d(QString("create bank account failed username already exists, we should be on network thread: ") + QString::number(QThread::currentThreadId()));

    CreateBankAccountMessage *cbam = static_cast<CreateBankAccountMessage*>(sender());
    //write to network
    cbam->doneWithMessage();
}
void IRpcBankServerMessageTransporter::createBankAccountFailedPersistError()
{
    emit d(QString("create bank account failed persist error, we should be on network thread: ") + QString::number(QThread::currentThreadId()));

    CreateBankAccountMessage *cbam = static_cast<CreateBankAccountMessage*>(sender());
    //write to network
    cbam->doneWithMessage();
}
void IRpcBankServerMessageTransporter::pendingBalanceDetected()
{
    PendingBalanceAddedMessage *pbam = static_cast<PendingBalanceAddedMessage*>(sender());

    emit d(QString("pending balance detected (") + pbam->Username + QString("-") + QString::number(pbam->PendingBalance) + QString("), we should be on network thread: ") + QString::number(QThread::currentThreadId()));

    pbam->doneWithMessage();
}
void IRpcBankServerMessageTransporter::init()
{
    emit d(QString("received init on what should be the network thread: ") + QString::number(QThread::currentThreadId()));
    emit initialized();
}
void IRpcBankServerMessageTransporter::start()
{
    emit d(QString("received start on what should be the network thread: ") + QString::number(QThread::currentThreadId()));
    emit started();
}
void IRpcBankServerMessageTransporter::stop()
{
    emit d(QString("received stop on what should be the network thread: ") + QString::number(QThread::currentThreadId()));
    emit stopped();
}
void IRpcBankServerMessageTransporter::simulateCreateBankAccount()
{
    emit d(QString("received simulate create bank account on what should be network thread: ") + QString::number(QThread::currentThreadId()));

    CreateBankAccountMessage *cbam = m_CreateBankAccountMessageDispenser->getNewOrRecycled();
    cbam->Username = QString("username@") + QDateTime::currentDateTime().toString();
    emit createBankAccount(cbam);
}
