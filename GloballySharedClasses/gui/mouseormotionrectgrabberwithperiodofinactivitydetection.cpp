#include "mouseormotionrectgrabberwithperiodofinactivitydetection.h"

#include <QGuiApplication>
#include <QScreen>
#include <QPixmap>

#include "mouseormotionorperiodofinactivitydetector.h"

//thin class is thin, but clean OO code is clean
MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::MouseOrMotionRectGrabberWithPeriodOfInactivityDetection(QObject *parent)
    : QObject(parent)
    , m_Screen(QGuiApplication::primaryScreen())
{ }
void MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::startGrabbingMouseOrMotionRectsWhileAlsoDetectingPeriodsOfInactivity(const QSize &rectAroundMouseOrMotionToGrab, int pollRateMSec, int amountOfTimeMSecWithNoMouseOrMotionActivityToBeConsideredAPeriodOfInactivity)
{
    m_Rect = rectAroundMouseOrMotionToGrab;

    MouseOrMotionOrPeriodOfInactivityDetector *detector = new MouseOrMotionOrPeriodOfInactivityDetector(this);
    connect(detector, SIGNAL(mouseMovementDetected(QPoint)), this, SLOT(handleMouseMovementDetected(QPoint)));
    connect(detector, SIGNAL(motionOnScreenDetected(QPoint,QImage)), this, SLOT(handleMotionOnScreenDetected(QPoint,QImage)));
    connect(detector, SIGNAL(periodOfInactivityDetected()), this, SIGNAL(periodOfInactivityDetected()));

    detector->startDetectingMouseOrMotionOrPeriodsOfInactivity(pollRateMSec, amountOfTimeMSecWithNoMouseOrMotionActivityToBeConsideredAPeriodOfInactivity);
}
void MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::handleMouseMovementDetected(const QPoint &newMousePos)
{
    QPixmap rectAroundTheCursorPixmap = m_Screen->grab(0, m_Rect.x(), m_Rect.y(), m_Rect.width(), m_Rect.height());
    QImage rectAroundTheCursorImage = rectAroundTheCursorPixmap.toImage();
    emit mouseOrMotionRectGrabbed(newMousePos, rectAroundTheCursorImage, Mouse);

    //QImage screenGrab = m_Screen.grab(); //TO DOneoptimization(oh duh it does, screen motion will use tha- wait no it won't xD): I think grab() accepts a rect actually...
    //grabRectAroundPointOnImageAndEmitIt(newMousePos, screenGrab, Mouse);
}
void MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::handleMotionOnScreenDetected(const QPoint &pointOnImageAroundWhichToGrabRect, const QImage &imageToGrabRectFrom)
{
    QImage rectAroundTheScreenMovementImage = imageToGrabRectFrom.copy(m_Rect);
    emit mouseOrMotionRectGrabbed(pointOnImageAroundWhichToGrabRect, rectAroundTheScreenMovementImage, Motion);
}
