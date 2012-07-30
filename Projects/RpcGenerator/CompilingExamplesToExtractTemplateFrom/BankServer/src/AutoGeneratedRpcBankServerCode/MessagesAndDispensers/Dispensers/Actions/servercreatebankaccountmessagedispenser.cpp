#include "servercreatebankaccountmessagedispenser.h"

ServerCreateBankAccountMessageDispenser::ServerCreateBankAccountMessageDispenser(IAcceptRpcBankServerActionAndActionErrorAndBroadcastMessageDeliveries *destination, QObject *owner)
    : IRecycleableDispenser(destination, owner)
{ }
ServerCreateBankAccountMessage *ServerCreateBankAccountMessageDispenser::getNewOrRecycled()
{
    return static_cast<ServerCreateBankAccountMessage*>(privateGetNewOrRecycled());
}
IRecycleableAndStreamable *ServerCreateBankAccountMessageDispenser::newOfTypeAndConnectToDestinationObjectIfApplicable()
{
    ServerCreateBankAccountMessage *createBankAccountMessage = new ServerCreateBankAccountMessage(this);

    //delivery
    connect(createBankAccountMessage, SIGNAL(deliverSignal()), static_cast<IAcceptRpcBankServerActionNonErrorDeliveries*>(m_Destination), SLOT(createBankAccountDelivery()));

    //errors
    connect(createBankAccountMessage, SIGNAL(createBankAccountFailedUsernameAlreadyExistsSignal()), static_cast<IAcceptRpcBankServerActionAndActionErrorAndBroadcastMessageDeliveries*>(m_Destination), SLOT(createBankAccountFailedUsernameAlreadyExists()));
    connect(createBankAccountMessage, SIGNAL(createBankAccountFailedPersistErrorSignal()), static_cast<IAcceptRpcBankServerActionAndActionErrorAndBroadcastMessageDeliveries*>(m_Destination), SLOT(createBankAccountFailedPersistError()));

    return createBankAccountMessage;
}
