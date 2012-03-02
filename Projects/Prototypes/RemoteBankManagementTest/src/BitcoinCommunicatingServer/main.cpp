#include <QtGui/QApplication>
#include "debugserverview.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    debugServerView w;
    w.show();

    return a.exec();
}
