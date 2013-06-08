#include "ClientCreateBankAccountMessagedispenser.h"

ClientCreateBankAccountMessageDispenser::ClientCreateBankAccountMessageDispenser(IAcceptRpcBankServerActionDeliveries *destination, QObject *owner)
    : IRecycleableDispenser(destination, owner)
{ }
ClientCreateBankAccountMessage *ClientCreateBankAccountMessageDispenser::getNewOrRecycled()
{
    return static_cast<ClientCreateBankAccountMessage*>(privateGetNewOrRecycled());
}
IRecycleableAndStreamable *ClientCreateBankAccountMessageDispenser::newOfTypeAndConnectToDestinationObjectIfApplicable()
{
    ClientCreateBankAccountMessage *ClientCreateBankAccountMessage = new ClientCreateBankAccountMessage(this);

    connect(ClientCreateBankAccountMessage, SIGNAL(deliverSignal()), static_cast<IAcceptRpcBankServerActionDeliveries*>(m_Destination), SLOT(createBankAccountDelivery()));

    return ClientCreateBankAccountMessage;
}
