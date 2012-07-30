#include "clientgetaddfundskeymessagedispenser.h"

ClientGetAddFundsKeyMessageDispenser::ClientGetAddFundsKeyMessageDispenser(IAcceptRpcBankServerActionNonErrorDeliveries *destination, QObject *owner)
    : IRecycleableDispenser(destination, owner)
{ }
ClientGetAddFundsKeyMessage *ClientGetAddFundsKeyMessageDispenser::getNewOrRecycled()
{
    return static_cast<ClientGetAddFundsKeyMessage*>(privateGetNewOrRecycled());
}
IRecycleableAndStreamable *ClientGetAddFundsKeyMessageDispenser::newOfTypeAndConnectToDestinationObjectIfApplicable()
{
    ClientGetAddFundsKeyMessage *clientGetAddFundsKeyMessage = new ClientGetAddFundsKeyMessage(this);

    connect(clientGetAddFundsKeyMessage, SIGNAL(deliverSignal()), static_cast<IAcceptRpcBankServerActionNonErrorDeliveries*>(m_Destination), SLOT(getAddFundsKeyDelivery()));

    return clientGetAddFundsKeyMessage;
}
