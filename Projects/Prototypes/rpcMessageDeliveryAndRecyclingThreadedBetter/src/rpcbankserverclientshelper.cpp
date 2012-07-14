#include "rpcbankserverclientshelper.h"

#include "irpcbankserver.h"
#include "messages/actions/createbankaccountmessage.h"
#include "messageDispensers/broadcasts/pendingbalanceaddedmessagedispenser.h"
#include "ssltcpserverandprotocolknower.h"

RpcBankServerClientsHelper::RpcBankServerClientsHelper(IRpcBankServer *business)
    : m_Business(business)
{
    m_ActionDispensers = new ActionDispensers();
    m_BroadcastDispensers = new BroadcastDispensers();

    //TODOreq: connect etc to IRpcBusiness signals/slots (actions/broadcasts)(i think? pretty sure yes.)
    //connect(m_Transporter, SIGNAL(createBankAccount(CreateBankAccountMessage*)), m_Business, SLOT(createBankAccount(CreateBankAccountMessage*)));

    //transporter has to be instantiated before business->takeOwnership because inside of it, we set the destination object to be transporter
    m_Transporter = new SslTcpServerAndProtocolKnower();

    //next line must be before the .moveToThreads below -- it sets up broadcast dispensers properly
    m_Business->takeOwnershipOfAllBroadcastDispensers(this); //m_Transport has to be new'd before we can call this anyways, as we use it as a destination object. so this move to setupConnections() seems worthwhile

    if(!m_BroadcastDispensers->everyDispenserIsCreated())
        return; //TODOreq: return false. even though shit, it's called from a constructor. maybe just set up an initFailed flag and throw an error?

    m_Business->moveBackendBusinessObjectsToTheirOwnThreadsAndStartThem();

    connectTransporterActionRequestSignalsToBusinessSlots();

    //TODOreq: also, we might need to set up actions dispensers (make m_Transport the parent i think?) BEFORE moving threads etc
    m_Transporter->takeOwnershipOfAllActionDispensers(this);

    //^^^TODOreq: in the rpc client, the two above calls will be opposite. m_Business->takeOwnershipOfActionDispensersAndOrganizeAndStartPrivateBusinessObjects(); (rpc CLIENT business in that case)
    //and: m_Transport->claimAllBroadcastDispensers(); //transport being rpc server helper (or possibly irpcbusinesscontroller? fml i should have kept the "clientshelper" and "serverhelper" names...

    m_BusinessThread = new QThread();
    m_Business->moveToThread(m_BusinessThread);
    m_BusinessThread->start();

    m_RpcBankServerMessageTransporterThread = new QThread();
    m_Transporter->moveToThread(m_RpcBankServerMessageTransporterThread);
    m_RpcBankServerMessageTransporterThread->start();

    //daisy-chain init
    connect(m_Business, SIGNAL(initialized()), m_Transporter, SLOT(init()));
    connect(m_Business, SIGNAL(started()), m_Transporter, SLOT(start()));
    connect(m_Transporter, SIGNAL(stopped()), m_Business, SLOT(stop())); //we reverse the order when stopping, so that transporter can block and wait for business to complete pending requests (should probably put a timer in place or something... but if the code is good enough we shouldn't need one)

    connect(m_Business, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
    connect(m_Transporter, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
}
void RpcBankServerClientsHelper::init()
{
    QMetaObject::invokeMethod(m_Business, "init", Qt::QueuedConnection);
}
void RpcBankServerClientsHelper::start()
{
    QMetaObject::invokeMethod(m_Business, "start", Qt::QueuedConnection);
}
void RpcBankServerClientsHelper::stop()
{
    QMetaObject::invokeMethod(m_Transporter, "stop", Qt::QueuedConnection);
}
void RpcBankServerClientsHelper::connectTransporterActionRequestSignalsToBusinessSlots()
{
    //action requests
    connect(m_Transporter, SIGNAL(createBankAccount(CreateBankAccountMessage*)), m_Business, SLOT(createBankAccount(CreateBankAccountMessage*)));

    //actions responses are handled via .deliver();
    //broadcasts are handled via .deliver(); also
}
PendingBalanceAddedMessageDispenser *RpcBankServerClientsHelper::takeOwnershipOfPendingBalanceAddedMessageDispenserRiggedForDelivery(QObject *owner)
{
    //we set the object passed in (an rpc server impl backend object) as the dispenser's owner, so that it is moved on .moveToThread and so we can pass 'this' (dispenser) in when new'ing our specific message type... and we will be calling it from the owner object's thread (same one .moveTo'd) also
    m_BroadcastDispensers->setPendingBalanceAddedMessageDispenser(new PendingBalanceAddedMessageDispenser(owner));
    //we set up the destination object so they can just do message.deliver(); and it emits itself
    m_BroadcastDispensers->pendingBalanceAddedMessageDispenser()->setDestinationObject(m_Transporter);
    //we return a reference to it to the object that took ownership. we keep a copy in m_BroadcastDispensers just so we can make sure they all got instantiated. no other reason i can think of yet. it would be unsafe for us to touch them when we are up and running. only the owners can
    return m_BroadcastDispensers->pendingBalanceAddedMessageDispenser();
}
CreateBankAccountMessageDispenser *RpcBankServerClientsHelper::takeOwnershipOfCreateBankAccountMessageDispenserRiggedForDelivery(QObject *owner)
{
    m_ActionDispensers->setCreateBankAccountMessageDispenser(new CreateBankAccountMessageDispenser(owner));
    m_ActionDispensers->createBankAccountMessageDispenser()->setDestinationObject(m_Transporter); //for actions, we could just set the destination object right after the call to this method... but it makes no difference
    return m_ActionDispensers->createBankAccountMessageDispenser();
}
void RpcBankServerClientsHelper::simulateCreateBankAccount()
{
    QMetaObject::invokeMethod(m_Transporter, "simulateCreateBankAccount", Qt::QueuedConnection);
}
