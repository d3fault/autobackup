#include <QCoreApplication>

#include "atomicallyupdatesymlinktolatestgitcommitcli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    AtomicallyUpdateSymlinkToLatestGitCommitCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
