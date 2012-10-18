#include "clientgetaddfundskeymessagedispenser.h"

ClientGetAddFundsKeyMessageDispenser::ClientGetAddFundsKeyMessageDispenser(IAcceptRpcBankServerActionDeliveries *destination, QObject *owner)
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

    connect(clientGetAddFundsKeyMessage, SIGNAL(deliverSignal()), static_cast<IAcceptRpcBankServerActionDeliveries*>(m_Destination), SLOT(getAddFundsKeyDelivery()));

    return clientGetAddFundsKeyMessage;
}
