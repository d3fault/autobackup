#include <QApplication>

#include "drummachinegui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DrumMachineGui w;
    w.show();

    return a.exec();
}
