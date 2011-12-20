#include <QtGui/QApplication>

#include "frontend/libavplayerwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    libAvPlayerWidget w;
    w.show();

    return a.exec();
}
