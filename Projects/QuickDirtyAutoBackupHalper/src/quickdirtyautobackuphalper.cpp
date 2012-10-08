#include "quickdirtyautobackuphalper.h"

QuickDirtyAutoBackupHalper::QuickDirtyAutoBackupHalper(QObject *parent) :
    QObject(parent)
{
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()), Qt::DirectConnection);

    //observer pattern ftw (front-end observes backend! backend is oblivious. 'middle man' / adapter pattern is pointless extra work (and easily added later) imo). synchronous calls on a backend object on it's own thread sounds perfect [for now (I'm implying that I could very well be wrong. We'll see)]

    //TODOmb: QWidget::closeEvent should maybe synchronously ask the backend if it's ok to quit. a QMutex protected bool 'doingSomething'... that if true, cancels window quit procedure. so that when we get _HERE_ we know that we aren't in the middle of something. the reason it says 'mb' is because i _THINK_ the QThread::quit() is itself an event... and since I'm now doing synchronous processing on the backend thread that the quit() won't even be processed until the shit it's working on finishes (so we can PROCESS the quit()).
    //^^^I'm pretty sure that as long as my backend/business object (on it's own thread) does all of it's actions SYNCHRONOUSLY, this design will work quite well and easily. If I do asynchronous on backend, then the 'doingSomething' mutex protected bool becomes necessary
    //explaination: backendActionStep1() [slot], quit() [slot], backendActionStep2() [slot] <---- since it's async, we won't ever get to step2 and therefore the data is not safe. everything needs to be done in a single slot (synchronous)... but since we're on our own thread it won't block the GUI so fuck yea it's peachy and perfect I think

    m_Business.moveToThread(&m_ThreadBusiness);
    m_ThreadBusiness.start();

    m_Gui.setBusiness(&m_Business);
}
void QuickDirtyAutoBackupHalper::start()
{
    m_Gui.show();
    QMetaObject::invokeMethod(&m_Business, "start", Qt::QueuedConnection);
}
void QuickDirtyAutoBackupHalper::handleAboutToQuit()
{
    //I had these two calls after a.exec() returns it's integer until I read that it doesn't always return (lol windows). but now I wonder: what if wait() never returns (which would be because of bad coding!)?? does the shutdown never happen? TODOoptional: test that shit... too lazy right now
    QMetaObject::invokeMethod(&m_Business, "stop", Qt::BlockingQueuedConnection); //hope this works. what would it do if it never returns :-/, stop a shutdown? can shutdown override this call? that's potentially unsafe memory destruction. can it not? then can't i hang a shutdown?
    m_ThreadBusiness.quit();
    m_ThreadBusiness.wait();
}
