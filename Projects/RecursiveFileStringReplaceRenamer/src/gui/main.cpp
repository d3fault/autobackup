#include <QApplication>

#include "recursivefilestringreplacerenamergui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    RecursiveFileStringReplaceRenamerGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
