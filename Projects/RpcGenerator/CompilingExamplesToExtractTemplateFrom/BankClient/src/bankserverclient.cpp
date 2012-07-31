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
    connect(signalEmitter, SIGNAL(createBankAccountCompleted(ClientCreateBankAccountMessage*)), this, SLOT(handleCreateBankAccountCompleted(ClientCreateBankAccountMessage*)));
    connect(signalEmitter, SIGNAL(getAddFundsKeyCompleted(ClientGetAddFundsKeyMessage*)), this, SLOT(handleGetAddFundsKeyCompleted(ClientGetAddFundsKeyMessage*)));

    //Action Errors
    connect(signalEmitter, SIGNAL(createBankAccountFailedUsernameAlreadyExists(ClientCreateBankAccountMessage*)), this, SLOT(handleCreateBankAccountFailedUsernameAlreadyExists(ClientCreateBankAccountMessage*)));
    connect(signalEmitter, SIGNAL(createBankAccountFailedPersistError(ClientCreateBankAccountMessage*)), this, SLOT(handleCreateBankAccountFailedPersistError(ClientCreateBankAccountMessage*)));
    connect(signalEmitter, SIGNAL(getAddFundsKeyFailedUsernameDoesntExist(ClientGetAddFundsKeyMessage*)), this, SLOT(handleGetAddFundsKeyFailedUsernameDoesntExist(ClientGetAddFundsKeyMessage*)));
    connect(signalEmitter, SIGNAL(getAddFundsKeyFailedUseExistingKeyFirst(ClientGetAddFundsKeyMessage*)), this, SLOT(handleGetAddFundsKeyFailedUseExistingKeyFirst(ClientGetAddFundsKeyMessage*)));
    connect(signalEmitter, SIGNAL(getAddFundsKeyFailedWaitForPendingToBeConfirmed(ClientGetAddFundsKeyMessage*)), this, SLOT(handleGetAddFundsKeyFailedWaitForPendingToBeConfirmed(ClientGetAddFundsKeyMessage*)));

    //Broadcasts
    connect(signalEmitter, SIGNAL(pendingBalanceDetected(ClientPendingBalanceDetectedMessage*)), this, SLOT(handlePendingBalanceDetected(ClientPendingBalanceDetectedMessage*)));
    connect(signalEmitter, SIGNAL(confirmedBalanceDetected(ClientConfirmedBalanceDetectedMessage*)), this, SLOT(handleConfirmedBalanceDetected(ClientConfirmedBalanceDetectedMessage*)));
}
void BankServerClient::init()
{
    //TODOmb: if we ever add a backend object, we'd send a queued init to it right here.. and also need a daisyChain() method to get it's initialized() signal
    emit d("BankServerClient received init message");
    emit initialized();
}
void BankServerClient::start()
{
    emit d("BankServerClient received start message");
    emit started();
}
void BankServerClient::stop()
{
    emit d("BankServerClient received stopped message");
    emit stopped();
}
void BankServerClient::handleCreateBankAccountCompleted(ClientCreateBankAccountMessage *createBankAccountMessage)
{
    emit d(QString("create bank account message completed for user: ") + createBankAccountMessage->Username);
    createBankAccountMessage->doneWithMessage();
}
void BankServerClient::handleGetAddFundsKeyCompleted(ClientGetAddFundsKeyMessage *getAddFundsKeyMessage)
{
    emit d(QString("get add funds key message completed for user: ") + getAddFundsKeyMessage->Username + QString(" with key: ") + getAddFundsKeyMessage->AddFundsKey);
    getAddFundsKeyMessage->doneWithMessage();
}
void BankServerClient::handleCreateBankAccountFailedUsernameAlreadyExists(ClientCreateBankAccountMessage *createBankAccountMessage)
{
    emit d(QString("create bank account message failed, username already exists for user: ") + createBankAccountMessage->Username);
    createBankAccountMessage->doneWithMessage();
}
void BankServerClient::handleCreateBankAccountFailedPersistError(ClientCreateBankAccountMessage *createBankAccountMessage)
{
    emit d(QString("create bank account message failed, persist error for user: ") + createBankAccountMessage->Username);
    createBankAccountMessage->doneWithMessage();
}
void BankServerClient::handleGetAddFundsKeyFailedUsernameDoesntExist(ClientGetAddFundsKeyMessage *getAddFundsKeyMessage)
{
    emit d(QString("get add funds key message failed, username doesn't exist' for user: ") + getAddFundsKeyMessage->Username);
    getAddFundsKeyMessage->doneWithMessage();
}
void BankServerClient::handleGetAddFundsKeyFailedUseExistingKeyFirst(ClientGetAddFundsKeyMessage *getAddFundsKeyMessage)
{
    emit d(QString("get add funds key message failed, use existing key first for user: ") + getAddFundsKeyMessage->Username + QString(" with key: ") + getAddFundsKeyMessage->AddFundsKey); //TODOreq: determine if an action returning as an ERROR should even be able to specify anything. like do we get the 'existing key' from the server via the message or from our local cache?? all up to me really
    getAddFundsKeyMessage->doneWithMessage();
}
void BankServerClient::handleGetAddFundsKeyFailedWaitForPendingToBeConfirmed(ClientGetAddFundsKeyMessage *getAddFundsKeyMessage)
{
    emit d(QString("get add funds key message failed, wait for pending to be confirmed for user: ") + getAddFundsKeyMessage->Username + QString(" with key: ") + getAddFundsKeyMessage->AddFundsKey);
    getAddFundsKeyMessage->doneWithMessage();
}
void BankServerClient::handlePendingBalanceDetected(ClientPendingBalanceDetectedMessage *pendingBalanceDetectedMessage)
{
    emit d(QString("pending balance detected for user: ") + pendingBalanceDetectedMessage->Username + QString(" with amount: ") + QString::number(pendingBalanceDetectedMessage->PendingBalance, 'f', 8));
    pendingBalanceDetectedMessage->doneWithMessage();
}
void BankServerClient::handleConfirmedBalanceDetected(ClientConfirmedBalanceDetectedMessage *confirmedBalanceDetectedMessage)
{
    emit d(QString("confirmed balance detected for user: ") + confirmedBalanceDetectedMessage->Username + QString(" with amount: ") + QString::number(confirmedBalanceDetectedMessage->ConfirmedBalance, 'f', 8));
    confirmedBalanceDetectedMessage->doneWithMessage();
}
void BankServerClient::simulateCreateBankAccountAction(QString username)
{
    ClientCreateBankAccountMessage *createBankAccountMessage = m_CreateBankAccountMessageDispenser->getNewOrRecycled();
    createBankAccountMessage->Username = username;
    emit d(QString("SIMULATING create bank account: ") + createBankAccountMessage->Username);
    createBankAccountMessage->deliver();
}
void BankServerClient::simulateGetAddFundsKeyAction(QString username)
{
    ClientGetAddFundsKeyMessage *getAddFundsKeyMessage = m_GetAddFundsKeyMessageDispenser->getNewOrRecycled();
    getAddFundsKeyMessage->Username = username;
    emit d(QString("SIMULATING get add funds key: ") + getAddFundsKeyMessage->Username);
    getAddFundsKeyMessage->deliver();
}
