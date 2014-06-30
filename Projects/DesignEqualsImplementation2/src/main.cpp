#include <QApplication>

#include "designequalsimplementationgui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DesignEqualsImplementationGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
