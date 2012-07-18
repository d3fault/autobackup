#include "confirmedbalancedetectedmessagedispenser.h"

ConfirmedBalanceDetectedMessageDispenser::ConfirmedBalanceDetectedMessageDispenser(IAcceptRpcBankServerMessageDeliveries *destination, QObject *owner)
    : IMessageDispenser(destination, owner)
{
}
ConfirmedBalanceDetectedMessage *ConfirmedBalanceDetectedMessageDispenser::getNewOrRecycled()
{
    return static_cast<ConfirmedBalanceDetectedMessage*>(privateGetNewOrRecycled());
}
void ConfirmedBalanceDetectedMessageDispenser::getNewOfTypeAndConnectToDestinationObject()
{
    ConfirmedBalanceDetectedMessage *confirmedBalanceDetectedMessage = new ConfirmedBalanceDetectedMessage(this);
    connect(confirmedBalanceDetectedMessage, SIGNAL(deliverSignal()), static_cast<IAcceptMessageDeliveriesGoingToRpcBankClient*>(m_Destination), SLOT(confirmedBalanceDetected(ConfirmedBalanceDetectedMessage*)));
    return confirmedBalanceDetectedMessage;
}
