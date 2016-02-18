#include "mouseormotionrectgrabberwithperiodofinactivitydetection.h"

#include <QGuiApplication>
#include <QScreen>
#include <QPixmap>

#include "mouseormotionorperiodofinactivitydetector.h"

//thin class is thin, but clean OO code is clean
MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::MouseOrMotionRectGrabberWithPeriodOfInactivityDetection(QObject *parent)
    : QObject(parent)
    , m_Screen(QGuiApplication::primaryScreen())
    , m_ScreenResolution(m_Screen->size())
{ }
QRect MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::makeRectAroundPointStayingWithinResolution(const QPoint &inputPoint)
{
    QSize targetSize; //taking into account zoom factor
    targetSize.setWidth(m_RectSize.width()/m_ZoomFactor);
    targetSize.setHeight(m_RectSize.height()/m_ZoomFactor);

    //start with a normal calculation of the top-left corner (may be out of bounds)
    QPoint topLeft(inputPoint.x()-(targetSize.width()/2),
              inputPoint.y()-(targetSize.height()/2));

    //then adjust it to make sure it's within bounds. TODOreq: m_RectSize has to be smaller than m_ScreenResolution, otherwise we can m_Screen->grabWindow _outside_ of the resolution. so these bounds checks need to also adjust the width/height under certain/rarish circumstances. I think the zoom factor might effect it too
    if(topLeft.x() < 0)
        topLeft.setX(0);
    if(topLeft.y() < 0)
        topLeft.setY(0);
    if(topLeft.x() > (m_ScreenResolution.width()-targetSize.width()))
        topLeft.setX(m_ScreenResolution.width()-targetSize.width());
    if(topLeft.y() > (m_ScreenResolution.height()-targetSize.height()))
        topLeft.setY(m_ScreenResolution.height()-targetSize.height());

    QRect ret(topLeft.x(), topLeft.y(), targetSize.width(), targetSize.height());
    return ret;
}
void MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::scaleIfZoomFactorWarrantsIt(QImage *imageToMaybeScale)
{
    if(m_ZoomFactor != 1.0)
        (*imageToMaybeScale) = (*imageToMaybeScale).scaled(m_RectSize);
}
void MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::startGrabbingMouseOrMotionRectsWhileAlsoDetectingPeriodsOfInactivity(const QSize &rectSizeAroundMouseOrMotionToGrab, int pollRateMSec, int amountOfTimeMSecWithNoMouseOrMotionActivityToBeConsideredAPeriodOfInactivity, double zoomFactor)
{
    m_RectSize = rectSizeAroundMouseOrMotionToGrab;
    m_ZoomFactor = zoomFactor;

    MouseOrMotionOrPeriodOfInactivityDetector *detector = new MouseOrMotionOrPeriodOfInactivityDetector(this);
    connect(detector, SIGNAL(mouseMovementDetected(QPoint)), this, SLOT(handleMouseMovementDetected(QPoint)));
    connect(detector, SIGNAL(motionOnScreenDetected(QPoint,QImage)), this, SLOT(handleMotionOnScreenDetected(QPoint,QImage)));
    connect(detector, SIGNAL(periodOfInactivityDetected()), this, SIGNAL(periodOfInactivityDetected()));

    detector->startDetectingMouseOrMotionOrPeriodsOfInactivity(pollRateMSec, amountOfTimeMSecWithNoMouseOrMotionActivityToBeConsideredAPeriodOfInactivity);
}
void MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::handleMouseMovementDetected(const QPoint &newMousePos)
{
    QRect rectAroundMouse = makeRectAroundPointStayingWithinResolution(newMousePos);
    QPixmap rectAroundTheCursorPixmap = m_Screen->grabWindow(0, rectAroundMouse.x(), rectAroundMouse.y(), rectAroundMouse.width(), rectAroundMouse.height());
    QImage rectAroundTheCursorImage = rectAroundTheCursorPixmap.toImage();
    scaleIfZoomFactorWarrantsIt(&rectAroundTheCursorImage);
    emit mouseOrMotionRectGrabbed(newMousePos, rectAroundTheCursorImage, Mouse);

    //QImage screenGrab = m_Screen.grab(); //TO DOneoptimization(oh duh it does, screen motion will use tha- wait no it won't xD): I think grab() accepts a rect actually...
    //grabRectAroundPointOnImageAndEmitIt(newMousePos, screenGrab, Mouse);
}
void MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::handleMotionOnScreenDetected(const QPoint &pointOnImageAroundWhichToGrabRect, const QImage &imageToGrabRectFrom)
{
    QRect rectAroundTheScreenMovement = makeRectAroundPointStayingWithinResolution(pointOnImageAroundWhichToGrabRect);
    QImage rectAroundTheScreenMovementImage = imageToGrabRectFrom.copy(rectAroundTheScreenMovement);
    scaleIfZoomFactorWarrantsIt(&rectAroundTheScreenMovementImage);
    emit mouseOrMotionRectGrabbed(pointOnImageAroundWhichToGrabRect, rectAroundTheScreenMovementImage, Motion);
}
