#include <QtGui/QApplication>
#include "layoutappendupdatesgui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LayoutAppendUpdatesGui w;
    w.show();

    return a.exec();
}
