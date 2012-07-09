#include "irpcbankserverclientshelper.h"

#include "irpcbankserver.h"

#include "messages/actions/createbankaccountmessage.h"
#include "messageDispensers/broadcasts/pendingbalanceaddedmessagedispenser.h"

IRpcBankServerClientsHelper::IRpcBankServerClientsHelper(IRpcBankServer *business)
    : m_Business(business)
{
    m_BroadcastDispensers = new BroadcastDispensers();

    //TODOreq: connect etc to IRpcBusiness signals/slots (actions/broadcasts)(i think? pretty sure yes.)
    //connect(m_Transporter, SIGNAL(createBankAccount(CreateBankAccountMessage*)), m_Business, SLOT(createBankAccount(CreateBankAccountMessage*)));


    //next line must be before the .moveToThreads below -- it sets up broadcast dispensers properly
    m_Business->takeOwnershipOfAllBroadcastDispensers(this); //m_Transport has to be new'd before we can call this anyways, as we use it as a destination object. so this move to setupConnections() seems worthwhile

    if(!m_BroadcastDispensers->everyDispenserIsCreated())
        return; //TODOreq: return false. even though shit, it's called from a constructor. maybe just set up an initFailed flag and throw an error?

    m_Business->moveBackendBusinessObjectsToTheirOwnThreadsAndStartThem();

    m_Transporter = Transporter::getNewTransporterImpl();

    connectTransporterActionRequestSignalsToBusinessSlots();

    //TODOreq: also, we might need to set up actions dispensers (make m_Transport the parent i think?) BEFORE moving threads etc
    m_Transporter->takeOwnershipOfAllActionDispensers();

    //^^^TODOreq: in the rpc client, the two above calls will be opposite. m_Business->takeOwnershipOfActionDispensersAndOrganizeAndStartPrivateBusinessObjects(); (rpc CLIENT business in that case)
    //and: m_Transport->claimAllBroadcastDispensers(); //transport being rpc server helper (or possibly irpcbusinesscontroller? fml i should have kept the "clientshelper" and "serverhelper" names...

    m_BusinessThread = new QThread();
    m_Business->moveToThread(m_BusinessThread);
    m_BusinessThread->start();

    m_RpcBankServerMessageTransporterThread = new QThread();
    m_Transporter->moveToThread(m_RpcBankServerMessageTransporterThread);
    m_RpcBankServerMessageTransporterThread->start();
}
PendingBalanceAddedMessageDispenser *IRpcBankServerClientsHelper::takeOwnershipOfPendingBalanceAddedMessageDispenserRiggedForDelivery(QObject *owner)
{
    //we set the object passed in (an rpc server impl backend object) as the dispenser's owner, so that it is moved on .moveToThread and so we can pass 'this' (dispenser) in when new'ing our specific message type... and we will be calling it from the owner object's thread (same one .moveTo'd) also
    m_BroadcastDispensers->setPendingBalanceAddedMessageDispenser(new PendingBalanceAddedMessageDispenser(owner));
    //we set up the destination object so they can just do message.deliver(); and it emits itself
    m_BroadcastDispensers->pendingBalanceAddedMessageDispenser()->setDestinationObject(m_Transporter);
    //we return a reference to it to the object that took ownership. we keep a copy in m_BroadcastDispensers just so we can make sure they all got instantiated. no other reason i can think of yet. it would be unsafe for us to touch them when we are up and running. only the owners can
    return m_BroadcastDispensers->pendingBalanceAddedMessageDispenser();
}
void IRpcBankServerClientsHelper::connectTransporterActionRequestSignalsToBusinessSlots()
{
    //action requests
    connect(m_Transporter, SIGNAL(createBankAccount(CreateBankAccountMessage*)), m_Business, SLOT(createBankAccount(CreateBankAccountMessage*)));

    //actions responses are handled via .deliver();
    //broadcasts are handled via .deliver(); also
}
