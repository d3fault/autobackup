#include "createbankaccountmessagedispenser.h"

CreateBankAccountMessageDispenser::CreateBankAccountMessageDispenser(QObject *destination, QObject *owner)
    : IMessageDispenser(destination, owner)
{ }
CreateBankAccountMessage *CreateBankAccountMessageDispenser::getNewOrRecycled()
{
    return static_cast<CreateBankAccountMessage*>(privateGetNewOrRecycled());
}
IMessage *CreateBankAccountMessageDispenser::getNewOfTypeAndConnectToDestinationObject()
{
    CreateBankAccountMessage *createBankAccountMessage = new CreateBankAccountMessage(this);
    connect(createBankAccountMessage, SIGNAL(deliverSignal()), static_cast<IAcceptRpcBankServerMessageDeliveries*>(m_Destination), SLOT(createBankAccountDelivery()));
    //connect(createBankAccountMessage, SIGNAL(createBankAccountFailedUsernameAlreadyExistsSignal()), static_cast<IAcceptRpcBankServerMessageDeliveries*>(m_Destination), SLOT(createBankAccountFailedReasonXYZ()));
    //TODOreq: rig errors
    return createBankAccountMessage;
}
