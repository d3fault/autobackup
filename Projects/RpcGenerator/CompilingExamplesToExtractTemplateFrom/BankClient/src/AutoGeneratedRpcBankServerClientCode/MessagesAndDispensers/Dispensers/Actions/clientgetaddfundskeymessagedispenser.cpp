#include "clientgetaddfundskeymessagedispenser.h"

ClientGetAddFundsKeyMessageDispenser::ClientGetAddFundsKeyMessageDispenser(IAcceptRpcBankServerActionNonErrorDeliveries *destination, QObject *owner)
    : IRecycleableDispenser(destination, owner)
{ }
ClientGetAddFundsKeyMessage *ClientGetAddFundsKeyMessageDispenser::getNewOrRecycled()
{
    return static_cast<ClientGetAddFundsKeyMessage*>(privateGetNewOrRecycled());
    /*ClientGetAddFundsKeyMessage *getAddFundsKeyMessage = static_cast<ClientGetAddFundsKeyMessage*>(privateGetNewOrRecycled());
    getAddFundsKeyMessage->Header.MessageId =
    return getAddFundsKeyMessage;*/
}
IRecycleableAndStreamable *ClientGetAddFundsKeyMessageDispenser::newOfTypeAndConnectToDestinationObjectIfApplicable()
{
    ClientGetAddFundsKeyMessage *clientGetAddFundsKeyMessage = new ClientGetAddFundsKeyMessage(this);

    connect(clientGetAddFundsKeyMessage, SIGNAL(deliverSignal()), static_cast<IAcceptRpcBankServerActionNonErrorDeliveries*>(m_Destination), SLOT(getAddFundsKeyDelivery()));

    return clientGetAddFundsKeyMessage;
}
