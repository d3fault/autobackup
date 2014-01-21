#include <QApplication>

#include "addoublesrawmathlaunchsimgui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    AdDoublesRawMathLaunchSimGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
