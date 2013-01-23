#include "serverpendingbalancedetectedmessagedispenser.h"

ServerPendingBalanceDetectedMessageDispenser::ServerPendingBalanceDetectedMessageDispenser(IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptActionDeliveries *destination, QObject *owner)
    : IRecycleableDispenser(destination, owner)
{ }
ServerPendingBalanceDetectedMessage *ServerPendingBalanceDetectedMessageDispenser::getNewOrRecycled()
{
    return static_cast<ServerPendingBalanceDetectedMessage*>(privateGetNewOrRecycled());
}
IRecycleableAndStreamable *ServerPendingBalanceDetectedMessageDispenser::newOfTypeAndConnectToDestinationObjectIfApplicable()
{
    ServerPendingBalanceDetectedMessage *pendingBalanceDetectedMessage = new ServerPendingBalanceDetectedMessage(this);
    connect(pendingBalanceDetectedMessage, SIGNAL(deliverSignal()), static_cast<IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptActionDeliveries*>(m_Destination), SLOT(pendingBalanceDetectedDelivery()));
    return pendingBalanceDetectedMessage;
}
