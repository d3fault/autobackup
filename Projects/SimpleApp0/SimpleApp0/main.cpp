#include "simplewidget0.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SimpleWidget0 w;
    w.show();

    return a.exec();
}
