#include <QApplication>

#include "musicfingersbackendtestergui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MusicFingersBackendTesterGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
