#include <QApplication>

#include "runtimedynamiciterationsdeterminerretainingresponsivenessgui.h"

//tl;dr: maximize cpu utilization while keeping a 'backend' [thread] responsive within X milliseconds
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    RuntimeDynamicIterationsDeterminerRetainingResponsivenessGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
