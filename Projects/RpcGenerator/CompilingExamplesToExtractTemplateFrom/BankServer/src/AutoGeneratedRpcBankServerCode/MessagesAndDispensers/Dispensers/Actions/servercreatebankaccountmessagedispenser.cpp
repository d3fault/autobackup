#include "servercreatebankaccountmessagedispenser.h"

ServerCreateBankAccountMessageDispenser::ServerCreateBankAccountMessageDispenser(IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptAllDeliveries *destination, QObject *owner)
    : IRecycleableDispenser(destination, owner)
{ }
CreateBankAccountMessage *ServerCreateBankAccountMessageDispenser::getNewOrRecycled()
{
    return static_cast<CreateBankAccountMessage*>(privateGetNewOrRecycled());
}
IRecycleableAndStreamable *ServerCreateBankAccountMessageDispenser::newOfTypeAndConnectToDestinationObjectIfApplicable()
{
    CreateBankAccountMessage *createBankAccountMessage = new CreateBankAccountMessage(this);

    connect(createBankAccountMessage, SIGNAL(deliverSignal()), static_cast<IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptAllDeliveries*>(m_Destination), SLOT(createBankAccountDelivery()));

    return createBankAccountMessage;
}
