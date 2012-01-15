#include <QtGui/QApplication>
#include "audiovideobufferswidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    AudioVideoBuffersWidget w;
    w.show();

    return a.exec();
}
