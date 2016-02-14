#ifndef MOUSEORMOTIONORPERIODOFINACTIVITYDETECTOR_H
#define MOUSEORMOTIONORPERIODOFINACTIVITYDETECTOR_H

#include <QObject>

#include <QSize>
#include <QPoint>
#include <QImage>

class QScreen;

class MouseOrMotionOrPeriodOfInactivityDetector : public QObject
{
    Q_OBJECT
public:
    explicit MouseOrMotionOrPeriodOfInactivityDetector(QObject *parent = 0);
private:
    QScreen *m_Screen;
    QSize m_ScreenResolution;
    QPoint m_CurrentMousePos;
    QImage m_OldScreenGrab;
    QPoint m_PointOnScreenWhereMotionWasDetected;
    QImage m_QtCreatorBlinkingCursorToExcludeFromMotionDetectionChecks;

    bool mouseMovedSinceLastPoll();
    bool thereWasMotionOnScreenSinceLastPoll();
    bool thereHasBeenAperiodOfInactivity() const;
    bool newScreenGrabMatchesOldScreenGrab_andBtwNoteThePointWhereTheyDontMatchPlox(const QImage &newScreenGrab);
    bool thereIsEnoughRoomToDrawQtCreatorBlinkingCursorInOrderToExcludeItFromFutureSearching(int verticalPositionOfMotionPoint, int horizontalPositionOfMotionPoint, int imageWidth) const;
signals:
    void mouseMovementDetected(const QPoint &newMousePos);
    void motionOnScreenDetected(const QPoint &pointOnScreenWhereMotionWasDetected, const QImage &screenGrabImage);
    void periodOfInactivityDetected();
public slots:
    void startDetectingMouseOrMotionOrPeriodsOfInactivity(int pollRateMSec = 100, int amountOfTimeMSecWithNoMouseOrMotionActivityToBeConsideredAPeriodOfInactivity = 2000);
private slots:
    void handlePollTimerTimedOut();
};

#endif // MOUSEORMOTIONORPERIODOFINACTIVITYDETECTOR_H
