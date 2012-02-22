#include <QtGui/QApplication>
#include "mainclientwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mainClientWidget w;
    w.show();

    return a.exec();
}
