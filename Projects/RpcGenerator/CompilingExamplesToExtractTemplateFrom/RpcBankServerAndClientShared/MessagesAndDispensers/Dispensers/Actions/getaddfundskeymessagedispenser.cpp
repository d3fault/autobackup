#include "getaddfundskeymessagedispenser.h"

GetAddFundsKeyMessageDispenser::GetAddFundsKeyMessageDispenser(QObject *destination, QObject *owner)
    : IMessageDispenser(destination, owner)
{ }
GetAddFundsKeyMessage *GetAddFundsKeyMessageDispenser::getNewOrRecycled()
{
    return static_cast<GetAddFundsKeyMessage*>(privateGetNewOrRecycled());
}
IMessage *GetAddFundsKeyMessageDispenser::getNewOfTypeAndConnectToDestinationObject()
{
    GetAddFundsKeyMessage *getAddFundsKeyMessage = new GetAddFundsKeyMessage(this);
    connect(getAddFundsKeyMessage, SIGNAL(deliverSignal()), static_cast<IAcceptRpcBankServerActionNonErrorDeliveries*>(m_Destination), SLOT(getAddFundsKeyDelivery()));
    return getAddFundsKeyMessage;
}
