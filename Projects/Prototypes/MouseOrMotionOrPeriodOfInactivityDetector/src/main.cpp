#include <QApplication>

#include "mouseormotionorperiodofinactivitydetector.h"
#include "mouseormotionorperiodofinactivitydetectordebugoutput.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MouseOrMotionOrPeriodOfInactivityDetector detector;
    MouseOrMotionOrPeriodOfInactivityDetectorDebugOutput debugOutput;
    QObject::connect(&detector, SIGNAL(mouseMovementDetected(QPoint)), &debugOutput, SLOT(handleMouseMovementDetected(QPoint)));
    QObject::connect(&detector, SIGNAL(motionOnScreenDetected(QPoint,QImage)), &debugOutput, SLOT(handleScreenMovementDetected(QPoint,QImage)));
    QObject::connect(&detector, SIGNAL(periodOfInactivityDetected()), &debugOutput, SLOT(handlePeriodOfInactivityDetected()));
    debugOutput.show();
    detector.startDetectingMouseOrMotionOrPeriodsOfInactivity(1000);

    return a.exec();
}
