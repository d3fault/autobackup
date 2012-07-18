#include "createbankaccountmessagedispenser.h"

CreateBankAccountMessageDispenser::CreateBankAccountMessageDispenser(IAcceptRpcBankServerMessageDeliveries *destination, QObject *owner)
    : IMessageDispenser(destination, owner)
{ }
CreateBankAccountMessage *CreateBankAccountMessageDispenser::getNewOrRecycled()
{
    return static_cast<CreateBankAccountMessage*>(privateGetNewOrRecycled());
}
void CreateBankAccountMessageDispenser::getNewOfTypeAndConnectToDestinationObject()
{
    CreateBankAccountMessage *createBankAccountMessage = new CreateBankAccountMessage(this);
    connect(createBankAccountMessage, SIGNAL(deliverSignal()), static_cast<IAcceptMessageDeliveriesGoingToRpcBankServer*>(m_Destination), SLOT(createBankAccount(CreateBankAccountMessage*)));
    return createBankAccountMessage;
}
