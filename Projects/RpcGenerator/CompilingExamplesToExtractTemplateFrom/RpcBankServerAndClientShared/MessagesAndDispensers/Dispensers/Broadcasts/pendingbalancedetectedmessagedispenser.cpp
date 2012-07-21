#include "pendingbalancedetectedmessagedispenser.h"

PendingBalanceDetectedMessageDispenser::PendingBalanceDetectedMessageDispenser(QObject *destination, QObject *owner)
    : IMessageDispenser(destination, owner)
{ }
PendingBalanceDetectedMessage *PendingBalanceDetectedMessageDispenser::getNewOrRecycled()
{
    return static_cast<PendingBalanceDetectedMessage*>(privateGetNewOrRecycled());
}
IMessage *PendingBalanceDetectedMessageDispenser::getNewOfTypeAndConnectToDestinationObject()
{
    PendingBalanceDetectedMessage *pendingBalanceDetectedMessage = new PendingBalanceDetectedMessage(this);
    connect(pendingBalanceDetectedMessage, SIGNAL(deliverSignal()), static_cast<IAcceptRpcBankServerMessageDeliveries*>(m_Destination), SLOT(pendingBalanceDetectedDelivery()));
    return pendingBalanceDetectedMessage;
}
