#include "irpcbusinesscontroller.h"

#include "irpcbusiness.h"

#include "messageDispensers/broadcasts/pendingbalanceaddedmessagedispenser.h"

IRpcBusinessController::IRpcBusinessController(IRpcBusiness *business)
    : m_Business(business)
{ }
void IRpcBusinessController::setupConnections()
{
    //TODOreq: connect etc to IRpcBusiness signals/slots (actions/broadcasts)(i think? pretty sure yes.)


    //next line must be before the .moveToThreads below -- it sets up broadcast dispensers properly
    m_Business->organizeBackendBusinessObjectsThreadsAndStartThem(this); //m_Transport has to be new'd before we can call this anyways, as we use it as a destination object. so this move to setupConnections() seems worthwhile

    //TODOreq: also, we might need to set up actions dispensers (make m_Transport the parent i think?) BEFORE moving threads etc
    m_Transport->claimAllActionDispensers();

    //^^^TODOreq: in the rpc client, the two above calls will be opposite. m_Business->takeOwnershipOfActionDispensersAndOrganizeAndStartPrivateBusinessObjects(); (rpc CLIENT business in that case)
    //and: m_Transport->claimAllBroadcastDispensers(); //transport being rpc server helper (or possibly irpcbusinesscontroller? fml i should have kept the "clientshelper" and "serverhelper" names...

    m_BusinessThread = new QThread();
    m_Business->moveToThread(m_BusinessThread);
    m_BusinessThread->start();

    m_RpcTransportThread = new QThread();
    m_Transport->moveToThread(m_RpcTransportThread);
    m_RpcTransportThread->start();
}
void IRpcBusinessController::setBroadcastDispenserParentForPendingBalanceAddedMessages(QObject *owner)
{
    m_Business->broadcastDispensers()->setPendingBalanceAddedMessageDispenser(new PendingBalanceAddedMessageDispenser(owner));
    m_Business->broadcastDispensers()->pendingBalanceAddedMessageDispenser()->setDestinationObject(m_Transport);
}
