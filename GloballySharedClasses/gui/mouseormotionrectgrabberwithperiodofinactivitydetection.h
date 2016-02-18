#ifndef MOUSEORMOTIONRECTGRABBERWITHPERIODOFINACTIVITYDETECTION_H
#define MOUSEORMOTIONRECTGRABBERWITHPERIODOFINACTIVITYDETECTION_H

#include <QObject>

#include <QSize>

class QScreen;

class MouseOrMotionRectGrabberWithPeriodOfInactivityDetection : public QObject
{
    Q_OBJECT
    Q_ENUMS(MouseOrMotionEnum)
public:
    explicit MouseOrMotionRectGrabberWithPeriodOfInactivityDetection(QObject *parent = 0);
    enum MouseOrMotionEnum
    {
         Mouse = 0
        , Motion = 1
    };
private:
    QScreen *m_Screen;
    QSize m_ScreenResolution;
    QSize m_RectSize;
    double m_ZoomFactor;

    QRect makeRectAroundPointStayingWithinResolution(const QPoint &inputPoint);
    void scaleIfZoomFactorWarrantsIt(QImage *imageToMaybeScale);
signals:
    void mouseOrMotionRectGrabbed(const QPoint &positionOnDesktopWhereMouseOrMotionOccured, const QImage &rectAroundMouseOrMotion, MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::MouseOrMotionEnum mouseOrMotion);
    void periodOfInactivityDetected();
public slots:
    void startGrabbingMouseOrMotionRectsWhileAlsoDetectingPeriodsOfInactivity(const QSize &rectSizeAroundMouseOrMotionToGrab, int pollRateMSec = 100, int amountOfTimeMSecWithNoMouseOrMotionActivityToBeConsideredAPeriodOfInactivity = 2000, double zoomFactor = 1.0);
private slots:
    void handleMouseMovementDetected(const QPoint &newMousePos);
    //void grabRectAroundPointOnImageAndEmitIt(const QPoint &pointOnImageAroundWhichToGrabRect, const QImage &imageToGrabRectFrom, MouseOrMotionEnum mouseOrMotion = Motion);
    void handleMotionOnScreenDetected(const QPoint &pointOnImageAroundWhichToGrabRect, const QImage &imageToGrabRectFrom);
};

#endif // MOUSEORMOTIONRECTGRABBERWITHPERIODOFINACTIVITYDETECTION_H
