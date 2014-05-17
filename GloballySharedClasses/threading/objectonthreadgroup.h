#ifndef OBJECTONTHREADGROUP_H
#define OBJECTONTHREADGROUP_H

#include <QStack>
#include <QThread>

//TODOoptional: make these classes actually private
class ObjectOnThreadGroupPrivateSingleThreadBase : public QThread
{
    Q_OBJECT
public:
    explicit ObjectOnThreadGroupPrivateSingleThreadBase(const char *slotOnParentsParentToInvokeWhenReadyForConnections, QObject *parent)
        : QThread(parent)
        , m_SlotOnParentsParentToInvokeWhenReadyForConnections(slotOnParentsParentToInvokeWhenReadyForConnections)
    { }
    ~ObjectOnThreadGroupPrivateSingleThreadBase()
    {
        if(isRunning())
            quit();
        wait();
    }
protected:
    const char *m_SlotOnParentsParentToInvokeWhenReadyForConnections;
    virtual void run() = 0;
signals:
    void objectIsInstantiatedOnThread(QObject *objectOnThread, const char *slotOnParentsParentToInvokeWhenReadyForConnections);
};
template<class UserObjectType>
class ObjectOnThreadGroupPrivateSingleThread : public ObjectOnThreadGroupPrivateSingleThreadBase
{
public:
    explicit ObjectOnThreadGroupPrivateSingleThread(const char *slotOnParentsParentToInvokeWhenReadyForConnections, QObject *parent)
        : ObjectOnThreadGroupPrivateSingleThreadBase(slotOnParentsParentToInvokeWhenReadyForConnections, parent)
    { }
protected:
    virtual void run()
    {
        UserObjectType userObject;
        emit objectIsInstantiatedOnThread(static_cast<QObject*>(&userObject), m_SlotOnParentsParentToInvokeWhenReadyForConnections);
        exec();
    }
};
//Object must derive from QObject, object must have default constructor overload
class ObjectOnThreadGroup : public QObject
{
    Q_OBJECT
public:
    explicit ObjectOnThreadGroup(QObject *parentContainingReadyForConnectionsSlots) //TODOoptional: could allow parent to be different from notifyee, and could even support overloads for addObjectOnThread so that each object has a unique notifyee (and the overload without a notifyee just uses the "all objects ready for connections" notifyee. would be best to use the new connect overloads for compile time checking that notifyee stuff is set up correctly (would maybe even be worth littering ifdef qt > 4 etc for backward compatibility). but actually since we only know it's a qobject that might not work anyways (without templating hacks (gl;hf))
        : QObject(parentContainingReadyForConnectionsSlots)
        , m_ObjectsLeftToSynchronize(0)
    { }
    ~ObjectOnThreadGroup()
    {
        while(!m_ObjectOnThreadsToQuitWhenDestructorIsCalled.isEmpty()) //this isn't strictly necessary, but doing it here instead of in the underlying thread's destructor means we can quit ALL of them before waiting on any of them (more async-er)
        {
            ObjectOnThreadGroupPrivateSingleThreadBase *objectOnThreadHelper = m_ObjectOnThreadsToQuitWhenDestructorIsCalled.pop();
            /*unsafe: if(objectOnThreadHelper->isRunning())
                objectOnThreadHelper->quit();*/

            //safe <3 Qt (i'm pretty sure calling quit twice (no isRunning check anymore because it isn't safe) is ok):
            QMetaObject::invokeMethod(objectOnThreadHelper, "quit");
        }
    }
    template<class ObjectType>
    void addObjectOnThread(const char *slotOnParentToBeNotifiedWhenObjectIsReadyForConnections)
    {
        ObjectOnThreadGroupPrivateSingleThreadBase *singleThreadForObject = new ObjectOnThreadGroupPrivateSingleThread<ObjectType>(slotOnParentToBeNotifiedWhenObjectIsReadyForConnections, this);
        connect(singleThreadForObject, SIGNAL(objectIsInstantiatedOnThread(QObject*,const char*)), this, SLOT(handleOneObjectIsInstantiatedOnThread(QObject*,const char*)));
        m_ObjectOnThreadsToQuitWhenDestructorIsCalled.push(singleThreadForObject);
        ++m_ObjectsLeftToSynchronize;

        singleThreadForObject->start(); //TODOoptional: right now all the ObjectTypes have to be added to thread manager in one unit of execution, otherwise allObjectsOnThreadsReadyForConnections may be emitted multiple times. i could fix that by using an optional delayed start. but perhaps delayed start (manual start) should be the default since it's safer? still starting here/now is faster/etc
    }
private:
    QStack<ObjectOnThreadGroupPrivateSingleThreadBase*> m_ObjectOnThreadsToQuitWhenDestructorIsCalled; //quit but not wait :-P (async). TODOreq: perhaps an "all have finished" signal... but idk i kinda like having it all be transparent in destruction phase
    quint32 m_ObjectsLeftToSynchronize;
signals:
    void allObjectsOnThreadsInGroupReadyForConnections();
private slots:
    void handleOneObjectIsInstantiatedOnThread(QObject *objectOnThread, const char *slotOnParentToBeNotifiedWhenObjectIsReadyForConnections)
    {
        QMetaObject::invokeMethod(parent(), slotOnParentToBeNotifiedWhenObjectIsReadyForConnections, Q_ARG(QObject*, objectOnThread));
        if(--m_ObjectsLeftToSynchronize == 0)
        {
            emit allObjectsOnThreadsInGroupReadyForConnections();
        }
    }
};

#endif // OBJECTONTHREADGROUP_H
