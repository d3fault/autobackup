#include <QApplication>

#include "osiosgui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Osios Org");
    QCoreApplication::setOrganizationDomain("osiosDomain");
    QCoreApplication::setApplicationName("Osios");

    OsiosGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
