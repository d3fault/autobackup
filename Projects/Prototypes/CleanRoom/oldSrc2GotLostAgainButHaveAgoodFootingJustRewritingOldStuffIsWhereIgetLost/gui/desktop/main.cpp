#include <QApplication>

#include "cleanroomgui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    CleanRoomGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
