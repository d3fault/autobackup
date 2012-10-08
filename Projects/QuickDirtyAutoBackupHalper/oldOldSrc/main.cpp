#include <QtGui/QApplication>

#include "quickdirtyautobackuphalper.h"
#include "mainwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QuickDirtyAutoBackupHalper halper;

    //TODOreq: detect a SIGTERM (on shutdown) and release resources and stop business thread etc. Funny that we'll be the actual source of the sigterm :-P (detached poweroff)
    mainWidget w(&halper);
    w.show();
    
    return a.exec();
}
