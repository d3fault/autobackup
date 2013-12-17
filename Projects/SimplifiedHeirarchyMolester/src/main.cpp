#include <QApplication>

#include "simplifiedheirarchymolestergui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SimplifiedHeirarchyMolesterGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
