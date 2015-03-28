#include <QCoreApplication>

#include "watchsigsfileandpostchangestousenetcli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    WatchSigsFileAndPostChangesToUsenetCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
