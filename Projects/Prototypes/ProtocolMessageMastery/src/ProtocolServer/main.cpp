#include <QtGui/QApplication>
#include "mainserverwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mainServerWidget w;
    w.show();

    return a.exec();
}
