#include "bankserverclient.h"

#include <QDateTime> //debug

BankServerClient::BankServerClient()
{
    //instantiate backend business objects here

    //m_AnotherRpcServerSayWtSideOfThings = new QObject(); //rpc client's backend business object?
    //TODO^ having a single project be both a client and server is a future project. don't lose track of your objective
    //simplify it just like the RemoteBankManagementTest did so there's just 2 portions

    //yes, there will be actual objects instantiated here... but i can't think of any for the simplified RemoteBankManagementTest example
}
void BankServerClient::instructBackendObjectsToClaimRelevantDispensers()
{
    //once we have backend objects, we'll do backendObject->takeOwnershipOfApplicableActionDispensers(m_ActionDispensers);
    //for now, we will take ownership for debug/testing etc
    m_CreateBankAccountMessageDispenser = m_ActionDispensers->takeOwnershipOfCreateBankAccountMessageDispenserRiggedForDelivery(this);
    m_GetAddFundsKeyMessageDispenser = m_ActionDispensers->takeOwnershipOfGetAddFundsKeyMessageDispenserRiggedForDelivery(this);
}
void BankServerClient::moveBackendBusinessObjectsToTheirOwnThreadsAndStartTheThreads()
{
    //no backend objects atm
}
void BankServerClient::connectRpcBankServerSignalsToBankServerClientImplSlots(IEmitRpcBankServerBroadcastAndActionResponseSignalsWithMessageAsParam *signalEmitter)
{
    //Action Responses
    connect(signalEmitter, SIGNAL(createBankAccountCompleted(CreateBankAccountMessage*)), this, SLOT(handleCreateBankAccountCompleted(CreateBankAccountMessage*)));
    connect(signalEmitter, SIGNAL(getAddFundsKeyCompleted(GetAddFundsKeyMessage*)), this, SLOT(handleGetAddFundsKeyCompleted(GetAddFundsKeyMessage*)));

    //Broadcasts
    connect(signalEmitter, SIGNAL(pendingBalanceDetected(PendingBalanceDetectedMessage*)), this, SLOT(handlePendingBalanceDetected(PendingBalanceDetectedMessage*)));
    connect(signalEmitter, SIGNAL(confirmedBalanceDetected(ConfirmedBalanceDetectedMessage*)), this, SLOT(handleConfirmedBalanceDetected(ConfirmedBalanceDetectedMessage*)));
}
void BankServerClient::init()
{
    //TODOmb: if we ever add a backend object, we'd send a queued init to it right here.. and also need a daisyChain() method to get it's initialized() signal

    emit initialized();
}
void BankServerClient::start()
{
    emit started();
}
void BankServerClient::stop()
{
    emit stopped();
}
void BankServerClient::handleCreateBankAccountCompleted(CreateBankAccountMessage *createBankAccountMessage)
{
    emit d(QString("create bank account message completed for user: ") + createBankAccountMessage->Username);
    createBankAccountMessage->doneWithMessage();
}
void BankServerClient::handleGetAddFundsKeyCompleted(GetAddFundsKeyMessage *getAddFundsKeyMessage)
{
    emit d(QString("get add funds key message completed for user: ") + getAddFundsKeyMessage->Username + QString(" with key: ") + getAddFundsKeyMessage->AddFundsKey);
    getAddFundsKeyMessage->doneWithMessage();
}
void BankServerClient::handlePendingBalanceDetected(PendingBalanceDetectedMessage *pendingBalanceDetectedMessage)
{
    emit d(QString("pending balance detected for user: ") + pendingBalanceDetectedMessage->Username + QString(" with amount: ") + QString::number(pendingBalanceDetectedMessage->PendingBalance, 'f', 8));
    pendingBalanceDetectedMessage->doneWithMessage();
}
void BankServerClient::handleConfirmedBalanceDetected(ConfirmedBalanceDetectedMessage *confirmedBalanceDetectedMessage)
{
    emit d(QString("confirmed balance detected for user: ") + confirmedBalanceDetectedMessage->Username + QString(" with amount: ") + QString::number(confirmedBalanceDetectedMessage->ConfirmedBalance, 'f', 8));
    confirmedBalanceDetectedMessage->doneWithMessage();
}
void BankServerClient::simulateCreateBankAccountAction()
{
    CreateBankAccountMessage *createBankAccountMessage = m_CreateBankAccountMessageDispenser->getNewOrRecycled();
    createBankAccountMessage->Username = (QString("randomUsername@") + QDateTime::currentDateTime().toString());
    createBankAccountMessage->deliver();
}
void BankServerClient::simulateGetAddFundsKeyAction()
{
    GetAddFundsKeyMessage *getAddFundsKeyMessage = m_GetAddFundsKeyMessageDispenser->getNewOrRecycled();
    getAddFundsKeyMessage->Username = (QString("randomUsername@") + QDateTime::currentDateTime().toString());
    getAddFundsKeyMessage->deliver();
}
