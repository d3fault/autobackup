#include <QApplication>

#include "recursivevideolengthandsizecalculatorgui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    RecursiveVideoLengthAndSizeCalculatorGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
