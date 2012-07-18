#include "getaddfundskeymessagedispenser.h"

GetAddFundsKeyMessageDispenser::GetAddFundsKeyMessageDispenser(IAcceptRpcBankServerMessageDeliveries *destination, QObject *owner)
    : IMessageDispenser(destination, owner)
{ }
GetAddFundsKeyMessage *GetAddFundsKeyMessageDispenser::getNewOrRecycled()
{
    return static_cast<GetAddFundsKeyMessage*>(privateGetNewOrRecycled());
}
void GetAddFundsKeyMessageDispenser::getNewOfTypeAndConnectToDestinationObject()
{
    GetAddFundsKeyMessage *getAddFundsKeyMessage = new GetAddFundsKeyMessage(this);
    connect(getAddFundsKeyMessage, SIGNAL(deliverSignal()), static_cast<IAcceptMessageDeliveriesGoingToRpcBankServer*>(m_Destination), SLOT(getAddFundsKey(GetAddFundsKeyMessage*)));
    return getAddFundsKeyMessage;
}
