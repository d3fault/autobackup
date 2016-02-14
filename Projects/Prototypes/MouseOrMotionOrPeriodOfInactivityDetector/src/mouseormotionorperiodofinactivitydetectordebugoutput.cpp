#include "mouseormotionorperiodofinactivitydetectordebugoutput.h"

MouseOrMotionOrPeriodOfInactivityDetectorDebugOutput::MouseOrMotionOrPeriodOfInactivityDetectorDebugOutput(QWidget *parent)
    : QPlainTextEdit(parent)
{ }
void MouseOrMotionOrPeriodOfInactivityDetectorDebugOutput::handleMouseMovementDetected(const QPoint &mousePos)
{
    appendPlainText("Mouse movment detected at: " + QString::number(mousePos.x()) + ", " + QString::number(mousePos.y()));
}
void MouseOrMotionOrPeriodOfInactivityDetectorDebugOutput::handleScreenMovementDetected(const QPoint &positionOnScreen, const QImage &theScreenGrab)
{
    appendPlainText("Screen movment detected at: " + QString::number(positionOnScreen.x()) + ", " + QString::number(positionOnScreen.y()));
    Q_UNUSED(theScreenGrab)
}
void MouseOrMotionOrPeriodOfInactivityDetectorDebugOutput::handlePeriodOfInactivityDetected()
{
    appendPlainText("Period of inactivity detected");
}
