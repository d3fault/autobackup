#include "imessagedispenser.h"

IMessageDispenser::IMessageDispenser(QObject *destination, QObject *owner)
    : QObject(owner), m_Destination(destination)
{ }
IMessage *IMessageDispenser::privateGetNewOrRecycled()
{
    if(!m_RecycleList.isEmpty())
    {
        return m_RecycleList.takeLast();
    }
    IMessage *newMessage = getNewOfTypeAndConnectToDestinationObject();
    connect(newMessage, SIGNAL(doneWithMessageSignal()), this, SLOT(handleMessageReportingItselfDone()));
    return newMessage;
}
void IMessageDispenser::handleMessageReportingItselfDone()
{
    m_RecycleList.append(static_cast<IMessage*>(sender()));
}
