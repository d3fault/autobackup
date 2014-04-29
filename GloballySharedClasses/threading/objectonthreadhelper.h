#ifndef OBJECTONTHREADHELPER_H
#define OBJECTONTHREADHELPER_H

#include <QThread>
//#include <QStack> //destructor does LIFO, so i should too

//TODOoptional: something like, ObjectOnThreadManager::addObjectOnThread(ObjectType, "slotToBeNotifiedWhenObjectIsReadyForConnections"); can assume that slot is on the parent of ObjectOnThreadManager, or can have that be an argument, or could do both via overloads :-P. I guess the slot would expect a QObject*, but maybe I can figure out that casting of it to the actual type for schmexiness

//This base class is just a hack around MOC not playing nicely with templates
class ObjectOnThreadHelperBase : public QThread
{
    Q_OBJECT
public:
    explicit ObjectOnThreadHelperBase(QObject *parent = 0) : QThread(parent), m_TheObject(0) { }
    ~ObjectOnThreadHelperBase()
    {
        if(isRunning())
        {
            quit();
        }
        wait();
    }
    QObject *getBaseObjectPointerForConnectionsOnly()
    {
        return m_TheObject;
    }
    static void synchronouslyStopObjectOnThreadHelperIfNeeded(ObjectOnThreadHelperBase *objectOnThreadHelper)
    {
        if(objectOnThreadHelper)
        {
            if(objectOnThreadHelper->isRunning())
                objectOnThreadHelper->quit();
            objectOnThreadHelper->wait();
            delete objectOnThreadHelper;
            objectOnThreadHelper = 0;
        }
    }
    static void stopThreadIfNeeded(ObjectOnThreadHelperBase *objectOnThreadHelper)
    {
        if(objectOnThreadHelper)
        {
            if(objectOnThreadHelper->isRunning())
                objectOnThreadHelper->quit();
        }
    }
    static void waitOnAndDeleteThreadIfNeeded(ObjectOnThreadHelperBase *objectOnThreadHelper)
    {
        if(objectOnThreadHelper)
        {
            //should i call stopThreadIfNeeded(); ??? depends whether or not quit() makes isRunning() false instantly or not...
            objectOnThreadHelper->wait();
            delete objectOnThreadHelper;
            objectOnThreadHelper = 0;
        }
    }
protected:
    QObject *m_TheObject;
    virtual void run() = 0;
signals:
    void objectIsReadyForConnectionsOnly();
};

template<class UserObjectType>
class ObjectOnThreadHelper : public ObjectOnThreadHelperBase
{
public:
    explicit ObjectOnThreadHelper(QObject *parent = 0) : ObjectOnThreadHelperBase(parent) { }
    UserObjectType *getObjectPointerForConnectionsOnly()
    {
        return static_cast<UserObjectType*>(m_TheObject);
    }
protected:
    virtual void run()
    {
        //The user's object is allocated on the stack (not heap because this way if the object throws, it is still destroyed) right when the thread starts and we record it's address in a member pointer so the thread that created us can request it when they receive our objectIsReadyForConnectionsOnly() signal. Due to limitations with MOC, we cannot emit the object pointer as a signal parameter (because it is a templated type)
        UserObjectType theObject; //m_TheObject = new UserObjectType();
        m_TheObject = &theObject;

        //Tell whoever created us that the object is ready for connections
        emit objectIsReadyForConnectionsOnly();

        //Enter the event loop and do not leave it until QThread::quit() or QThread::terminate() are called
        exec();

        //After exec() returns (from calling QThread::quit() ideally), the object goes out of scope and is deleted and m_TheObject set back to zero
        m_TheObject = 0; //Just in case they ask for it again (they shouldn't)
    }
};

class ObjectOnThreadSynchronizer : public QObject
{
    Q_OBJECT
public:
    explicit ObjectOnThreadSynchronizer(QObject *parent = 0)
        : QObject(parent)
        , m_ObjectsLeftToSynchronize(0)
    { }
#if 0
    ~ObjectOnThreadSynchronizer()
    {
        while(!m_ObjectOnThreadsToQuitWhenDestructorIsCalled.isEmpty())
        {
            ObjectOnThreadHelperBase *objectOnThreadHelper = m_ObjectOnThreadsToQuitWhenDestructorIsCalled.pop();
            /*unsafe: if(objectOnThreadHelper->isRunning())
                objectOnThreadHelper->quit();*/

            //safe <3 Qt (i'm pretty sure calling quit twice (no isRunning check anymore because it isn't safe) is ok):
            QMetaObject::invokeMethod(objectOnThreadHelper, "quit");
        }
        emit internalSignalUsedToMakeThreadsQuit();
    }
#endif
    void addObjectToSynchronizer(ObjectOnThreadHelperBase *objectOnThreadHelper)
    {
        connect(objectOnThreadHelper, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleOneObjectIsReadyForConnections()));
        //connect(this, SIGNAL(internalSignalUsedToMakeThreadsQuit()), objectOnThreadHelper, SLOT(quit()));
        connect(this, SIGNAL(destroyed()), objectOnThreadHelper, SLOT(quit())); //now it's only not asynchronous if synchronizer isn't destroyed first, not a freaking segfault...
        //m_ObjectOnThreadsToQuitWhenDestructorIsCalled.push(objectOnThreadHelper);
        ++m_ObjectsLeftToSynchronize;
    }
private:
    int m_ObjectsLeftToSynchronize;
    //QStack<ObjectOnThreadHelperBase*> m_ObjectOnThreadsToQuitWhenDestructorIsCalled; //quit but not wait :-P
signals:
    void allObjectsOnThreadsReadyForConnections();
    //void internalSignalUsedToMakeThreadsQuit();
private slots:
    void handleOneObjectIsReadyForConnections()
    {
        if(--m_ObjectsLeftToSynchronize == 0)
        {
            emit allObjectsOnThreadsReadyForConnections();
        }
    }
};

#endif // OBJECTONTHREADHELPER_H
