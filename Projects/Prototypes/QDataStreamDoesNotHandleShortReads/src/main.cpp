#include <QtGui/QApplication>

#include "datastreamdoesnothandleshortreadsbackend.h"
#include "mainwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DataStreamDoesNotHandleShortReadsBackend backend;

    mainWidget w(&backend);
    w.show();

    return a.exec();
}
