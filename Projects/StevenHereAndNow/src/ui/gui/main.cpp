#include "stevenhereandnowgui.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //ILawsOfPhysics *the = new Existence();

    StevenHereAndNowGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
