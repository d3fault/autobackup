#ifndef MOUSEORMOTIONRECTGRABBERWITHPERIODOFINACTIVITYDETECTION_H
#define MOUSEORMOTIONRECTGRABBERWITHPERIODOFINACTIVITYDETECTION_H

#include <QObject>

#include <QSize>
#include <QVariant>

class QScreen;

class MouseOrMotionRectGrabberWithPeriodOfInactivityDetection : public QObject
{
    Q_OBJECT
    Q_ENUMS(MouseOrMotionEnum)
public:
    enum MouseOrMotionEnum
    {
         Mouse = 0
        , Motion = 1
    };
    explicit MouseOrMotionRectGrabberWithPeriodOfInactivityDetection(QObject *parent = 0);
    static bool rectGrabImageTypeIsSupported(const QVariant::Type &type);
private:
    QScreen *m_Screen;
    QSize m_ScreenResolution;
    QSize m_RectSize;
    double m_ZoomFactor;
    QVariant::Type m_RequestedImageType;

    QRect makeRectAroundPointStayingWithinResolution(const QPoint &inputPoint);

    template<class T>
    T scaleIfZoomFactorWarrantsIt(T imageToMaybeScale)
    {
        if(m_ZoomFactor != 1.0) //TODOoptimization: only check != 1.0 once, in the start slot, then rely on a bool here
            return imageToMaybeScale.scaled(m_RectSize);
        return imageToMaybeScale;
    }

    template<class T>
    QVariant copyAndScaleIfZoomFactorWarrantsIt(const QVariant &imageToCopyFromAndZoomInOn, const QRect &rectToCopy);
signals:
    void mouseOrMotionRectGrabbed(const QPoint &positionOnDesktopWhereMouseOrMotionOccured, const QVariant &rectAroundMouseOrMotion, MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::MouseOrMotionEnum mouseOrMotion);
    void periodOfInactivityDetected();
public slots:
    void startGrabbingMouseOrMotionRectsWhileAlsoDetectingPeriodsOfInactivity(const QSize &rectSizeAroundMouseOrMotionToGrab, int pollRateMSec = 100, int amountOfTimeMSecWithNoMouseOrMotionActivityToBeConsideredAPeriodOfInactivity = 2000, double zoomFactor = 1.0, QVariant::Type requestedImageType = QVariant::Pixmap);
private slots:
    void handleMouseMovementDetected(const QPoint &newMousePos);
    void handleMotionOnScreenDetected(const QPoint &pointOnImageAroundWhichToGrabRect, const QVariant &imageToGrabRectFrom);
};

#endif // MOUSEORMOTIONRECTGRABBERWITHPERIODOFINACTIVITYDETECTION_H
