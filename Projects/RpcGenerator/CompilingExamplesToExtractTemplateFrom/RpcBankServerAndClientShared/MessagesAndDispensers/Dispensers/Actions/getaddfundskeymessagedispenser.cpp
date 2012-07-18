#include "getaddfundskeymessagedispenser.h"

#include "../../../iacceptrpcbankserveractiondeliveries.h"

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
    connect(getAddFundsKeyMessage, SIGNAL(deliverSignal()), static_cast<IAcceptRpcBankServerActionDeliveries*>(m_Destination), SLOT(getAddFundsKeyDelivery()));
    return getAddFundsKeyMessage;
}
