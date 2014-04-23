#include "whenstacktracefailswidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    WhenStackTraceFailsWidget w;
    w.show();

    return a.exec();
}
