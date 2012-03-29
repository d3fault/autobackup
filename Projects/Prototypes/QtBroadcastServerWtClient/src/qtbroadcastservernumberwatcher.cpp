#include "qtbroadcastservernumberwatcher.h"

QtBroadcastServerNumberWatcher::QtBroadcastServerNumberWatcher()
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
    //todo: set up random number generator on it's own thread, connect to it's signal
    //we could just keep sending ourselves unlimited events to simulate a while(true) loop...but it's better to only check our list of numbers that our clients are watching whenever the random number generator emits a new number generated

    qRegisterMetaType<WtClientCallback>("WtClientCallback"); //because invokeObject sends these as Q_ARG's
    m_UsersAndTheirNumbersBeingWatched = new QHash<QtBroadcastServerNumberWatcher::QtBroadcastServerClient*, UsersAndTheirNumbersBeingWatched*>();

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
        qDebug() << "connected";
    }
    //else, already connected...
}
void QtBroadcastServerNumberWatcher::watchForNum(QtBroadcastServerNumberWatcher::QtBroadcastServerClient *client, int numberToWatch)
{
    qDebug() << "qt broadcast server received request to watch for num: " << QString::number(numberToWatch);
    if(m_UsersAndTheirNumbersBeingWatched->contains(client))
    {
        UsersAndTheirNumbersBeingWatched *user = m_UsersAndTheirNumbersBeingWatched->value(client);
        if(!user->m_NumbersBeingWatched->contains(numberToWatch))
        {
            user->m_NumbersBeingWatched->append(numberToWatch);
            qDebug() << "now watching: " << QString::number(numberToWatch);
        }
        //else num already being watched
    }
    else
    {
        qDebug() << "not yet connected...";
    }
}
void QtBroadcastServerNumberWatcher::disconnectFromQtBroadcastServer(QtBroadcastServerNumberWatcher::QtBroadcastServerClient *client)
{
    if(m_UsersAndTheirNumbersBeingWatched->contains(client))
    {
        UsersAndTheirNumbersBeingWatched *user = m_UsersAndTheirNumbersBeingWatched->value(client);
        delete user->m_NumbersBeingWatched;
        m_UsersAndTheirNumbersBeingWatched->remove(client);
        delete user;
    }
}
void QtBroadcastServerNumberWatcher::handleRandomNumberGenerated(int num)
{
    QHashIterator<QtBroadcastServerNumberWatcher::QtBroadcastServerClient*, UsersAndTheirNumbersBeingWatched*> it(*m_UsersAndTheirNumbersBeingWatched);
    UsersAndTheirNumbersBeingWatched *userPtr;
    while(it.hasNext())
    {
        it.next();
        userPtr = it.value();
        if(userPtr->m_NumbersBeingWatched->contains(num))
        {
            qDebug() << "a number being watched, " + QString::number(num) + ", was generated. posting to session id: " << QString(userPtr->m_SessionId.c_str());
            Wt::WServer::instance()->post(userPtr->m_SessionId, boost::bind(userPtr->m_Callback, num)); //todo: we could do a callback function that takes the it.key(); as an input and all it does is removes is calls the disconnectFromQtBroadcastServer so that we don't post() to it ever again. we might even be able to just use the disconnectFromQt[...] function right in the binding!! to test that it works, comment out the call to disconnect in the finalize(); in the WApplication object... and add a QDebug call to disconnect of course
        }
    }

}
void QtBroadcastServerNumberWatcher::stop()
{
    //TODO: dealloc the QHash
    QMetaObject::invokeMethod(m_RandomNumberGenerator, "stop", Qt::BlockingQueuedConnection);
    QMetaObject::invokeMethod(m_RandomNumberGeneratorThread, "quit", Qt::QueuedConnection);
    m_RandomNumberGeneratorThread->wait();
}
