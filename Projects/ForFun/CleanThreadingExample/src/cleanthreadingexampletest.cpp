#include "cleanthreadingexampletest.h"

CleanThreadingExampleTest::CleanThreadingExampleTest(QObject *parent) :
    QObject(parent), m_CleanedUpBackends(false)
{
    //Before starting the thread on which our backend object will live, we connect to the helper so that he can notify us (which happens right after the object is instantiated) when the object is ready to be connected to
    connect(&m_BackendObject1ThreadHelper, SIGNAL(objectCleanThreadingBackend1IsReadyForConnectionsOnly(CleanThreadingBackend1*)), this, SLOT(handleBackendObject1isReadyForConnections(CleanThreadingBackend1*)));
    //Start the helper/thread. This is where our object gets instantiated and objectIsReadyForConnections() is emitted
    m_BackendObject1ThreadHelper.start();

    //Same thing, but for our second backend object/helper/thread
    connect(&m_BackendObject2ThreadHelper, SIGNAL(objectCleanThreadingBackend2IsReadyForConnectionsOnly(CleanThreadingBackend2*)), this, SLOT(handleBackendObject2isReadyForConnections(CleanThreadingBackend2*)));
    m_BackendObject2ThreadHelper.start();

    //We connect the QCoreApplication::aboutToQuit() signal to our CleanThreadingExampleTest::handleAboutToQuit() slot. It has to be called using a Direct Connection, otherwise the application would continue shutting down (not letting us properly clean up) after the aboutToQuit() signal is emitted. The slot is called using a Qt::DirectConnection for us (Qt determines at runtime what thread a destination object/slot lives on). Since 'this' lives on the main/GUI thread, specifying Qt::DirectConnection is redundant
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()) /*, Qt::DirectConnection*/);

    //Show the widget/window:
    m_Gui.show();
}
CleanThreadingExampleTest::~CleanThreadingExampleTest()
{
    cleanupBackendObjectsIfNeeded(); //This is not necessary for this particular example, but if you're doing it wrong and your 'this' goes out of scope before QCoreApplication::aboutToQuit() is emitted, this is a safety measure to ensure that we've cleaned up our backend objects. Note that for the example this should (WILL) never happen. Looking at our main(), QCoreApplication::aboutToQuit() will be emitted before CleanThreadingExampleTest ('this') goes out of scope... so ideally we'll be calling our cleanupBackendsIfNeeded() method from our handleAboutToQuit() slot
}
void CleanThreadingExampleTest::cleanupBackendObjectsIfNeeded()
{
    if(!m_CleanedUpBackends) //See comment in our destructor for justfication of this bool
    {
        //Post a message to the thread's event loop telling it to quit. This is an asynchronous call, we have to wait() for it to definitely be finished later on. After the thread quits, exec() in QThread::run() returns and our backend object goes out of scope, allowing our destructor to run on the thread just before it finishes
        m_BackendObject1ThreadHelper.quit();

        //Same thing, but for our second backend object/helper/thread
        m_BackendObject2ThreadHelper.quit();

        //Wait on the backend object/helper/threads to:
        // a) Finish whatever they're currently doing. They get until the end of whatever slot they're currently in, and that's it
        // b) Process the quit() message we just sent, which tells the exec() call in QThread::run() to return
        // c) Run the destructor for our object types implicitly, just before QThread::run() goes out of scope
        // d) End the actual thread (when QThread::run() goes out of scope)
        //If We don't wait for them, the applicatioin will continue shutting down and the threads might not have cleaned themselves up properly. For example, the backend threads might write the contents of a "QString m_EventLog" to file in their destructor. If you don't call wait(), the application will continue shutting down before or during that write to file... resulting in data loss/corruption which could be disastrous to you
        m_BackendObject1ThreadHelper.wait();
        m_BackendObject2ThreadHelper.wait();

        //Don't clean up twice :-P
        m_CleanedUpBackends = true;
    }
}
void CleanThreadingExampleTest::handleBackendObject1isReadyForConnections(CleanThreadingBackend1 *backend1)
{
    //When this slot is called, our backend object (the type we specified in the template parameter) is instantiated and ready to be connected to. It is on it's own thread, so calling methods it directly is unsafe (unless you use a QMutex to protect data accessed by both the thread and the method you call directly, of course. Connecting signals/slots to an object on another thread is thread-safe

    //GUI signals ---> backend object slots
    //"Do Something!". Usually triggered by user input. Triggered by button clicks a lot of the time, but also triggered in this application by changing the random number generator's properties. Note that our GUI emits the "requested" signals after catching the clicked() and textChanged() signals and first sanitizing the user input, which is all handled in our GUI class
    connect(&m_Gui, SIGNAL(thrashHashStringNtimesRequested(const QString &, int)), backend1, SLOT(thrashHashStringNtimes(const QString &, int)));

    //backend signals ---> GUI slots
    //"Something happened!". It could be anything. This is how a backend threads safely update the GUI without locking it up. The signals are emitted from the backend threads... and after Qt's signals/slots awesomeness takes place: the slots are called on the GUI thread
    connect(backend1, SIGNAL(hashGenerated(const QString &)), &m_Gui, SLOT(handleHashGenerated(const QString &)));
}
void CleanThreadingExampleTest::handleBackendObject2isReadyForConnections(CleanThreadingBackend2 *backend2)
{
    //All of the comments in handleBackendObject1isReadyForConnections() apply here too

    connect(&m_Gui, SIGNAL(updateRandomNumberGeneratorPropertiesRequested(int,int)), backend2, SLOT(updateRandomNumberGeneratorProperties(int,int)));
    connect(backend2, SIGNAL(randomNumberResultsGathered(const QString &)), &m_Gui, SLOT(handleRandomNumberResultsGathered(const QString &)));
}
void CleanThreadingExampleTest::handleAboutToQuit()
{
    cleanupBackendObjectsIfNeeded();
}
