#ifndef MOUSEORMOTIONRECTGRABBERWITHPERIODOFINACTIVITYDETECTION_H
#define MOUSEORMOTIONRECTGRABBERWITHPERIODOFINACTIVITYDETECTION_H

#include <QObject>

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
    QSize m_Rect;
    QScreen *m_Screen;
signals:
    void mouseOrMotionRectGrabbed(const QPoint &positionOnDesktopWhereMouseOrMotionOccured, const QImage &rectAroundMouseOrMotion, MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::MouseOrMotionEnum mouseOrMotion);
    void periodOfInactivityDetected();
public slots:
    void startGrabbingMouseOrMotionRectsWhileAlsoDetectingPeriodsOfInactivity(const QSize &rectAroundMouseOrMotionToGrab, int pollRateMSec = 100, int amountOfTimeMSecWithNoMouseOrMotionActivityToBeConsideredAPeriodOfInactivity = 2000);
private slots:
    void handleMouseMovementDetected(const QPoint &newMousePos);
    //void grabRectAroundPointOnImageAndEmitIt(const QPoint &pointOnImageAroundWhichToGrabRect, const QImage &imageToGrabRectFrom, MouseOrMotionEnum mouseOrMotion = Motion);
    void handleMotionOnScreenDetected(const QPoint &pointOnImageAroundWhichToGrabRect, const QImage &imageToGrabRectFrom);
};

#endif // MOUSEORMOTIONRECTGRABBERWITHPERIODOFINACTIVITYDETECTION_H
