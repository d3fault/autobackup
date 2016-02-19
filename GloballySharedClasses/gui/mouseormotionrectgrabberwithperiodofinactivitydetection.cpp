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
{
    qRegisterMetaType<QVariant::Type>("QVariant::Type"); //muahaha register it the QVariant::Type as a QVariant::Type muahahaha. another perpetual motion machine! or perhaps an infinite loop? methinks it'll work just fine actually. hmm not with just Q_DECLARE_METATYPE, but this seems to work although it's shittier :(. where does one put the static initialization of the nothingness of a class?
    qRegisterMetaType<MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::MouseOrMotionEnum>("MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::MouseOrMotionEnum");
}
bool MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::rectGrabImageTypeIsSupported(const QVariant::Type &type)
{
    return (type == QVariant::Pixmap || type == QVariant::Image); //bleh why do all this whacky premature optimization conversion logic for the end user when it's so simple to call eg .toBitmap() on the result? fuggit
    //return MouseOrMotionOrPeriodOfInactivityDetector::screenGrabImageTypeIsSupported(type);
}
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
void MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::startGrabbingMouseOrMotionRectsWhileAlsoDetectingPeriodsOfInactivity(const QSize &rectSizeAroundMouseOrMotionToGrab, int pollRateMSec, int amountOfTimeMSecWithNoMouseOrMotionActivityToBeConsideredAPeriodOfInactivity, double zoomFactor, QVariant::Type requestedImageType)
{
    m_RectSize = rectSizeAroundMouseOrMotionToGrab;
    m_ZoomFactor = zoomFactor;
    m_RequestedImageType = requestedImageType;

    if(!rectGrabImageTypeIsSupported(m_RequestedImageType))
    {
        //if(MouseOrMotionOrPeriodOfInactivityDetector::screenGrabImageTypeIsSupported(type))
        //{
            //derp use image type for detection then convert and blah blah fuck that shit
        //}
        QString blah("Rect grab image type not supported: ");
        blah.append(QVariant::typeToName(m_RequestedImageType));
        std::string blahStd = blah.toStdString();
        qFatal(blahStd.c_str());
        return;
    }

    MouseOrMotionOrPeriodOfInactivityDetector *detector = new MouseOrMotionOrPeriodOfInactivityDetector(this);
    connect(detector, SIGNAL(mouseMovementDetected(QPoint)), this, SLOT(handleMouseMovementDetected(QPoint)));
    connect(detector, SIGNAL(motionOnScreenDetected(QPoint,QVariant)), this, SLOT(handleMotionOnScreenDetected(QPoint,QVariant)));
    connect(detector, SIGNAL(periodOfInactivityDetected()), this, SIGNAL(periodOfInactivityDetected()));

    detector->startDetectingMouseOrMotionOrPeriodsOfInactivity(pollRateMSec, amountOfTimeMSecWithNoMouseOrMotionActivityToBeConsideredAPeriodOfInactivity, requestedImageType);
}
void MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::handleMouseMovementDetected(const QPoint &newMousePos)
{
    QRect rectAroundMouse = makeRectAroundPointStayingWithinResolution(newMousePos);
    QPixmap rectAroundTheCursorPixmap = m_Screen->grabWindow(0, rectAroundMouse.x(), rectAroundMouse.y(), rectAroundMouse.width(), rectAroundMouse.height());
    QPixmap rectAroundTheCursorPixmapZoomedMaybe = scaleIfZoomFactorWarrantsIt<QPixmap>(rectAroundTheCursorPixmap); //TODOoptimization: might be faster to convert to QImage (if that's m_RequestedImageType) before calling scaleIfZoomFactorWarrantsIt
    QVariant imageRect;
    switch(m_RequestedImageType)
    {
    case QVariant::Pixmap:
        imageRect = QVariant::fromValue(rectAroundTheCursorPixmapZoomedMaybe);
        break;
    case QVariant::Image:
        imageRect = QVariant::fromValue(rectAroundTheCursorPixmapZoomedMaybe.toImage()); //our 'default' case could have done this (when I wrote this comment, I was planning on trying to convert), but it saves us from having to check canConvert/convert etc. we know it converts.
        break;
    default:
        imageRect = QVariant(QVariant::Invalid); //TODoreq: should we _TRY_ to convert? it would work for eg QBitmap. see the 'default' case comment in handleMotionOnScreenDetected, as it's directly related
        break;
    }
    emit mouseOrMotionRectGrabbed(newMousePos, imageRect, Mouse);

    //QImage rectAroundTheCursorImage = rectAroundTheCursorPixmap.toImage();
    //scaleIfZoomFactorWarrantsIt(&rectAroundTheCursorImage);
    //emit mouseOrMotionRectGrabbed(newMousePos, rectAroundTheCursorImage, Mouse);
    //convertIfneededToRequestedImageTypeThenEmit(newMousePos, rectAroundTheCursorPixmap, Mouse);

    //QImage screenGrab = m_Screen.grab(); //TO DOneoptimization(oh duh it does, screen motion will use tha- wait no it won't xD): I think grab() accepts a rect actually...
    //grabRectAroundPointOnImageAndEmitIt(newMousePos, screenGrab, Mouse);
}
void MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::handleMotionOnScreenDetected(const QPoint &pointOnImageAroundWhichToGrabRect, const QVariant &imageToGrabRectFrom)
{
    QRect rectAroundTheScreenMovement = makeRectAroundPointStayingWithinResolution(pointOnImageAroundWhichToGrabRect);
    QVariant imageRect;
    switch(m_RequestedImageType)
    {
    case QVariant::Pixmap:
        imageRect = copyAndScaleIfZoomFactorWarrantsIt<QPixmap>(imageToGrabRectFrom, rectAroundTheScreenMovement);
        break;
    case QVariant::Image:
        imageRect = copyAndScaleIfZoomFactorWarrantsIt<QImage>(imageToGrabRectFrom, rectAroundTheScreenMovement);
        break;
    default:
        //TODOmb: maybe the requested type doesn't have ::copy and ::scaled (QBitmap), in which case we can convert it to an image, copy/scale it, then convert it back to a QBitmap xD. We even could have pipleined this optimization by giving QImage (not QPixmap since it [probably] copies/scales slower) to MouseOrMotionOrPeriodOfInactivityDetector as IT'S requested image type
        imageRect = QVariant(QVariant::Invalid);
        break;
    }
    emit mouseOrMotionRectGrabbed(pointOnImageAroundWhichToGrabRect, imageRect, Motion);
#if 0
    QImage rectAroundTheScreenMovementImage = imageToGrabRectFrom.copy(rectAroundTheScreenMovement);
    //scaleIfZoomFactorWarrantsIt(&rectAroundTheScreenMovementImage);
    //emit mouseOrMotionRectGrabbed(pointOnImageAroundWhichToGrabRect, rectAroundTheScreenMovementImage, Motion);
    convertIfneededToRequestedImageTypeThenEmit(pointOnImageAroundWhichToGrabRect, rectAroundTheScreenMovementImage, Motion);
#endif
}
template<class T>
QVariant MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::copyAndScaleIfZoomFactorWarrantsIt(const QVariant &imageToCopyFromAndZoomInOn, const QRect &rectToCopy)
{
    //T must have ::copy and ::scaled
    T image = imageToCopyFromAndZoomInOn.value<T>();
    T temp = image.copy(rectToCopy);
    T tempZoomedMaybe = scaleIfZoomFactorWarrantsIt<T>(temp);
    QVariant ret = QVariant::fromValue(tempZoomedMaybe);
    return ret;
}
