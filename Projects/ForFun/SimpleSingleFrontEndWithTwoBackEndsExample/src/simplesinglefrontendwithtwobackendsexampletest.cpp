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
#include "simplesinglefrontendwithtwobackendsexampletest.h"

SimpleSingleFrontendWithTwoBackendsExampleTest::SimpleSingleFrontendWithTwoBackendsExampleTest(QObject *parent) :
    QObject(parent)
{
    //Move both backend objects to their own threads, and also start the threads. This allows them to do whatever busy work they please... blocking their own threads at will... without locking up the GUI
    m_SimpleSingleBackend1.moveToThread(&m_BackendThread1);
    m_SimpleSingleBackend2.moveToThread(&m_BackendThread2);
    m_BackendThread1.start();
    m_BackendThread2.start();

    //We connect the QCoreApplication aboutToQuit signal to our application's handleAboutToQuit slot. It has to be called using a Direct Connection, otherwise the application would continue shutting down (not letting us properly clean up) after the aboutToQuit signal is emitted. The slot is called using a Qt::DirectConnection for us (Qt determines at runtime what thread an object/slot lives on), since 'this' lives on the main/GUI thread. Specifying it is redundant
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()) /*, Qt::DirectConnection*/);


    //connect to our backend objects:

    //GUI signals ---> backend object slots
    //"Do Something!". Usually triggered by user input. Triggered by button clicks a lot of the time, but also triggered in this application by changing the random number generator's properties. Note that our GUI emits the "requested" signals after catching the clicked() and textChanged() signals and first sanitizing the user input, which is all handled in our GUI class
    connect(&m_Gui, SIGNAL(thrashHashStringNtimesRequested(const QString &, int)), &m_SimpleSingleBackend1, SLOT(thrashHashStringNtimes(const QString &, int)));
    connect(&m_Gui, SIGNAL(updateRandomNumberGeneratorPropertiesRequested(int,int)), &m_SimpleSingleBackend2, SLOT(updateRandomNumberGeneratorProperties(int,int)));

    //backend signals ---> GUI slots
    //"Something happened!". It could be anything. This is how a backend threads safely update the GUI without locking it up. The signals are emitted from the backend threads... and after Qt's signals/slots awesomeness takes place: the slots are called on the GUI thread
    connect(&m_SimpleSingleBackend1, SIGNAL(hashGenerated(const QString &)), &m_Gui, SLOT(handleHashGenerated(const QString &)));
    connect(&m_SimpleSingleBackend2, SIGNAL(randomNumberResultsGathered(const QString &)), &m_Gui, SLOT(handleRandomNumberResultsGathered(const QString &)));

}
void SimpleSingleFrontendWithTwoBackendsExampleTest::startExample()
{
    //Show the widget/window:
    m_Gui.show();

    //Call the threadStartingUp slots in the correct thread via invokeMethod. This is how you can call a slot you have not yet connected a signal to. Since the destination object (first parameter) lives on a different thread than us (we move them to their own threads in our constructor), Qt automatically calls them via Qt::QueuedConnection, thus specifying it is redundant
    QMetaObject::invokeMethod(&m_SimpleSingleBackend1, "threadStartingUp" /*, Qt::QueuedConnection*/);
    QMetaObject::invokeMethod(&m_SimpleSingleBackend2, "threadStartingUp" /*, Qt::QueuedConnection*/);
}
void SimpleSingleFrontendWithTwoBackendsExampleTest::handleAboutToQuit()
{
    //Our last chance to do clean-up etc
    //Send both of our backend objects a heads up that we're about to quit. Let the threads clean up their own resources. threadShuttingDown is called via a Qt::QueuedConnection since m_SimpleSingleBackend1 lives on a different thread. Specifying it is redundant
    QMetaObject::invokeMethod(&m_SimpleSingleBackend1, "threadShuttingDown" /*, Qt::QueuedConnection*/);
    //Post a message to the thread's event loop telling it to quit. This is an asynchronous call, we have to wait() for it to definitely be finished later on
    m_BackendThread1.quit();
    //Same thing, but for the second backend object
    QMetaObject::invokeMethod(&m_SimpleSingleBackend2, "threadShuttingDown" /*, Qt::QueuedConnection*/);
    m_BackendThread2.quit();

    //Wait on the backend threads to finish cleaning up (threadShuttingDown) and quit. If we don't wait for them, the applicatioin will continue shutting down and the threads might not have cleaned themselves up properly. For example, the backend threads might write the contents of a "QString m_EventLog" to file during threadShuttingDown. If you don't call wait(), the application may very well shutdown before or during that write to file... resulting in data loss which could be disastrous to you
    m_BackendThread1.wait();
    m_BackendThread2.wait();
}
