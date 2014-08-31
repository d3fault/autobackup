#include <QApplication>

#include "copyfilecontentstoclipboardwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    CopyFileContentsToClipboardWidget w;
    w.show();

    return a.exec();
}
