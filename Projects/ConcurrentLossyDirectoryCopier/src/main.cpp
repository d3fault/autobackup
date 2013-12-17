#include <QApplication>
#include "concurrentlossydirectorycopiergui.h"

//invoke redirecting stderr to stdout: 2>&1
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ConcurrentLossyDirectoryCopierGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
