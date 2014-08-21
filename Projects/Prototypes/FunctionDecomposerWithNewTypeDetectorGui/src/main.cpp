#include <QApplication>

#include "functiondecomposerwithnewtypedetectorgui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    FunctionDecomposerWithNewTypeDetectorGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
