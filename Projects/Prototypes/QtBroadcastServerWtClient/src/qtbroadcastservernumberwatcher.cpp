#include "qtbroadcastservernumberwatcher.h"

QtBroadcastServerNumberWatcher::QtBroadcastServerNumberWatcher()
    : m_UsersAndTheirNumbersBeingWatched(0)
{
    //don't do anything in here as the object hasn't been .moveToThread'd yet
}
QtBroadcastServerNumberWatcher *QtBroadcastServerNumberWatcher::m_pInstance = NULL;
QtBroadcastServerNumberWatcher * QtBroadcastServerNumberWatcher::Instance()
{
    if(!m_pInstance) //only one instance allowed
    {
        m_pInstance = new QtBroadcastServerNumberWatcher();
    }
    return m_pInstance;
}
void QtBroadcastServerNumberWatcher::init()
{
    qRegisterMetaType<WtClientCallback>("WtClientCallback"); //because invokeObject sends these as Q_ARG's
    qRegisterMetaType<QtBroadcastServerNumberWatcher::QtBroadcastServerClient*>("QtBroadcastServerNumberWatcher::QtBroadcastServerClient*");
    qRegisterMetaType<std::string>("std::string");

    if(!m_UsersAndTheirNumbersBeingWatched)
    {
        m_UsersAndTheirNumbersBeingWatched = new QHash<QtBroadcastServerNumberWatcher::QtBroadcastServerClient*, UsersAndTheirNumbersBeingWatched*>();
    }

    m_RandomNumberGeneratorThread = new QThread();
    m_RandomNumberGenerator = new MyRandomNumberGenerator();
    m_RandomNumberGenerator->moveToThread(m_RandomNumberGeneratorThread);
    m_RandomNumberGeneratorThread->start();

    connect(m_RandomNumberGenerator, SIGNAL(randomNumberGenerated(int)), this, SLOT(handleRandomNumberGenerated(int)), Qt::QueuedConnection);

    QMetaObject::invokeMethod(m_RandomNumberGenerator, "start", Qt::QueuedConnection);
}
void QtBroadcastServerNumberWatcher::connectToQtBroadcastServer(QtBroadcastServerNumberWatcher::QtBroadcastServerClient* client, std::string sessionId, WtClientCallback callback)
{
    if(!m_UsersAndTheirNumbersBeingWatched->contains(client))
    {
        UsersAndTheirNumbersBeingWatched *user = new UsersAndTheirNumbersBeingWatched();
        user->m_SessionId = sessionId;
        user->m_Callback = callback;
        user->m_NumbersBeingWatched = new QList<int>();

        m_UsersAndTheirNumbersBeingWatched->insert(client, user);
    }
    //else, already connected...
}
void QtBroadcastServerNumberWatcher::watchForNum(QtBroadcastServerNumberWatcher::QtBroadcastServerClient *client, int numberToWatch)
{
    if(m_UsersAndTheirNumbersBeingWatched->contains(client))
    {
        UsersAndTheirNumbersBeingWatched *user = m_UsersAndTheirNumbersBeingWatched->value(client);
        if(!user->m_NumbersBeingWatched->contains(numberToWatch))
        {
            user->m_NumbersBeingWatched->append(numberToWatch);
        }
        //else num already being watched
    }
    //else not yet connected
}
void QtBroadcastServerNumberWatcher::disconnectFromQtBroadcastServer(QtBroadcastServerNumberWatcher::QtBroadcastServerClient *client)
{
    if(m_UsersAndTheirNumbersBeingWatched->contains(client))
    {
        UsersAndTheirNumbersBeingWatched *user = m_UsersAndTheirNumbersBeingWatched->take(client);
        delete user->m_NumbersBeingWatched;
        delete user;
    }
}
void QtBroadcastServerNumberWatcher::handleRandomNumberGenerated(int num)
{
    QHashIterator<QtBroadcastServerNumberWatcher::QtBroadcastServerClient*, UsersAndTheirNumbersBeingWatched*> it(*m_UsersAndTheirNumbersBeingWatched);
    UsersAndTheirNumbersBeingWatched *userPtr;
    //it would be an optimization to keep sychronized a map that uses ALL "numbers being watched" as a key and the associated "users watching that key (their key into m_UsersAndTheirNumbersBeingWatched)" as a value (QList)... but for this simple ass example who gives a shit...
    while(it.hasNext())
    {
        it.next();
        userPtr = it.value();
        if(userPtr->m_NumbersBeingWatched->contains(num))
        {
            Wt::WServer::instance()->post(userPtr->m_SessionId, boost::bind(userPtr->m_Callback, num)); //todo: we could do a callback function that takes the it.key(); as an input and all it does is removes is calls the disconnectFromQtBroadcastServer so that we don't post() to it ever again. we might even be able to just use the disconnectFromQt[...] function right in the binding!! to test that it works, comment out the call to disconnect in the finalize(); in the WApplication object... and add a QDebug call to disconnect of course
        }
    }

}
void QtBroadcastServerNumberWatcher::stop()
{
    QMetaObject::invokeMethod(m_RandomNumberGenerator, "stop", Qt::BlockingQueuedConnection);
    m_RandomNumberGeneratorThread->quit(); //direct call it, since the thread that is currently executing is the owner of the QThread object. when we call .wait() on it on the next line, it blocks indefinitely and we never have a change to even receive/process the 'quit' event. actually BlockingQueued would solve this too. ok i am wrong, that causes a DeadLock (qt detects this)
    //QMetaObject::invokeMethod(m_RandomNumberGeneratorThread, "quit", Qt::BlockingQueuedConnection);
    m_RandomNumberGeneratorThread->wait();

    //this needs to be after the random number generator thread is done stopping because it raises signals that will cause us to access the hash...
    //dealloc the QHash
    if(m_UsersAndTheirNumbersBeingWatched)
    {
        QMutableHashIterator<QtBroadcastServerNumberWatcher::QtBroadcastServerClient*,UsersAndTheirNumbersBeingWatched*> it(*m_UsersAndTheirNumbersBeingWatched);
        UsersAndTheirNumbersBeingWatched *userPtr;
        while(it.hasNext())
        {
            it.next();
            userPtr = it.value();
            it.remove();
            delete userPtr->m_NumbersBeingWatched;
            delete userPtr;
        }
        delete m_UsersAndTheirNumbersBeingWatched;
        m_UsersAndTheirNumbersBeingWatched = 0;
    }
}
