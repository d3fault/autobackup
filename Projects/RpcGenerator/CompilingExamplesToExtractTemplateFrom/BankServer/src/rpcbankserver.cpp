#include "rpcbankserver.h"

RpcBankServer::RpcBankServer(QObject *parent)
    : QObject(parent), m_RpcBankServerClientsHelper(0), m_InitializeSlotCalled(false), m_BitcoinInstantiated(false), m_BankDbInstantiated(false), m_BitcoinDoneUsingRpcBankServerClientsHelper(false), m_BankDbDoneUsingRpcBankServerClientsHelper(false), m_DoneUsingRpcBankServerDuringInitialization(false), m_BitcoinStarted(false), m_BankDbStarted(false)
{
    connect(&m_BitcoinThreadHelper, SIGNAL(objectIsReadyForConnectionsOnly()), SLOT(handleBitcoinInstantiated()));
    connect(&m_BankDbHelperThreadHelper, SIGNAL(objectIsReadyForConnectionsOnly()), SLOT(handleBankInstantiated()));

    m_BitcoinThreadHelper.start();
    m_BankDbHelperThreadHelper.start();

    connect(this, SIGNAL(doneUsingRpcBankServerClientsHelperDuringInitialization()), this, SLOT(emitInitializedIfDoneWithAllRpcServices()));
    //etc for each rpc service
}
#if 0
void RpcBankServer::connectRpcBankServerClientActionRequestSignalsToBankServerImplSlots(IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptAllDeliveries *actionRequestSignalEmitter)
{
    connect(actionRequestSignalEmitter, SIGNAL(createBankAccount(CreateBankAccountMessage*)), m_BankDbHelperThreadHelper, SLOT(createBankAccount(CreateBankAccountMessage*)));
    connect(actionRequestSignalEmitter, SIGNAL(getAddFundsKey(GetAddFundsKeyMessage*)), m_BankDbHelperThreadHelper, SLOT(getAddFundsKey(GetAddFundsKeyMessage*)));

    //TODOreq: create bank account definitely needs to go to the db, but where should GetAddFundsKey go?
    //even if it goes to both db and bitcoin, we should set up signal/slot chaining to achieve it. the two backend objects should remain unaware of each other
    //just making it go to db for now

    //but i can always do chaining from db -> bitcoin and bitcoin -> db etc. like createBankAccountStep1() in db and createBankAccountStep2() in bitcoin ----------- and possibly a createBankAccountStep3() in db again to verify the bitcoin shit? idfk. the point is that _WE_ never receive a message.. we only manage the connections etc (as well as init/start/stop) to the backend objects. the rpc code uses our pure virtuals to set everything up in the right order. fugly imo
}
#endif
void RpcBankServer::initializeAllBackends()
{
    emit bitcoinInitializeRequested(m_RpcBankServerClientsHelper);
    emit bankDbInitializeRequested(m_RpcBankServerClientsHelper); //if they all have the same parameters, I can just connect them all to an 'initializeRequested' signal. However not all backends will need/want all rpc service pointers. In fact BankDb doesn't even need this bank server one. I could have set up his connections for him (that's all he uses it for, as opposed to bitcoin who claims broadcast dispensers on it etc)
    //etc
}
void RpcBankServer::initialize(RpcBankServerClientsHelper *rpcBankServerClientsHelper)
{
    //TODOreq: not sure if it matters, but the business should be destructed before the clientshelper? maybe stop (both), sync, then teardown business then teardown clients helper? i'm thinking it might matter because our backends "own" some broadcast dispensers that *MIGHT* go out of scope/be-deleted if/when i delete clients helper? now that i think about it, they're pointers so i doubt it (TODOreq: delete them then!), but still seems like better form to not have dangling pointers or something idfk

    emit d("RpcBankServer received initialize message");
    m_InitializeSlotCalled = true;
    m_RpcBankServerClientsHelper = rpcBankServerClientsHelper;
    connect(this, SIGNAL(doneUsingRpcBankServerClientsHelperDuringInitialization()), rpcBankServerClientsHelper, SLOT(handleDoneClaimingBroadcastDispensers()));

    initializeAllBackendsIfIAmInitializedAndAllBackendThreadsAreInstantiated();
}
void RpcBankServer::start()
{
    emit d("RpcBankServer received start message");
    emit startBackendsRequested();
}
void RpcBankServer::stop()
{
    emit d("RpcBankServer received stop message");
    emit stopBackendsRequested();
}
void RpcBankServer::handleBitcoinInstantiated()
{
    RpcBitcoinHelper *rpcBitcoinHelper = m_BitcoinThreadHelper.getObjectPointerForConnectionsOnly();
    connect(this, SIGNAL(bitcoinInitializeRequested(RpcBankServerClientsHelper*)), rpcBitcoinHelper, SLOT(initialize(RpcBankServerClientsHelper*)));
    connect(rpcBitcoinHelper, SIGNAL(initialized()), this, SLOT(handleBitcoinInitialized()));
    connect(rpcBitcoinHelper, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
    connect(this, SIGNAL(startBackendsRequested()), rpcBitcoinHelper, SLOT(start())); //etc for each backend (in their 'instantiated' slot)
    connect(rpcBitcoinHelper, SIGNAL(started()), this, SLOT(handleBitcoinStarted()));
    connect(this, SIGNAL(stopBackendsRequested()), rpcBitcoinHelper, SLOT(stop())); //etc for each backend (in their 'instantiated' slot)
    connect(rpcBitcoinHelper, SIGNAL(stopped()), this, SLOT(handleBitcoinStopped()));

    //connect Actions to Slots
    //and Signals to Broadcasts <-- Actually broadcasts are already set up when we "claim" them :-P

    //or we could have the bitcoin class do it himself during initialization? SINCE IT IS rpc specific after all...

    //I see them both as equal except for that we might have custom object types (or we.. DO.. i should say?) and I might as well make it so only the backend object has to worry about/include them? It really doesn't make any difference since slots are public and so are signals!

    //So for bitcoin it doesn't matter because there ARE NO CONNECTIONS (broadcasts have their deliver() already rigged), but for BankDbHelper it does factor in. They only really need to know about the specific messages though. Since 'this' ALREADY knows about the ClientsHelper, we could do the connections in here so that BankDbHelper never has to know about the ClientsHelper. Gah it doesn't make much of a difference, just the difference between passing the ClientsHelper into BankDbHelper's initialize() method or not...


    //DEBUG:
    connect(this, SIGNAL(simulatePendingBalanceDetectedBroadcastRequested()), rpcBitcoinHelper, SLOT(simulatePendingBalanceDetectedBroadcast()));
    connect(this, SIGNAL(simulateConfirmedBalanceDetectedBroadcastRequested()), rpcBitcoinHelper, SLOT(simulateConfirmedBalanceDetectedBroadcast()));


    m_BitcoinInstantiated = true;
    initializeAllBackendsIfIAmInitializedAndAllBackendThreadsAreInstantiated();
}
void RpcBankServer::handleBitcoinInitialized()
{
    m_BitcoinDoneUsingRpcBankServerClientsHelper = true;
    //etc for each rpc service that bitcoin uses
    emitDoneUsingRpcBankServerClientsHelperDuringInitializationIfAllRelevantBackendsAreDoneUsingIt();
}
void RpcBankServer::handleBitcoinStarted()
{
    m_BitcoinStarted = true;
    emitStartedIfAllBackendsStarted();
}
void RpcBankServer::handleBitcoinStopped()
{
    m_BitcoinStarted = false;
    emitStoppedIfAllBackendsStopped();
}
void RpcBankServer::handleBankInstantiated()
{
    BankDbHelper *bankDbHelper = m_BankDbHelperThreadHelper.getObjectPointerForConnectionsOnly();

    connect(this, SIGNAL(bankDbInitializeRequested(RpcBankServerClientsHelper*)), bankDbHelper, SLOT(initialize(RpcBankServerClientsHelper*)));
    connect(bankDbHelper, SIGNAL(initialized()), this, SLOT(handleBankDbInitialized()));
    connect(bankDbHelper, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
    connect(this, SIGNAL(startBackendsRequested()), bankDbHelper, SLOT(start()));
    connect(bankDbHelper, SIGNAL(started()), this, SLOT(handleBankDbStarted()));
    connect(this, SIGNAL(stopBackendsRequested()), bankDbHelper, SLOT(stop()));
    connect(bankDbHelper, SIGNAL(stopped()), this, SLOT(handleBankDbStopped()));


    //connect Rpc Service's Actions to BankDb slots
    connect(m_RpcBankServerClientsHelper, SIGNAL(createBankAccountRequested(CreateBankAccountMessage*)), bankDbHelper, SLOT(createBankAccount(CreateBankAccountMessage*)));
    connect(m_RpcBankServerClientsHelper, SIGNAL(getAddFundsKeyRequested(GetAddFundsKeyMessage*)), bankDbHelper, SLOT(getAddFundsKey(GetAddFundsKeyMessage*)));
    //etc for each rpc service. also bitcoin backend (and/or ALL backends) would/might have some but doesn't atm

    m_BankDbInstantiated = true;
    initializeAllBackendsIfIAmInitializedAndAllBackendThreadsAreInstantiated();
}
void RpcBankServer::handleBankDbInitialized()
{
    m_BankDbDoneUsingRpcBankServerClientsHelper = true;
    //etc for each rpc service that bankdb uses
    emitDoneUsingRpcBankServerClientsHelperDuringInitializationIfAllRelevantBackendsAreDoneUsingIt();
}
void RpcBankServer::handleBankDbStarted()
{
    m_BankDbStarted = true;
    emitStartedIfAllBackendsStarted();
}
void RpcBankServer::handleBankDbStopped()
{
    m_BankDbStarted = false;
    emitStoppedIfAllBackendsStopped();
}
void RpcBankServer::emitInitializedIfDoneWithAllRpcServices()
{
    if(m_DoneUsingRpcBankServerDuringInitialization /*&& etc for each rpc service */)
    {
        emit initialized();
    }
}
