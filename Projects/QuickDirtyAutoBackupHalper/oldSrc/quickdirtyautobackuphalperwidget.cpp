#include "quickdirtyautobackuphalperwidget.h"

#include "quickdirtyautobackuphalper.h"

QuickDirtyAutoBackupHalperWidget::QuickDirtyAutoBackupHalperWidget(QWidget *parent)
    : QWidget(parent), m_Halper(0)
{ }
void QuickDirtyAutoBackupHalperWidget::setHalper(QuickDirtyAutoBackupHalper *halper)
{
    m_Halper = halper;
}
QuickDirtyAutoBackupHalperWidget::~QuickDirtyAutoBackupHalperWidget()
{
    
}
void QuickDirtyAutoBackupHalperWidget::closeEvent(QCloseEvent *closeEvent)
{
    //this is a blocking function that waits for the business and it's thread to finish what it's doing. since we're trying to close, blocking is ok for data safety / memory leaks etc
    //we are ONLY blocking because of the Qt::DirectConnection flag...
    //...but then again the m_Halper is on the same thread as us so AutoConnection would be Direct anyways...

    bool closeAccepted = false;
    if(m_Halper)
    {
        QMetaObject::invokeMethod(m_Halper, "canCloseBecauseBusinessIsIdleAndWeStoppedItOk", Qt::DirectConnection, Q_RETURN_ARG(bool,closeAccepted));
    }

    if(closeAccepted)
    {
        closeEvent->accept();
    }
    else
    {
        closeEvent->ignore();
    }
}
