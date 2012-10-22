#include "serverconfirmedbalancedetectedmessagedispenser.h"

ServerConfirmedBalanceDetectedMessageDispenser::ServerConfirmedBalanceDetectedMessageDispenser(IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptAllDeliveries *destination, QObject *owner)
    : IRecycleableDispenser(destination, owner)
{ }
ServerConfirmedBalanceDetectedMessage *ServerConfirmedBalanceDetectedMessageDispenser::getNewOrRecycled()
{
    return static_cast<ServerConfirmedBalanceDetectedMessage*>(privateGetNewOrRecycled());
}
IRecycleableAndStreamable *ServerConfirmedBalanceDetectedMessageDispenser::newOfTypeAndConnectToDestinationObjectIfApplicable()
{
    ServerConfirmedBalanceDetectedMessage *confirmedBalanceDetectedMessage = new ServerConfirmedBalanceDetectedMessage(this);
    connect(confirmedBalanceDetectedMessage, SIGNAL(deliverSignal()), static_cast<IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptAllDeliveries*>(m_Destination), SLOT(confirmedBalanceDetectedDelivery()));
    return confirmedBalanceDetectedMessage;
}
