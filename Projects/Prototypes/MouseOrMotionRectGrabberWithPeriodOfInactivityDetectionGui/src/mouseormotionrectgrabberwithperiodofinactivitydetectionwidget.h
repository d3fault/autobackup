#ifndef MOUSEORMOTIONRECTGRABBERWITHPERIODOFINACTIVITYDETECTIONWIDGET_H
#define MOUSEORMOTIONRECTGRABBERWITHPERIODOFINACTIVITYDETECTIONWIDGET_H

#include <QLabel>

#include "mouseormotionrectgrabberwithperiodofinactivitydetection.h"

class MouseOrMotionRectGrabberWithPeriodOfInactivityDetectionWidget : public QLabel
{
    Q_OBJECT
public:
    MouseOrMotionRectGrabberWithPeriodOfInactivityDetectionWidget(QWidget *parent = 0);
public slots:
    void handleMotionOrMotionRectGrabbed(const QPoint &positionOnDesktopWhereMouseOrMotionOccured, const QVariant &rectAroundMouseOrMotion, MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::MouseOrMotionEnum mouseOrMotion);
    void handlePeriodOfInactivityDetected();
};

#endif // MOUSEORMOTIONRECTGRABBERWITHPERIODOFINACTIVITYDETECTIONWIDGET_H
