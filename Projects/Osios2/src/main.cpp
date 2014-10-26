#include <QApplication>
#include <QSettings>

#include "itimelinenode.h"
#include "osiosgui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Osios");
    QCoreApplication::setOrganizationDomain("OsiosDomain");
    QCoreApplication::setApplicationName("Osios");
    QSettings::setDefaultFormat(QSettings::IniFormat); //because we use the dir selected as our default data dir in profile create dialog

    OsiosGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
