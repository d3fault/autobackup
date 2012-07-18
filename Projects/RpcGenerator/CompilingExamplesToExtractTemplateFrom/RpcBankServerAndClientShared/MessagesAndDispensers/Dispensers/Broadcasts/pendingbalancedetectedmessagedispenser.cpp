#include "pendingbalancedetectedmessagedispenser.h"

PendingBalanceDetectedMessageDispenser::PendingBalanceDetectedMessageDispenser(IAcceptRpcBankServerMessageDeliveries *destination, QObject *owner)
    : IMessageDispenser(destination, owner)
{ }
PendingBalanceAddedMessage *PendingBalanceDetectedMessageDispenser::getNewOrRecycled()
{
    return static_cast<PendingBalanceAddedMessage*>(privateGetNewOrRecycled());
}
void PendingBalanceDetectedMessageDispenser::getNewOfTypeAndConnectToDestinationObject()
{
    PendingBalanceDetectedMessage *pendingBalanceDetectedMessage = new PendingBalanceDetectedMessage(this);
    connect(pendingBalanceDetectedMessage, SIGNAL(deliverSignal()), static_cast<IAcceptMessageDeliveriesGoingToRpcBankClient*>(m_Destination), SLOT(pendingBalanceDetected(PendingBalanceDetectedMessage*)));
    return pendingBalanceDetectedMessage;
}
