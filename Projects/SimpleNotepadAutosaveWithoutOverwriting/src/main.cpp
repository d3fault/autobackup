#include "simplenotepadautosavewithoutoverwritingwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SimpleNotepadAutosaveWithoutOverwritingWidget w;
    w.show();

    return a.exec();
}
