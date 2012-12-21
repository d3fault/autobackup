#include "rpcbankserver.h"

RpcBankServer::RpcBankServer(QObject *parent)
    : QObject(parent), m_RpcBankServerClientsHelper(0), m_Initialized(false), m_BitcoinInstantiated(false)
{
    /*m_BankDbHelperThreadHelper = new BankDbHelper();
    m_BitcoinThreadHelper = new BitcoinHelper();*/
    connect(&m_BitcoinThreadHelper, SIGNAL(objectIsReadyForConnectionsOnly()), SLOT(handleBitcoinInstantiated()));
    m_BitcoinThreadHelper.start();
}
void RpcBankServer::connectRpcBankServerClientActionRequestSignalsToBankServerImplSlots(IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptAllDeliveries *actionRequestSignalEmitter)
{
    connect(actionRequestSignalEmitter, SIGNAL(createBankAccount(CreateBankAccountMessage*)), m_BankDbHelperThreadHelper, SLOT(createBankAccount(CreateBankAccountMessage*)));
    connect(actionRequestSignalEmitter, SIGNAL(getAddFundsKey(GetAddFundsKeyMessage*)), m_BankDbHelperThreadHelper, SLOT(getAddFundsKey(GetAddFundsKeyMessage*)));

    //TODOreq: create bank account definitely needs to go to the db, but where should GetAddFundsKey go?
    //even if it goes to both db and bitcoin, we should set up signal/slot chaining to achieve it. the two backend objects should remain unaware of each other
    //just making it go to db for now

    //but i can always do chaining from db -> bitcoin and bitcoin -> db etc. like createBankAccountStep1() in db and createBankAccountStep2() in bitcoin ----------- and possibly a createBankAccountStep3() in db again to verify the bitcoin shit? idfk. the point is that _WE_ never receive a message.. we only manage the connections etc (as well as init/start/stop) to the backend objects. the rpc code uses our pure virtuals to set everything up in the right order. fugly imo
}
void RpcBankServer::initializeAllBackends()
{
    emit bitcoinInitializeRequested(m_RpcBankServerClientsHelper);
}
void RpcBankServer::initialize(RpcBankServerClientsHelper * rpcBankServerClientsHelper)
{
    //TODOreq: not sure if it matters, but the business should be destructed before the clientshelper? maybe stop (both), sync, then teardown business then teardown clients helper? i'm thinking it might matter because our backends "own" some broadcast dispensers that *MIGHT* go out of scope/be-deleted if/when i delete clients helper? now that i think about it, they're pointers so i doubt it (TODOreq: delete them then!), but still seems like better form to not have dangling pointers or something idfk

    emit d("BankServer received init message");
    m_Initialized = true;
    m_RpcBankServerClientsHelper = rpcBankServerClientsHelper;
    connect(this, SIGNAL(doneUsingRpcBankServerClientsHelperDuringInitialization()), rpcBankServerClientsHelper, SLOT(handleDoneClaimingBroadcastDispensers()));

    initializeAllBackendsIfIAmInitializedAndAllBackendThreadsAreInstantiated();
}
void RpcBankServer::start()
{
    emit d("BankServer received start message");
    QMetaObject::invokeMethod(m_BankDbHelperThreadHelper, "start", Qt::QueuedConnection);
}
void RpcBankServer::stop()
{
    emit d("BankServer received stop message");
    QMetaObject::invokeMethod(m_BitcoinThreadHelper, "stop", Qt::QueuedConnection);
}
void RpcBankServer::handleBitcoinInstantiated()
{
    m_BitcoinInstantiated = true;
    RpcBitcoinHelper *rpcBitcoinHelper = m_BitcoinThreadHelper.getObjectPointerForConnectionsOnly();
    connect(this, SIGNAL(bitcoinInitializeRequested(RpcBankServerClientsHelper*)), rpcBitcoinHelper, SLOT(initialize(RpcBankServerClientsHelper*)));
    connect(rpcBitcoinHelper, SIGNAL(initialized()), this, SLOT(handleBitcoinInitialized()));
    connect(rpcBitcoinHelper, SIGNAL(d(QString)), this, SIGNAL(d(QString)));

    //connect Actions to Slots
    //and Signals to Broadcasts <-- Actually broadcasts are already set up when we "claim" them :-P


    //DEBUG:
    connect(this, SIGNAL(simulatePendingBalanceDetectedBroadcastRequested()), rpcBitcoinHelper, SLOT(simulatePendingBalanceDetectedBroadcast()));
    connect(this, SIGNAL(simulateConfirmedBalanceDetectedBroadcastRequested()), rpcBitcoinHelper, SLOT(simulateConfirmedBalanceDetectedBroadcast()));


    initializeAllBackendsIfIAmInitializedAndAllBackendThreadsAreInstantiated();
}
void RpcBankServer::handleBitcoinInitialized()
{
    emitDoneUsingRpcBankServerClientsHelperDuringInitializationIfAllRelevantBackendsAreDoneUsingIt();
}
/*
void RpcBankServer::simulatePendingBalanceDetectedBroadcast()
{
    QMetaObject::invokeMethod(m_BitcoinThreadHelper, "simulatePendingBalanceDetectedBroadcast", Qt::QueuedConnection);
}
void RpcBankServer::simulateConfirmedBalanceDetectedBroadcast()
{
    QMetaObject::invokeMethod(m_BitcoinThreadHelper, "simulateConfirmedBalanceDetectedBroadcast", Qt::QueuedConnection);
}
*/
