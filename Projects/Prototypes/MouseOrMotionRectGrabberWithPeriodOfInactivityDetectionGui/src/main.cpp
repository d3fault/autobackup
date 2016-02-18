#include <QApplication>

#include "mouseormotionrectgrabberwithperiodofinactivitydetectiongui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MouseOrMotionRectGrabberWithPeriodOfInactivityDetectionGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
