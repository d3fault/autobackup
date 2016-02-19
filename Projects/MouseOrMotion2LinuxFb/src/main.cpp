#include <QApplication>

#include "mouseormotion2linuxfb.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MouseOrMotion2LinuxFb m;
    Q_UNUSED(m)

    return a.exec();
}
