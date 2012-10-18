#include "clientcreatebankaccountmessagedispenser.h"

ClientCreateBankAccountMessageDispenser::ClientCreateBankAccountMessageDispenser(IAcceptRpcBankServerActionDeliveries *destination, QObject *owner)
    : IRecycleableDispenser(destination, owner)
{ }
ClientCreateBankAccountMessage *ClientCreateBankAccountMessageDispenser::getNewOrRecycled()
{
    return static_cast<ClientCreateBankAccountMessage*>(privateGetNewOrRecycled());
}
IRecycleableAndStreamable *ClientCreateBankAccountMessageDispenser::newOfTypeAndConnectToDestinationObjectIfApplicable()
{
    ClientCreateBankAccountMessage *clientCreateBankAccountMessage = new ClientCreateBankAccountMessage(this);

    connect(clientCreateBankAccountMessage, SIGNAL(deliverSignal()), static_cast<IAcceptRpcBankServerActionDeliveries*>(m_Destination), SLOT(createBankAccountDelivery()));

    return clientCreateBankAccountMessage;
}
