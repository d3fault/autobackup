#ifndef MOUSEORMOTIONORPERIODOFINACTIVITYDETECTORDEBUGOUTPUT_H
#define MOUSEORMOTIONORPERIODOFINACTIVITYDETECTORDEBUGOUTPUT_H

#include <QPlainTextEdit>

class MouseOrMotionOrPeriodOfInactivityDetectorDebugOutput : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit MouseOrMotionOrPeriodOfInactivityDetectorDebugOutput(QWidget *parent = 0);
public slots:
    void handleMouseMovementDetected(const QPoint &mousePos);
    void handleScreenMovementDetected(const QPoint &positionOnScreen, const QImage &theScreenGrab);
    void handlePeriodOfInactivityDetected();
};

#endif // MOUSEORMOTIONORPERIODOFINACTIVITYDETECTORDEBUGOUTPUT_H
