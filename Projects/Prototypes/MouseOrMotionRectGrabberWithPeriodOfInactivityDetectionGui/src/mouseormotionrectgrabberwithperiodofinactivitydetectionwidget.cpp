#include "mouseormotionrectgrabberwithperiodofinactivitydetectionwidget.h"

#include <QDebug>

MouseOrMotionRectGrabberWithPeriodOfInactivityDetectionWidget::MouseOrMotionRectGrabberWithPeriodOfInactivityDetectionWidget(QWidget *parent)
    : QLabel(parent)
{ }
void MouseOrMotionRectGrabberWithPeriodOfInactivityDetectionWidget::handleMotionOrMotionRectGrabbed(const QPoint &positionOnDesktopWhereMouseOrMotionOccured, const QVariant &rectAroundMouseOrMotion, MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::MouseOrMotionEnum mouseOrMotion)
{
    qDebug() << "Mouse or motion at point:" << positionOnDesktopWhereMouseOrMotionOccured;
    qDebug() << "Mouse or motion?" << mouseOrMotion;
    setPixmap(rectAroundMouseOrMotion.value<QPixmap>());
}
void MouseOrMotionRectGrabberWithPeriodOfInactivityDetectionWidget::handlePeriodOfInactivityDetected()
{
    qDebug() << "Period of inactivity detected";
    clear();
    setText(tr("Period of inactivity"));
}
