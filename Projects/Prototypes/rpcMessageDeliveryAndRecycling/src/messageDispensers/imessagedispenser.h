#ifndef IMESSAGEDISPENSER_H
#define IMESSAGEDISPENSER_H

#include <QObject>
#include <QList>

#include "../messages/imessage.h"
#include "../irpcclientshelper.h"

class IMessageDispenser : public QObject
{
    Q_OBJECT
public:
    IMessage *privateGetNewOrRecycled()
    {
        if(!m_RecycleList.isEmpty())
        {
            return m_RecycleList.takeLast();
        }
        //else
        //new it and set it up for easy delivery
        IMessage *newItem = getNewOfTypeAndConnectToDestinationObject();
        //connect easy recycling mechanism
        connect(newItem, SIGNAL(doneWithMessageSignal()), this, SLOT(handleMessageReportingItselfDone())); //TODOreq: 'this' needs to live on the same thread of the one object that calls ::get on the dispenser
        return newItem;
    }
    void setMessageFinishedDestinationObject(IRpcClientsHelper *destinationObject) //this really does belong in IDeliver type shit, but w/e. not to be confused with changing the threads that the dispensers live on (which is specific only to broadcasts anyways (not sure if i'm going to remove the ability for actions to do it though (actually i shouldn't... because i will want to do that in the rpc client (not the helper))))
    {
        m_DestinationObject = destinationObject;
    }
private:
    IRpcClientsHelper *m_DestinationObject; //whenever we get a new of the specific type, the specific type will connect it's own specific signals to slots located on this destination object
    QList<IMessage*> m_RecycleList;
private slots:
    void handleMessageReportingItselfDone()
    {
        //privateRecycle(static_cast<IRecycleable*>(sender()));
        //^^nobody else calls privateRecycle, so save a method call by putting it here...

        m_RecycleList.append(static_cast<IMessage*>(sender()));
    }
protected:
    virtual IMessage *getNewOfTypeAndConnectToDestinationObject()=0;
};

#endif // IMESSAGEDISPENSER_H
