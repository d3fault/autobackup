#include "irpcclientshelper.h"

#include "irpcbusiness.h"

#include "messageDispensers/broadcasts/pendingbalanceaddedmessagedispenser.h"

IRpcClientsHelper::IRpcClientsHelper(IRpcBusiness *business)
    : m_Business(business)
{
    //connect etc to IRpcBusiness signals/slots (actions/broadcasts)(i think?)
}
void IRpcClientsHelper::setBroadcastDispenserParentForPendingBalanceAddedMessages(QObject *owner)
{
    m_Business->broadcastDispensers()->setPendingBalanceAddedMessageDispenser(new PendingBalanceAddedMessageDispenser(owner));
    m_Business->broadcastDispensers()->pendingBalanceAddedMessageDispenser()->setDestinationObject(this);
}
