#include <QApplication>
#include "lossydirectorycopiergui.h"

//inb4 this entire app could be a "find" oneliner :) -- FUCK BASH

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LossyDirectoryCopierGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
