#include "irecycleabledispenser.h"

IRecycleableDispenser::IRecycleableDispenser(QObject *destination, QObject *owner)
    : QObject(owner), m_Destination(destination)
{ }
IRecycleableAndStreamable *IRecycleableDispenser::privateGetNewOrRecycled()
{
    if(!m_RecycleList.isEmpty())
    {
        return m_RecycleList.takeLast();
    }
    IRecycleableAndStreamable *newMessage = newOfTypeAndConnectToDestinationObjectIfApplicable();
    connect(newMessage, SIGNAL(doneWithMessageSignal()), this, SLOT(handleMessageReportingItselfDone()));
    return newMessage;
}
void IRecycleableDispenser::handleMessageReportingItselfDone()
{
    m_RecycleList.append(static_cast<IRecycleableAndStreamable*>(sender()));
}
