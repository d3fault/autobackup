#include "mouseormotionrectgrabberwithperiodofinactivitydetectiongui.h"

#include "objectonthreadgroup.h"
#include "mouseormotionrectgrabberwithperiodofinactivitydetection.h"

MouseOrMotionRectGrabberWithPeriodOfInactivityDetectionGui::MouseOrMotionRectGrabberWithPeriodOfInactivityDetectionGui(QObject *parent)
    : QObject(parent)
{
    ObjectOnThreadGroup *backendThread = new ObjectOnThreadGroup(this);
    backendThread->addObjectOnThread<MouseOrMotionRectGrabberWithPeriodOfInactivityDetection>("mouseOrMotionRectGrabberWithPeriodOfInactivityDetectionReadyForConnections");
    backendThread->doneAddingObjectsOnThreads();
}
void MouseOrMotionRectGrabberWithPeriodOfInactivityDetectionGui::mouseOrMotionRectGrabberWithPeriodOfInactivityDetectionReadyForConnections(QObject *mouseOrMotionRectGrabberWithPeriodOfInactivityAsQObject)
{
    MouseOrMotionRectGrabberWithPeriodOfInactivityDetection *business = static_cast<MouseOrMotionRectGrabberWithPeriodOfInactivityDetection*>(mouseOrMotionRectGrabberWithPeriodOfInactivityAsQObject);
    m_Gui.reset(new MouseOrMotionRectGrabberWithPeriodOfInactivityDetectionWidget());

    connect(business, SIGNAL(mouseOrMotionRectGrabbed(QPoint,QVariant,MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::MouseOrMotionEnum)), m_Gui.data(), SLOT(handleMotionOrMotionRectGrabbed(QPoint,QVariant,MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::MouseOrMotionEnum)));
    connect(business, SIGNAL(periodOfInactivityDetected()), m_Gui.data(), SLOT(handlePeriodOfInactivityDetected()));

    m_Gui->show();
    QSize rectSize(160, 128);
    m_Gui->resize(rectSize);
    QMetaObject::invokeMethod(business, "startGrabbingMouseOrMotionRectsWhileAlsoDetectingPeriodsOfInactivity", Q_ARG(QSize,rectSize), Q_ARG(int,2000), Q_ARG(int,10000), Q_ARG(double,1.0), Q_ARG(QVariant::Type,QVariant::Pixmap));
}
