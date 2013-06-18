#include <QtGui/QApplication>
#include "gitsubmoduleautobackuphalper.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GitSubmoduleAutoBackupHalper halper;
    Q_UNUSED(halper)
    
    return a.exec();
}
