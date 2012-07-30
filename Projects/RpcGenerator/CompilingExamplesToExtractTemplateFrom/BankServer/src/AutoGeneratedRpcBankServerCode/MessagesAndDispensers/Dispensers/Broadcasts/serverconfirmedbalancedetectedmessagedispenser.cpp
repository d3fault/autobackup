#include "serverconfirmedbalancedetectedmessagedispenser.h"

ServerConfirmedBalanceDetectedMessageDispenser::ServerConfirmedBalanceDetectedMessageDispenser(IAcceptRpcBankServerActionAndActionErrorAndBroadcastMessageDeliveries *destination, QObject *owner)
    : IRecycleableDispenser(destination, owner)
{ }
ServerConfirmedBalanceDetectedMessage *ServerConfirmedBalanceDetectedMessageDispenser::getNewOrRecycled()
{
    return static_cast<ServerConfirmedBalanceDetectedMessage*>(privateGetNewOrRecycled());
}
IRecycleableAndStreamable *ServerConfirmedBalanceDetectedMessageDispenser::newOfTypeAndConnectToDestinationObjectIfApplicable()
{
    ServerConfirmedBalanceDetectedMessage *confirmedBalanceDetectedMessage = new ServerConfirmedBalanceDetectedMessage(this);
    connect(confirmedBalanceDetectedMessage, SIGNAL(deliverSignal()), static_cast<IAcceptRpcBankServerActionAndActionErrorAndBroadcastMessageDeliveries*>(m_Destination), SLOT(confirmedBalanceDetectedDelivery()));
    return confirmedBalanceDetectedMessage;
}
