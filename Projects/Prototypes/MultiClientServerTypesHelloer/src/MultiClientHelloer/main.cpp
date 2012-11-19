#include <QtGui/QApplication>

#include "multiclienthelloertest.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MultiClientHelloerTest test;
    Q_UNUSED(test)
    
    return a.exec();
}
