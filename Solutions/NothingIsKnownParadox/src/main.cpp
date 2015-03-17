#include <QCoreApplication>

#include "nothingisknownparadoxcli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    NothingIsKnownParadoxCli cli;
    QObject::connect(&cli, SIGNAL(quitRequested()), qApp, SLOT(quit()), Qt::QueuedConnection);

    return a.exec();
}
