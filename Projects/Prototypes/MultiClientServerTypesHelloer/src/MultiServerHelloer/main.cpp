#include <QtGui/QApplication>

#include "multiserverhelloertest.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MultiServerHelloerTest test;
    Q_UNUSED(test)
    
    return a.exec();
}
