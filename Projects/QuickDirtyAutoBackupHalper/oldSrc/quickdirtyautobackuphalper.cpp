#include "quickdirtyautobackuphalper.h"

QuickDirtyAutoBackupHalper::QuickDirtyAutoBackupHalper(QObject *parent) :
    QObject(parent)
{
}
void QuickDirtyAutoBackupHalper::start()
{
    m_Gui.setHalper(this);
    m_Gui.show();

    m_MutexBusiness = m_Business.getWorkerMutex();

    m_Business.moveToThread(&m_ThreadBusiness);
    m_ThreadBusiness.start();

    //connections between business and gui go here...

    QMetaObject::invokeMethod(&m_Business, "start", Qt::QueuedConnection);
}
bool QuickDirtyAutoBackupHalper::canCloseBecauseBusinessIsIdleAndWeStoppedItOk()
{
    //this line guarantees us that the business will be idle when we start the shutdown procedure and makes it so we don't start a new one in the meantime
    QMutexLocker scopedLock(m_MutexBusiness);

    if(m_Business.isIdle())
    {
        //once we've quit and wait'd on the thread, it doesn't matter anymore when we release the mutex. actually just to be safe let's set a done flag

        m_Business.setDone(true); //we use our own event loop (still running ;-)) to call this method. actually fuck it ima move it above the quit/wait calls

        m_ThreadBusiness.quit();
        m_ThreadBusiness.wait();

        return true;
    }
    else //in the middle of something... ANYTHING... don't quit
    {
        return false;
    }
}
