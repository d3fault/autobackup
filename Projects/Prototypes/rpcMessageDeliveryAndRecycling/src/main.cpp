#include <QtGui/QApplication>
#include "mainwidget.h"

TODO LEFT OFF -- //shit appears to be working, but need to add debug output to verify message recycling is ACTUALLY working. also maybe ensure/verify(? (no thread checking easily in Qt :() that the broadcast dispensers are moved to the right threads?

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    mainWidget w;
    w.show();

    return a.exec();
}
