#include "bankserver.h"

BankServer::BankServer()
{
    m_BankDbHelper = new BankDbHelper();
    m_Bitcoin = new BitcoinHelper();

    connectRpcActionRequestSignalsToMySlotImpls();
    daisyChainInitStartStopConnections();
}
void BankServer::connectRpcActionRequestSignalsToMySlotImpls()
{
    //TODOoptimization: there's a slightly better design where i can do 'connect(rpc, signal(create()), m_BankDbHelper, SLOT(create())); ---- but it requires a radical redesign. right now our backend impl doesn't even need to be aware of clientshelper... but in that case it would (maybe? maybe i can just make available the interface that exposes the action request signals (i think IBankServerProtocolKnower implements that interface atm))
    //i think the above is possible but i just need to factor out the signals that IBankServerProtocolKnower defines. the action request signals... then pass them to BankServer... clientshelper can call a pure virtual or something with that new interface as the parameter... then we just connect the objects and that's all... ez gg. not worth doing yet however because i'm overloaded with trivial tasks (welcome to programming)

    connect(this, SIGNAL(createBankAccountRequested(CreateBankAccountMessage*)), m_BankDbHelper, SLOT(doCreateBankAccount(CreateBankAccountMessage*)));

    //TODOreq: create bank account definitely needs to go to the db, but where should GetAddFundsKey go?
    //even if it goes to both db and bitcoin, we should set up signal/slot chaining to achieve it. the two backend objects should remain unaware of each other
}
void BankServer::daisyChainInitStartStopConnections()
{
    //daisy-chain
    //init
    connect(m_BankDbHelper, SIGNAL(initialized()), m_Bitcoin, SLOT(init()));
    connect(m_Bitcoin, SIGNAL(initialized()), this, SIGNAL(initialized()));

    //start
    connect(m_BankDbHelper, SIGNAL(started()), m_Bitcoin, SLOT(start()));
    connect(m_Bitcoin, SIGNAL(started()), this, SIGNAL(started()));

    //stop
    connect(m_Bitcoin, SIGNAL(stopped()), m_BankDbHelper, SLOT(stop()));
    connect(m_BankDbHelper, SIGNAL(stopped()), this, SIGNAL(stopped()));
}
void BankServer::instructBackendObjectsToClaimRelevantDispensers()
{
    //db doesn't own any atm
    m_Bitcoin->takeOwnershipOfApplicableBroadcastDispensers(m_BroadcastDispensers);
}
void BankServer::moveBackendBusinessObjectsToTheirOwnThreadsAndStartTheThreads()
{
    m_BankDbHelperThread = new QThread();
    m_BankDbHelper->moveToThread(m_BankDbHelperThread);
    m_BankDbHelperThread->start();

    m_BitcoinThread = new QThread();
    m_Bitcoin->moveToThread(m_BitcoinThread);
    m_BitcoinThread->start();
}
void BankServer::init()
{
    QMetaObject::invokeMethod(m_BankDbHelper, "init", Qt::QueuedConnection);
}
void BankServer::start()
{
    QMetaObject::invokeMethod(m_BankDbHelper, "start", Qt::QueuedConnection);
}
void BankServer::stop()
{
    QMetaObject::invokeMethod(m_Bitcoin, "stop", Qt::QueuedConnection);
}
void BankServer::createBankAccount(CreateBankAccountMessage *createBankAccountMessage)
{
    emit d(QString("receiving create bank account message with username: ") + createBankAccountMessage->Username);
    //createBankAccountMessage->deliver(); //deliver implies "Success", for every Action
    emit createBankAccountRequested(createBankAccountMessage);
}
void BankServer::getAddFundsKey(GetAddFundsKeyMessage *getAddFundsKeyMessage)
{
    emit d(QString("receiving get add funds key message with username: ") + getAddFundsKeyMessage->Username);
    /*getAddFundsKeyMessage->AddFundsKey = "randomAddFundsKeyGoesHere";
    getAddFundsKeyMessage->deliver();*/
    //TODO: figure out where GetAddFundsKey should go first: bitcoin or db
}
