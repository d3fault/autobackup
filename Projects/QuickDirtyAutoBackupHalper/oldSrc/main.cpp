#include <QtGui/QApplication>
#include <QSettings>

#include "quickdirtyautobackuphalper.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("OrganizationOfQuickDirtyAutoBackups");
    QCoreApplication::setOrganizationDomain("DomainOfOrganizationOfQuickDirtyAutoBackups.com");
    QCoreApplication::setApplicationName("QuickDirtyAutoBackupHalper");

    QuickDirtyAutoBackupHalper halper;
    halper.start();

    ---breaking this one too because i'm overcomplicating the FUCK out of this. my background thread does not need to be asynchronous. going to use QProcess's synchronous functionality instead to greatly simplify shit
    
    return a.exec();
}
