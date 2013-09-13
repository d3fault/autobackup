#include <QtGui/QApplication>
#include "easytreetotalsizecalculatorgui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    EasyTreeTotalSizeCalculatorGui gui;
    Q_UNUSED(gui)
    
    return a.exec();
}
