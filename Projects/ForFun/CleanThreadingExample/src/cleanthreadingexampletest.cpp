/*
Copyright (c) 2012, d3fault <d3faultdotxbe@gmail.com>
Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE
*/
#include "cleanthreadingexampletest.h"

CleanThreadingExampleTest::CleanThreadingExampleTest(QObject *parent) :
    QObject(parent), m_CleanedUpBackends(false)
{
    //Before starting the thread on which our backend object will live, we connect to the helper so that he can notify us (which happens right after the object is instantiated) when the object is ready to be connected to
    connect(&m_BackendObject1ThreadHelper, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleBackendObject1isReadyForConnections()));
    //Start the helper/thread. This is where our object gets instantiated and objectIsReadyForConnections() is emitted
    m_BackendObject1ThreadHelper.start();

    //Same thing, but for our second backend object/helper/thread
    connect(&m_BackendObject2ThreadHelper, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleBackendObject2isReadyForConnections()));
    m_BackendObject2ThreadHelper.start();

    //We connect the QCoreApplication::aboutToQuit() signal to our CleanThreadingExampleTest::handleAboutToQuit() slot. It has to be called using a Direct Connection, otherwise the application would continue shutting down (not letting us properly clean up) after the aboutToQuit() signal is emitted. The slot is called using a Qt::DirectConnection for us (Qt determines at runtime what thread a destination object/slot lives on). Since 'this' lives on the main/GUI thread, specifying Qt::DirectConnection is redundant
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()) /*, Qt::DirectConnection*/);

    connect(&m_Gui, SIGNAL(debugStopBackendsRequested()), this, SLOT(handleAboutToQuit()));

    //Show the widget/window:
    m_Gui.show();
}
CleanThreadingExampleTest::~CleanThreadingExampleTest()
{
    cleanupBackendObjectsIfNeeded(); //This is not necessary for this particular example, but if you're doing it wrong and your 'this' goes out of scope before QCoreApplication::aboutToQuit() is emitted, this is a safety measure to ensure that we've cleaned up our backend objects. Note that for the example this should (WILL) never happen. Looking at our example's main(), we know that QCoreApplication::aboutToQuit() will be emitted before CleanThreadingExampleTest ('this') goes out of scope... so ideally we'll be calling our cleanupBackendsIfNeeded() method from our handleAboutToQuit() slot [first]
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

        //If we do a direct connection then we'll beat a lot of the backend messages that are queued to the GUI lol... and the message appears in the midst of a bunch of hashes. Note that if you press "STOP" while hashing... wait() prevents us from updating the GUI. So it _DOES_ freeze. But since we're simulating a shutdown, this is fine. If we were doing a "STOP" that was NOT simulating a shutdown... we should not be wait()'ing on the thread right afterwards... so our GUI would not lock up. For the shutdown event, yes you do want your GUI to lock up while you wait for the backend thread. Normally it won't be doing something so stupid as hashing 500k hashes lmfao... so would respond faster? I guess I should try pressing CLOSE in the middle of hashing. I should see the same exact effect. The GUI freezes for a few seconds, then closes [because we don't have time to read anything once wait() finishes]. LoL just tried it. The result: the window closed right away, but the IDE stayed in debug mode (the app was still running) until the 500k hash cycle completed :-P. Makes perfect sense, since the Test, GUI, and Backend Threads are completely independent of each other. Just not what I was expecting at first (is why we test :-P)
        QMetaObject::invokeMethod(&m_Gui, "handleHashGenerated", Qt::QueuedConnection, Q_ARG(QString,"Backend 1 Stopped Successfully"));

        m_BackendObject2ThreadHelper.wait();
        QMetaObject::invokeMethod(&m_Gui, "handleRandomNumberResultsGathered", Q_ARG(QString,"Backend 2 Stopped Successfully"));

        //Don't clean up twice :-P
        m_CleanedUpBackends = true;
    }
}
void CleanThreadingExampleTest::handleBackendObject1isReadyForConnections()
{
    //When this slot is called, our backend object (the type we specified in the template parameter) is instantiated and ready to be connected to. It is on it's own thread, so calling methods it directly is unsafe (unless you use a QMutex to protect data accessed by both the thread and the method you call directly, of course. Connecting signals/slots to an object on another thread is thread-safe

    CleanThreadingBackend1 *backend1 = m_BackendObject1ThreadHelper.getObjectPointerForConnectionsOnly();

    //GUI signals ---> backend object slots
    //"Do Something!". Usually triggered by user input. Triggered by button clicks a lot of the time, but also triggered in this application by changing the random number generator's properties. Note that our GUI emits the "requested" signals after catching the clicked() and textChanged() signals and sanitizing the user input first, which is all handled in our GUI class
    connect(&m_Gui, SIGNAL(thrashHashStringNtimesRequested(const QString &, int)), backend1, SLOT(thrashHashStringNtimes(const QString &, int)));

    //backend signals ---> GUI slots
    //"Something happened!". It could be anything. This is how a backend threads safely update the GUI without locking it up. The signals are emitted from the backend threads... and after Qt's signals/slots awesomeness takes place, the slots are called on the GUI thread
    connect(backend1, SIGNAL(hashGenerated(const QString &)), &m_Gui, SLOT(handleHashGenerated(const QString &)));
}
void CleanThreadingExampleTest::handleBackendObject2isReadyForConnections()
{
    //All of the comments in handleBackendObject1isReadyForConnections() apply here too

    CleanThreadingBackend2 *backend2 = m_BackendObject2ThreadHelper.getObjectPointerForConnectionsOnly();

    connect(&m_Gui, SIGNAL(updateRandomNumberGeneratorPropertiesRequested(int,int)), backend2, SLOT(updateRandomNumberGeneratorProperties(int,int)));
    connect(backend2, SIGNAL(randomNumberResultsGathered(const QString &)), &m_Gui, SLOT(handleRandomNumberResultsGathered(const QString &)));
}
void CleanThreadingExampleTest::handleAboutToQuit()
{
    cleanupBackendObjectsIfNeeded();
}
