#include "createbankaccountmessagedispenser.h"

CreateBankAccountMessageDispenser::CreateBankAccountMessageDispenser(QObject *mandatoryParent)
    : IMessageDispenser(mandatoryParent)
{ }
IMessage *CreateBankAccountMessageDispenser::getNewOfTypeAndConnectToDestinationObject()
{
    CreateBankAccountMessage *createBankAccountMessage = new CreateBankAccountMessage(this);
    //success
    connect(createBankAccountMessage, SIGNAL(deliverSignal()), m_DestinationObject, SLOT(createBankAccountCompleted()));

    //failed
    connect(createBankAccountMessage, SIGNAL(createBankAccountFailedUsernameAlreadyExistsSignal()), m_DestinationObject, SLOT(createBankAccountFailedUsernameAlreadyExists()));
    connect(createBankAccountMessage, SIGNAL(createBankAccountFailedPersistErrorSignal()), m_DestinationObject, SLOT(createBankAccountFailedPersistError()));

    return createBankAccountMessage;
}
CreateBankAccountMessage *CreateBankAccountMessageDispenser::getNewOrRecycled()
{
    return static_cast<CreateBankAccountMessage*>(privateGetNewOrRecycled());
}
