#include <QtGui/QApplication>

#include "easytreetest.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    EasyTreeTest test;
    test.start();
    
    return a.exec();
}
