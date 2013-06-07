#include "bankserverclient.h"

#include <QDateTime> //debug

BankServerClient::BankServerClient(QObject *parent)
    : QObject(parent)
{
    //instantiate backend business objects here, when they exist

    //m_AnotherRpcServerSayWtSideOfThings = new QObject(); //rpc client's backend business object?
    //TODO^ having a single project be both a client and server is a future project. don't lose track of your objective
    //simplify it just like the RemoteBankManagementTest did so there's just 2 portions

    //yes, there will be actual objects instantiated here... but i can't think of any for the simplified RemoteBankManagementTest example

    //to ease the burden of coding, i am stating that all "initialization" required to get BankServerClient in a "react-ready" state needs to be done in the constructor. Other use cases could still use custom logic to daisy chain initialize signals/slots but right now I'm already quite confused with the different times that BankServerClient and RpcBankServerHelper will be initializing. I get it now though. The client is rpc agnostic. oh my god i just figured out my problems. the gui should only talk to test! but test can and does connect the backends directly to the gui. the gui is ignorant of the business, and certain businesses are ignorant of each other! both businesses should be ignorant of the auto generated rpc code half.
    //Because of the way we are arranging the BankServerClient as react-ready, it makes sense for us to also make the test only communicate through BankServerClient, as Test receives NO GUARANTEES (though this can be hacked around, it shouldn't because the code needs to stand up to many use cases) from the BankServerClient as to how long, if and when, RpcBankServerHelper will even be instantiated/started for! So it can't logically be the one to daisy-chain the two together! Hell, it shouldn't even know about the Rpc*Helper classes... oh my duh...
    //The Rpc*Helper objects should be mere members on the Server/Client businesses.
    //It makes sense that the server business definitely be ready for the clientshelper to use it (it is it's data source)
    //But it doesn't make much sense for the client business to have to be ready for the serverhelper to use it... BECAUSE the client might be awaiting the serverhelper to be ready before marking itself ready. i guess what i mean is: ready to utilize. server business needs to be ready to be utilized by clientshelper. client business needs to be ready to be utilized by serverhelper.
    //or wait maybe i mistook it momentarily
    //the server business is ready to utilize the clientshelper/server. it is ready (data source) to do so.
    //the client business is ready to utilize the serverhelper/client. it is ready (even if just saving a pointer to the backend object to be daisy chained later) to do so. it isn't until we call start on the *helper that anything can/will happen anyways (can for client, will for server).
    //the client/server are essentially each exactly 1/2 of a "peer" btw
    //the first one uses it in a much stricter manner. it really has to be really really ready or else the system might fail. the second is like "ok so that particular connection didn't get made, whatever".
}
#if 0
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
#endif
void BankServerClient::initialize(RpcBankServerHelper *rpcBankServerHelper)
{
    //TODOreq maybe actually do stuff here
    //connect our signals to it's action slots (is this right? doesn't it work by just calling .deliver() on the message?)
    //connect it's broadcast signals to our slots (pretty sure this is right even if the above relating to actions isn't)

    //but also, TODOreq: do start/stop as well?

    //once we have backend objects, we'll do backendObject->takeOwnershipOfApplicableActionDispensers(m_ActionDispensers);
    //for now, we will take ownership for debug/testing etc

    //Claim Action Dispensers
    m_CreateBankAccountMessageDispenser = rpcBankServerHelper->actionDispensers()->takeOwnershipOfCreateBankAccountMessageDispenserRiggedForDelivery(this);
    m_GetAddFundsKeyMessageDispenser = rpcBankServerHelper->actionDispensers()->takeOwnershipOfGetAddFundsKeyMessageDispenserRiggedForDelivery(this);


    //Setup Broadcast Signals
    connect(rpcBankServerHelper, SIGNAL(pendingBalanceDetectedBroadcasted(ClientPendingBalanceDetectedMessage*)), this, SLOT(handlePendingBalanceDetected(ClientPendingBalanceDetectedMessage*)));
    connect(rpcBankServerHelper, SIGNAL(confirmedBalanceDetectedBroadcasted(ClientConfirmedBalanceDetectedMessage*)), this, SLOT(handleConfirmedBalanceDetected(ClientConfirmedBalanceDetectedMessage*)));


    //old/obsolete todo methinks:
    //TODOmb: if we ever add a backend object, we'd send a queued init to it right here.. and also need a daisyChain() method to get it's initialized() signal
    emit d("Initializing BankServerClient with rpc bank server helper pointer");

    emit initialized();
}
void BankServerClient::start()
{
    emit d("RpcBankServerClient received start message");
    emit started();
}
void BankServerClient::beginStoppingProcedure()
{
    //TODOreq: wait for shit to exit cleanly. use server solution here prolly once implemented
    //so basically it's this: client will now NOT accept any more action requests (from the "left" (gui/user in example/spec)). It can still receive broadcasts and action responses (from the right). It signals it's "right" (RpcBankServerHelper) to stop... which signals us after that's completed... and then we know we're stopped completely and emit stopped()

    emit beginStoppingProcedureRequested();
}
void BankServerClient::finishStoppingProcedure()
{
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
#if 0
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
#endif
void BankServerClient::handlePendingBalanceDetected(ClientPendingBalanceDetectedMessage *pendingBalanceDetectedMessage)
{
    //TODOreq: just like server already does, ack etc. perhaps all implicitly through the doneWithMessage call? idfk [yet] because broadcasts don't ack like actions do and i haven't coded the broadcast ack yet

    emit d(QString("BankServerClient: pending balance detected for user: ") + pendingBalanceDetectedMessage->Username + QString(" with amount: ") + QString::number(pendingBalanceDetectedMessage->PendingBalance, 'f', 8));
    pendingBalanceDetectedMessage->doneWithMessage();
}
void BankServerClient::handleConfirmedBalanceDetected(ClientConfirmedBalanceDetectedMessage *confirmedBalanceDetectedMessage)
{
    emit d(QString("BankServerClient: confirmed balance detected for user: ") + confirmedBalanceDetectedMessage->Username + QString(" with amount: ") + QString::number(confirmedBalanceDetectedMessage->ConfirmedBalance, 'f', 8));
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
