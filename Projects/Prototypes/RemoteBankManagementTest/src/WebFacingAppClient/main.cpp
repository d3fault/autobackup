#include <QtGui/QApplication>
#include "mainwebsitebankview.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mainWebsiteBankView w;
    w.show();

    return a.exec();
}
