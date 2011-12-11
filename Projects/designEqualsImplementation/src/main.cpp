#include <QtGui/QApplication>
#include "Gui/mainwindow.h"

#include "StateMachine/modesingleton.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
