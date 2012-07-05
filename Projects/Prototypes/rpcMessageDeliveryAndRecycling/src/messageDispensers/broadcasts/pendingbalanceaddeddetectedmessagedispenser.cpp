#include "pendingbalanceaddeddetectedmessagedispenser.h"

#include "../../messages/broadcasts/pendingbalanceaddeddetectedmessage.h"

IMessage *PendingBalanceAddedDetectedMessageDispenser::getNewOfTypeAndConnectToDestinationObject()
{
    emit d(QString("new'ing pending balance message on what should be bitcoin thread: ") + QString::number(QThread::currentThreadId()));
    emit d("new'ing a pending balance message");
    PendingBalanceAddedDetectedMessage *newMessage = new PendingBalanceAddedDetectedMessage();
    connect(newMessage, SIGNAL(pendingBalanceAddedDetected()), m_DestinationObject, SLOT(pendingBalanceAddedDetected()));
    //TODO: connect failed signals etc. i can't think of how a broadcast would fail... but actions definitely will
    return newMessage;
}
PendingBalanceAddedDetectedMessage *PendingBalanceAddedDetectedMessageDispenser::getNewOrRecycled()
{
    return static_cast<PendingBalanceAddedDetectedMessage*>(privateGetNewOrRecycled());
}
