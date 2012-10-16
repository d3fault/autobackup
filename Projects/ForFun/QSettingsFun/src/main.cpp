#include <QtGui/QApplication>

#include "mainwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("OrganizationSettingsLoL");
    QCoreApplication::setOrganizationDomain("OrganizationDomainLoL.lol");
    QCoreApplication::setApplicationName("QSettingsFunApplicationNameSup");

    mainWidget w;
    w.show();
    
    return a.exec();
}
