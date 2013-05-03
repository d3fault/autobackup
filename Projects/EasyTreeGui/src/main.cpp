#include <QtGui/QApplication>

#include "easytreegui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    EasyTreeGui gui;
    Q_UNUSED(gui)
    
    return a.exec();
}
