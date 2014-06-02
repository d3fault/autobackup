#include <QApplication>
#include <QThread>

#include "quickdirtyautobackuphalper.h"

//TODOreq: remove truecrypt dependency (checks for bin existence at startup and refuses to go forward). bleh probably rewriting this whole thing eventually anyways...
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("QuickDirtyAutoBackupHalperOrganization");
    QCoreApplication::setOrganizationDomain("QuickDirtyAutoBackupHalperOrganization.com");
    QCoreApplication::setApplicationName("QuickDirtyAutoBackupHalper");

#ifndef Q_OS_WIN32
    QuickDirtyAutoBackupHalper business;
    business.start();

    return a.exec();
#else
    return -1; //windows not supported for now...
#endif
}
