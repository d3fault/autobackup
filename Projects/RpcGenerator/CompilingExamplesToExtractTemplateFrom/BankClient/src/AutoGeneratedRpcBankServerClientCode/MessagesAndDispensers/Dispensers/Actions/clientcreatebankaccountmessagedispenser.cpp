#include "clientcreatebankaccountmessagedispenser.h"

ClientCreateBankAccountMessageDispenser::ClientCreateBankAccountMessageDispenser(IAcceptRpcBankServerActionNonErrorDeliveries *destination, QObject *owner)
    : IRecycleableDispenser(destination, owner)
{ }
ClientCreateBankAccountMessage *ClientCreateBankAccountMessageDispenser::getNewOrRecycled()
{
    return static_cast<ClientCreateBankAccountMessage*>(privateGetNewOrRecycled());
}
IRecycleableAndStreamable *ClientCreateBankAccountMessageDispenser::newOfTypeAndConnectToDestinationObjectIfApplicable()
{
    ClientCreateBankAccountMessage *clientCreateBankAccountMessage = new ClientCreateBankAccountMessage(this);

    connect(clientCreateBankAccountMessage, SIGNAL(deliverSignal()), static_cast<IAcceptRpcBankServerActionNonErrorDeliveries*>(m_Destination), SLOT(createBankAccountDelivery()));

    return clientCreateBankAccountMessage;
}
