#include "stevenhereandnowgui.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    StevenHereAndNowGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
