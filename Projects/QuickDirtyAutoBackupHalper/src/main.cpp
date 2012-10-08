#include <QtGui/QApplication>
#include <QThread>

#include "quickdirtyautobackuphalper.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("QuickDirtyAutoBackupHalperOrganization");
    QCoreApplication::setOrganizationDomain("QuickDirtyAutoBackupHalperOrganization.com");
    QCoreApplication::setApplicationName("QuickDirtyAutoBackupHalper");

#ifndef Q_OS_WIN32 //does this include 64 bit?
    QuickDirtyAutoBackupHalper business;
    business.start();

    return a.exec();
#else
    return -1; //windows not supported for now...
#endif
}
