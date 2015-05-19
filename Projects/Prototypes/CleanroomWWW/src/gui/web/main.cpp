#include <QCoreApplication>

#include "cleanroomwww.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    CleanRoomWWW gui;
    Q_UNUSED(gui)

    return a.exec();
}
