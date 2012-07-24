#include <QtGui/QApplication>
#include "qfsw2testwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFsw2TestWidget w;
    w.show();

    return a.exec();
}
