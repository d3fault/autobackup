#include <QApplication>

#include "bulkfiletextprependergui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    BulkFileTextPrependerGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
