#include "imessagedispenser.h"

IMessageDispenser::IMessageDispenser(QObject *mandatoryOwnerBeforeOwnerHasBeenMoveToThreaded)
    : QObject(mandatoryOwnerBeforeOwnerHasBeenMoveToThreaded) /*<- this is what I'm relying on to get this dispenser to follow business objects when they are .moveToThread'd. It makes the dispenser be a child of the "parent" mandatoryOwner (which is why it's mandatory)*/, m_DestinationObject(0) //TODOreq: before each connection to the destination object, make sure it isn't zero
{

}
void IMessageDispenser::setDestinationObject(IRpcBankServerMessageTransporter *destinationObject)
{
    m_DestinationObject = destinationObject;
}
IMessage *IMessageDispenser::privateGetNewOrRecycled()
{
    if(!m_RecycleList.isEmpty())
    {
        IMessage *recycledMessage = m_RecycleList.takeLast();
        //emit d(QString("getting recycled message of type: ") + QString(recycledMessage->metaObject()->className()) + QString(" on thread: ") + QString::number(QThread::currentThreadId()));
        emit d(QString("using recycled message of type: ") + recycledMessage->metaObject()->className() + QString(" on thread: ") + QString::number(QThread::currentThreadId()));
        return recycledMessage;
    }
    //else
    //new it and set it up for easy delivery
    IMessage *newMessage = getNewOfTypeAndConnectToDestinationObject();
    //connect easy recycling mechanism
    connect(newMessage, SIGNAL(doneWithMessageSignal()), this, SLOT(handleMessageReportingItselfDone())); //since newMessage _AND_ this are both technically owned by the same thread, i'm not sure if AutoConnection will send this message Queued. I could always force it, but for Actions (or broadcasts on rpc client side), we doooo want to use DirectConnection for this emit (since the dispenser user is also the recycler (transporter in this case)).
    //if my debugging shows that my handleMessageReportingItselfDone() code uses a different currentThread than the debug code shows for currentThread in getNewOrRecycled, then adding QueuedConnection to the above connect should fix it. just note that Actions should use DirectConnection... and the two reverse roles on the Rpc Client. actually no, the roles are the exact same on the rpc client. i was thinking of some other similarity.. can't remember what but i know it's there

    emit d(QString("got new message of type: ") + newMessage->metaObject()->className() + QString(" on thread: ") + QString::number(QThread::currentThreadId()));

    return newMessage;
}
void IMessageDispenser::handleMessageReportingItselfDone()
{
    IMessage *message = static_cast<IMessage*>(sender());

    emit d(QString("recycling message of type: ") + message->metaObject()->className() + QString(" on thread: ") + QString::number(QThread::currentThreadId()));

    m_RecycleList.append(message);
}
