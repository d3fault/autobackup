#include "clientconfirmedbalancedetectedmessagedispenser.h"

ClientConfirmedBalanceDetectedMessageDispenser::ClientConfirmedBalanceDetectedMessageDispenser(QObject *destination, QObject *owner)
    : IRecycleableDispenser(destination, owner)
{ }
ClientConfirmedBalanceDetectedMessage *ClientConfirmedBalanceDetectedMessageDispenser::getNewOrRecycled()
{
    return static_cast<ClientConfirmedBalanceDetectedMessage*>(privateGetNewOrRecycled());
}
IRecycleableAndStreamable *ClientConfirmedBalanceDetectedMessageDispenser::newOfTypeAndConnectToDestinationObjectIfApplicable()
{
    ClientConfirmedBalanceDetectedMessage *clientConfirmedBalanceDetectedMessage = new ClientConfirmedBalanceDetectedMessage(this);
    return clientConfirmedBalanceDetectedMessage;
}
