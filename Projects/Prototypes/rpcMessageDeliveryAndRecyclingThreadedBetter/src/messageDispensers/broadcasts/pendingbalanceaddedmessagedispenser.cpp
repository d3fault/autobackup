#include "pendingbalanceaddedmessagedispenser.h"

PendingBalanceAddedMessageDispenser::PendingBalanceAddedMessageDispenser(QObject *mandatoryParent)
    : IMessageDispenser(mandatoryParent)
{ }
PendingBalanceAddedMessage *PendingBalanceAddedMessageDispenser::getNewOrRecycled()
{
    return static_cast<PendingBalanceAddedMessage*>(privateGetNewOrRecycled());
}
IMessage *PendingBalanceAddedMessageDispenser::getNewOfTypeAndConnectToDestinationObject()
{
    PendingBalanceAddedMessage *newMessage = new PendingBalanceAddedMessage(this); //the 'this' is vital to correct ownership, i think
    connect(newMessage, SIGNAL(), m_DestinationObject, SLOT(pendingBalanceDetected()));
    //TODO: connect failed signals etc. i can't think of how a broadcast would fail... but actions definitely will. the failed signals are added in the specific/implemented message type
    return newMessage;
}
