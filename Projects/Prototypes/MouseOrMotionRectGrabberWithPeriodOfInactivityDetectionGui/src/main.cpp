#include <QApplication>

#include "mouseormotionrectgrabberwithperiodofinactivitydetection.h"
#include "mouseormotionrectgrabberwithperiodofinactivitydetectionwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MouseOrMotionRectGrabberWithPeriodOfInactivityDetection grabber;
    MouseOrMotionRectGrabberWithPeriodOfInactivityDetectionWidget w;
    connect(&grabber, SIGNAL(mouseOrMotionRectGrabbed(QPoint,QImage,MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::MouseOrMotionEnum)), &w, SLOT(handleMotionOrMotionRectGrabbed(QPoint,QImage,MouseOrMotionRectGrabberWithPeriodOfInactivityDetection::MouseOrMotionEnum)));
    connect(&grabber, SIGNAL(periodOfInactivityDetected()), &w, SLOT(handlePeriodOfInactivityDetected()));
    w.show();
    QSize size(160, 128); //TODOreq: don't grab 'out of bounds'
    grabber.startGrabbingMouseOrMotionRectsWhileAlsoDetectingPeriodsOfInactivity(size, 2000, 10000);

    return a.exec();
}
