#include "confirmedbalancedetectedmessagedispenser.h"

ConfirmedBalanceDetectedMessageDispenser::ConfirmedBalanceDetectedMessageDispenser(QObject *destination, QObject *owner)
    : IMessageDispenser(destination, owner)
{
}
ConfirmedBalanceDetectedMessage *ConfirmedBalanceDetectedMessageDispenser::getNewOrRecycled()
{
    return static_cast<ConfirmedBalanceDetectedMessage*>(privateGetNewOrRecycled());
}
IMessage *ConfirmedBalanceDetectedMessageDispenser::getNewOfTypeAndConnectToDestinationObject()
{
    ConfirmedBalanceDetectedMessage *confirmedBalanceDetectedMessage = new ConfirmedBalanceDetectedMessage(this);
    connect(confirmedBalanceDetectedMessage, SIGNAL(deliverSignal()), static_cast<IAcceptRpcBankServerMessageDeliveries*>(m_Destination), SLOT(confirmedBalanceDetectedDelivery()));
    return confirmedBalanceDetectedMessage;
}
