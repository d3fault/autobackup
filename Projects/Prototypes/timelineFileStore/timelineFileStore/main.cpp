#include <QtGui/QApplication>
#include "testtimelinefilestorewidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    testTimelineFileStoreWidget w;
    w.show();

    return a.exec();
}
