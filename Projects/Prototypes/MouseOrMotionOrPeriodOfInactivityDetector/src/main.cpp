#include <QApplication>

#include "mouseormotionorperiodofinactivitydetector.h"
#include "mouseormotionorperiodofinactivitydetectordebugoutput.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //TODOreq: MouseOrMotionOrPeriodOfInactivityDetector should go on it's own/separate thread, since checking for motion is expensive and blocks the GUI. I didn't notice it on this 1fps monitor though rofl ;-P
    MouseOrMotionOrPeriodOfInactivityDetector detector;
    MouseOrMotionOrPeriodOfInactivityDetectorDebugOutput debugOutput;
    QObject::connect(&detector, SIGNAL(mouseMovementDetected(QPoint)), &debugOutput, SLOT(handleMouseMovementDetected(QPoint)));
    QObject::connect(&detector, SIGNAL(motionOnScreenDetected(QPoint,QImage)), &debugOutput, SLOT(handleScreenMovementDetected(QPoint,QImage)));
    QObject::connect(&detector, SIGNAL(periodOfInactivityDetected()), &debugOutput, SLOT(handlePeriodOfInactivityDetected()));
    debugOutput.show();
    detector.startDetectingMouseOrMotionOrPeriodsOfInactivity(1000);

    return a.exec();
}
