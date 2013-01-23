#include "servergetaddfundskeymessagedispenser.h"

ServerGetAddFundsKeyMessageDispenser::ServerGetAddFundsKeyMessageDispenser(IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptActionDeliveries *destination, QObject *owner)
    : IRecycleableDispenser(destination, owner)
{ }
GetAddFundsKeyMessage *ServerGetAddFundsKeyMessageDispenser::getNewOrRecycled()
{
    return static_cast<GetAddFundsKeyMessage*>(privateGetNewOrRecycled());
}
IRecycleableAndStreamable *ServerGetAddFundsKeyMessageDispenser::newOfTypeAndConnectToDestinationObjectIfApplicable()
{
    GetAddFundsKeyMessage *getAddFundsKeyMessage = new GetAddFundsKeyMessage(this);

    //delivery
    connect(getAddFundsKeyMessage, SIGNAL(deliverSignal()), static_cast<IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptActionDeliveries*>(m_Destination), SLOT(getAddFundsKeyDelivery()));

    return getAddFundsKeyMessage;
}
