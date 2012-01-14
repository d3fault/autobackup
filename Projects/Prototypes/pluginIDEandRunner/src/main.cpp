#include <QtGui/QApplication>
#include "pluginideandrunnermainwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    pluginIDEandRunnerMainWidget w;
    w.show();

    return a.exec();
}
