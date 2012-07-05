#ifndef IMESSAGEDISPENSER_H
#define IMESSAGEDISPENSER_H

#include <QObject>
#include <QList>
#include <QThread> //for debugging getting threadId

#include "../messages/imessage.h"
#include "../irpcclientshelper.h"

class IRpcClientsHelper;

class IMessageDispenser : public QObject
{
    Q_OBJECT
public:
    IMessageDispenser(IRpcClientsHelper *destinationObject)
        : m_DestinationObject(destinationObject) { }
private:
    QList<IMessage*> m_RecycleList;
private slots:
    void handleMessageReportingItselfDone()
    {
        emit d(QString("recycling message of type: ") + QString(sender()->metaObject()->className()) + QString(" on thread: ") + QString::number(QThread::currentThreadId()) + QString("\n\n"));
        //privateRecycle(static_cast<IRecycleable*>(sender()));
        //^^nobody else calls privateRecycle, so save a method call by putting it here...

        m_RecycleList.append(static_cast<IMessage*>(sender()));
    }
signals:
    void d(const QString &);
protected:
    virtual IMessage *getNewOfTypeAndConnectToDestinationObject()=0;
    IRpcClientsHelper *m_DestinationObject;

    IMessage *privateGetNewOrRecycled()
    {
        if(!m_RecycleList.isEmpty())
        {
            IMessage *recycledMessage = m_RecycleList.takeLast();
            emit d(QString("getting recycled message of type: ") + QString(recycledMessage->metaObject()->className()) + QString(" on thread: ") + QString::number(QThread::currentThreadId()));
            return recycledMessage;
        }
        //else
        //new it and set it up for easy delivery
        IMessage *newItem = getNewOfTypeAndConnectToDestinationObject();
        //emit d(QString("new'ing message of type: ") + QString(newItem->metaObject()->className()) + QString(" on thread: ") + QString::number(QThread::currentThreadId()));
        //connect easy recycling mechanism
        connect(newItem, SIGNAL(doneWithMessageSignal()), this, SLOT(handleMessageReportingItselfDone())); //TODOreq: 'this' needs to live on the same thread of the one object that calls ::get on the dispenser
        return newItem;
    }
};

#endif // IMESSAGEDISPENSER_H
