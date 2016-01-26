#include <QApplication>

#include "comboboxwithtwodifferentcompletertypesdependingonwheretextcursoris.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ComboBoxWithTwoDifferentCompleterTypesDependingOnWhereTextCursorIs w;
    w.show();

    return a.exec();
}
