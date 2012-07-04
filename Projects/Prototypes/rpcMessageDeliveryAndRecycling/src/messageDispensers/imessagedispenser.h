#ifndef IMESSAGEDISPENSER_H
#define IMESSAGEDISPENSER_H

#include <QObject>
#include <QList>

#include "../messages/imessage.h"
#include "../irpcclientshelper.h"

class IRpcClientsHelper;

class IMessageDispenser : public QObject
{
    Q_OBJECT
public:
    IMessageDispenser(IRpcClientsHelper *destinationObject)
        : m_DestinationObject(destinationObject) { }

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
