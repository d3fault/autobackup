#include "pendingbalanceaddeddetectedmessagedispenser.h"

IMessage *PendingBalanceAddedDetectedMessageDispenser::getNewOfTypeAndConnectToDestinationObject()
{
    PendingBalanceAddedDetectedMessage *newMessage = new PendingBalanceAddedDetectedMessage();
    connect(newMessage, SIGNAL(pendingBalancedAddedDetected()) ,m_DestinationObject, SLOT(pendingBalanceAddedDetected()));
    //TODO: connect failed signals etc. i can't think of how a broadcast would fail... but actions definitely will
    return newMessage;
}
