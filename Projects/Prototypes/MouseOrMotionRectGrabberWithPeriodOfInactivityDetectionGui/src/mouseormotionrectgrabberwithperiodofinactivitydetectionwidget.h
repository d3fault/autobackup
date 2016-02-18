#ifndef MOUSEORMOTIONRECTGRABBERWITHPERIODOFINACTIVITYDETECTIONWIDGET_H
#define MOUSEORMOTIONRECTGRABBERWITHPERIODOFINACTIVITYDETECTIONWIDGET_H

#include <QWidget>

#include "mouseormotionrectgrabberwithperiodofinactivitydetection.h"

class MouseOrMotionRectGrabberWithPeriodOfInactivityDetectionWidget : public QWidget
{
    Q_OBJECT
public:
    MouseOrMotionRectGrabberWithPeriodOfInactivityDetectionWidget(QWidget *parent = 0);
public slots:
    void handleMotionOrMotionRectGrabbed(const QPoint &positionOnDesktopWhereMouseOrMotionOccured, const QImage &rectAroundMouseOrMotion, MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::MouseOrMotionEnum mouseOrMotion);
};

#endif // MOUSEORMOTIONRECTGRABBERWITHPERIODOFINACTIVITYDETECTIONWIDGET_H
