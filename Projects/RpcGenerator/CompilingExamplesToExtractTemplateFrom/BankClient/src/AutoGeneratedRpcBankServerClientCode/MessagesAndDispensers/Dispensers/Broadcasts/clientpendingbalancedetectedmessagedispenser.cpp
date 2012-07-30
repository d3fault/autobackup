#include "clientpendingbalancedetectedmessagedispenser.h"

ClientPendingBalanceDetectedMessageDispenser::ClientPendingBalanceDetectedMessageDispenser(QObject *destination, QObject *owner) :
    IRecycleableDispenser(destination, owner)
{ }
ClientPendingBalanceDetectedMessage *ClientPendingBalanceDetectedMessageDispenser::getNewOrRecycled()
{
    return static_cast<ClientPendingBalanceDetectedMessage*>(privateGetNewOrRecycled());
}
IRecycleableAndStreamable *ClientPendingBalanceDetectedMessageDispenser::newOfTypeAndConnectToDestinationObjectIfApplicable()
{
    ClientPendingBalanceDetectedMessage *clientPendingBalanceDetectedMessage = new ClientPendingBalanceDetectedMessage(this);
    return clientPendingBalanceDetectedMessage;
}
