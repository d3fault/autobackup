#include "servercreatebankaccountmessagedispenser.h"

ServerCreateBankAccountMessageDispenser::ServerCreateBankAccountMessageDispenser(IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptActionDeliveries *destination, QObject *owner)
    : IRecycleableDispenser(destination, owner) //TODOoptimization: could pass in destination twice here and keep compatibility with broadcasts. our constructor only needs the destination (which _IS_ a qobject/owner/parent)
{ }
CreateBankAccountMessage *ServerCreateBankAccountMessageDispenser::getNewOrRecycled()
{
    CreateBankAccountMessage *createBankAccountMessage = static_cast<CreateBankAccountMessage*>(privateGetNewOrRecycled());

    //Rpc Generator has implicit knowledge that the message is an ActionMessage and then calls resetSuccessAndErrorCode() on them. It does not call it for broadcasts
    createBankAccountMessage->resetSuccessAndErrorCode();

    return createBankAccountMessage;
}
IRecycleableAndStreamable *ServerCreateBankAccountMessageDispenser::newOfTypeAndConnectToDestinationObjectIfApplicable()
{
    CreateBankAccountMessage *createBankAccountMessage = new CreateBankAccountMessage(this);

    connect(createBankAccountMessage, SIGNAL(deliverSignal()), static_cast<IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptActionDeliveries*>(m_Destination), SLOT(createBankAccountDelivery()));

    return createBankAccountMessage;
}
