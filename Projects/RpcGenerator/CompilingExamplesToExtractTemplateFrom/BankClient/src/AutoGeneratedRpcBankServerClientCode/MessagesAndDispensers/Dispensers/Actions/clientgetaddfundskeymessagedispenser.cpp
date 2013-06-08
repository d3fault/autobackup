#include "clientgetaddfundskeymessagedispenser.h"

ClientGetAddFundsKeyMessageDispenser::ClientGetAddFundsKeyMessageDispenser(IAcceptRpcBankServerActionDeliveries *destination, QObject *owner)
    : IRecycleableDispenser(destination, owner)
{ }
GetAddFundsKeyMessage *ClientGetAddFundsKeyMessageDispenser::getNewOrRecycled()
{
    return static_cast<GetAddFundsKeyMessage*>(privateGetNewOrRecycled());
    /*GetAddFundsKeyMessage *getAddFundsKeyMessage = static_cast<GetAddFundsKeyMessage*>(privateGetNewOrRecycled());
    getAddFundsKeyMessage->Header.MessageId =
    return getAddFundsKeyMessage;*/
}
IRecycleableAndStreamable *ClientGetAddFundsKeyMessageDispenser::newOfTypeAndConnectToDestinationObjectIfApplicable()
{
    GetAddFundsKeyMessage *clientGetAddFundsKeyMessage = new GetAddFundsKeyMessage(this);

    connect(clientGetAddFundsKeyMessage, SIGNAL(deliverSignal()), static_cast<IAcceptRpcBankServerActionDeliveries*>(m_Destination), SLOT(getAddFundsKeyDelivery()));

    return clientGetAddFundsKeyMessage;
}
