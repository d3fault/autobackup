#include <QApplication>

#include "icepickgui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    IcePickGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
