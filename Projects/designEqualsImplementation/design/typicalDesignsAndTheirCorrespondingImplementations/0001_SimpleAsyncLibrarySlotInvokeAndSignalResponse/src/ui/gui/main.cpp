#include <QApplication>

#include "simpleasynclibraryslotinvokeandsignalresponsegui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SimpleAsyncLibrarySlotInvokeAndSignalResponseGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
