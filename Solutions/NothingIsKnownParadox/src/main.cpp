#include <QCoreApplication>

#include "nothingisknownparadoxcli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    NothingIsKnownParadoxCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
