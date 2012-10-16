#include "qwidgetcleanobserverpatternexampletest.h"

QWidgetCleanObserverPatternExampleTest::QWidgetCleanObserverPatternExampleTest(QObject *parent) :
    QObject(parent)
{
    //Move both backend objects to their own threads, and also start the threads
    m_CleanObserverPatternExampleBackendA.moveToThread(&m_BackendThreadA);
    m_CleanObserverPatternExampleBackendB.moveToThread(&m_BackendThreadB);

    //HACK: You should never specify a priority (unless you KNOW what you're doing) in your applications. Just leave the start() constructor blank and inherit the priority instead
    //I guess thrashing the CPU like this wasn't really a great idea haha. Had to change it to IdlePriority to even see it updating (otherwise it updates in huge chunks instead of 1-line-at-a-time-scrolling).
    //I think more importantly than thrashing the CPU, I am thrashing Qt's event loop. The GUI event loop is RECEIVING too many update slots lmfao
    m_BackendThreadA.start(QThread::IdlePriority);
    m_BackendThreadB.start();

    //We connect the QCoreApplication aboutToQuit signal to our application's handleAboutToQuit slot. It has to be called using a Direct Connection, otherwise the application would continue shutting down (not letting us properly clean up) after the aboutToQuit signal is emitted. The slot is called using a Qt::DirectConnection for us (Qt determines at runtime what thread an object/slot lives on), since 'this' lives on the main/GUI thread. Specifying it is redundant
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()) /*, Qt::DirectConnection*/);

    //connect to our backend object(s)

    //GUI signals (triggered by button clicks a lot of the time, but also triggered in this application by changing the random number generator's properties. "Do Something!") ---> backend object slots
    connect(&m_Gui, SIGNAL(thrashHashStringNtimesRequested(const QString &, int)), &m_CleanObserverPatternExampleBackendA, SLOT(thrashHashStringNtimes(const QString &, int)));
    connect(&m_Gui, SIGNAL(updateRandomNumberGeneratorPropertiesRequested(int,int)), &m_CleanObserverPatternExampleBackendB, SLOT(updateRandomNumberGeneratorProperties(int,int)));

    //backend signals ("Something happened!") ---> GUI slots
    connect(&m_CleanObserverPatternExampleBackendA, SIGNAL(hashGenerated(const QString &)), &m_Gui, SLOT(handleHashGenerated(const QString &)));
    connect(&m_CleanObserverPatternExampleBackendB, SIGNAL(randomNumberResultsGathered(const QString &)), &m_Gui, SLOT(handleRandomNumberResultsGathered(const QString &)));

}
void QWidgetCleanObserverPatternExampleTest::startExample()
{
    //Show the widget/window:
    m_Gui.show();

    //Call the threadStartingUp slots in the correct thread via invokeMethod. This is how you can call a slot you have not yet connected a signal to. Since the destination object (first parameter) lives on a different thread than us (we move them to their own threads in our constructor), it is called via Qt::QueuedConnection, thus specifying it is redundant
    QMetaObject::invokeMethod(&m_CleanObserverPatternExampleBackendA, "threadStartingUp" /*, Qt::QueuedConnection*/);
    QMetaObject::invokeMethod(&m_CleanObserverPatternExampleBackendB, "threadStartingUp" /*, Qt::QueuedConnection*/);
}
void QWidgetCleanObserverPatternExampleTest::handleAboutToQuit()
{
    //Our last chance to do clean-up etc
    //Send both of our backend objects a heads up that we're about to quit. Let the threads clean up their own resources. threadShuttingDown is called via a Qt::QueuedConnection since m_CleanObserverPatternExampleBackendA lives on a different thread. Specifying it is redundant
    QMetaObject::invokeMethod(&m_CleanObserverPatternExampleBackendA, "threadShuttingDown" /*, Qt::QueuedConnection*/);
    //Post a message to the thread's event loop telling it to quit. This is an asynchronous call, we have to wait() for it to definitely be finished later on
    m_BackendThreadA.quit();
    QMetaObject::invokeMethod(&m_CleanObserverPatternExampleBackendB, "threadShuttingDown" /*, Qt::QueuedConnection*/);
    m_BackendThreadB.quit();

    //Wait on the backend thread(s) to clean up (threadShuttingDown) and quit. If we don't wait for them, the applicatioin will continue shutting down and the threads might not have cleaned themselves up properly. For example, they might write the contents of a QString applicationLog to file during threadShuttingDown. If you don't call wait(), the application may very well shutdown before or during that write to file
    m_BackendThreadA.wait();
    m_BackendThreadB.wait();
}
