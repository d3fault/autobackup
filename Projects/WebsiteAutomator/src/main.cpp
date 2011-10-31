#include <QtGui/QApplication>
#include "selectwebsiteandthensiteactionwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    selectWebsiteAndThenSiteActionWidget w;
    w.show();

    return a.exec();
}
