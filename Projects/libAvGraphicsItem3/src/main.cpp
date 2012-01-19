#include <QtGui/QApplication>
#include "frontend/libavgraphicsitem3test.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    libAvGraphicsItem3test w;
    w.show();

    return a.exec();
}
