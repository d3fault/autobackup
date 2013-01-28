#include "serverconfirmedbalancedetectedmessagedispenser.h"

ServerConfirmedBalanceDetectedMessageDispenser::ServerConfirmedBalanceDetectedMessageDispenser(IAcceptRpcBankServerBroadcastDeliveries_AND_IEmitActionsForSignalRelayHack *destination, QObject *owner)
    : IRecycleableDispenser(destination, owner)
{ }
ServerConfirmedBalanceDetectedMessage *ServerConfirmedBalanceDetectedMessageDispenser::getNewOrRecycled()
{
    return static_cast<ServerConfirmedBalanceDetectedMessage*>(privateGetNewOrRecycled());
}
IRecycleableAndStreamable *ServerConfirmedBalanceDetectedMessageDispenser::newOfTypeAndConnectToDestinationObjectIfApplicable()
{
    ServerConfirmedBalanceDetectedMessage *confirmedBalanceDetectedMessage = new ServerConfirmedBalanceDetectedMessage(this);
    connect(confirmedBalanceDetectedMessage, SIGNAL(deliverSignal()), static_cast<IAcceptRpcBankServerBroadcastDeliveries_AND_IEmitActionsForSignalRelayHack*>(m_Destination), SLOT(confirmedBalanceDetectedDelivery()));
    return confirmedBalanceDetectedMessage;
}
