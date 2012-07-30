#include "servergetaddfundskeymessagedispenser.h"

ServerGetAddFundsKeyMessageDispenser::ServerGetAddFundsKeyMessageDispenser(IAcceptRpcBankServerActionAndActionErrorAndBroadcastMessageDeliveries *destination, QObject *owner)
    : IRecycleableDispenser(destination, owner)
{ }
ServerGetAddFundsKeyMessage *ServerGetAddFundsKeyMessageDispenser::getNewOrRecycled()
{
    return static_cast<ServerGetAddFundsKeyMessage*>(privateGetNewOrRecycled());
}
IRecycleableAndStreamable *ServerGetAddFundsKeyMessageDispenser::newOfTypeAndConnectToDestinationObjectIfApplicable()
{
    ServerGetAddFundsKeyMessage *getAddFundsKeyMessage = new ServerGetAddFundsKeyMessage(this);

    //delivery
    connect(getAddFundsKeyMessage, SIGNAL(deliverSignal()), static_cast<IAcceptRpcBankServerActionNonErrorDeliveries*>(m_Destination), SLOT(getAddFundsKeyDelivery()));

    //errors
    connect(getAddFundsKeyMessage, SIGNAL(getAddFundsKeyFailedUsernameDoesntExistSignal()), static_cast<IAcceptRpcBankServerActionAndActionErrorAndBroadcastMessageDeliveries*>(m_Destination), SLOT(getAddFundsKeyFailedUsernameDoesntExist()));
    connect(getAddFundsKeyMessage, SIGNAL(getAddFundsKeyFailedUseExistingKeyFirstSignal()), static_cast<IAcceptRpcBankServerActionAndActionErrorAndBroadcastMessageDeliveries*>(m_Destination), SLOT(getAddFundsKeyFailedUseExistingKeyFirst()));
    connect(getAddFundsKeyMessage, SIGNAL(getAddFundsKeyFailedWaitForPendingToBeConfirmedSignal()), static_cast<IAcceptRpcBankServerActionAndActionErrorAndBroadcastMessageDeliveries*>(m_Destination), SLOT(getAddFundsKeyFailedWaitForPendingToBeConfirmed()));

    return getAddFundsKeyMessage;
}
