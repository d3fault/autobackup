#include <QCoreApplication>

#include "quickdirtystaticgitwebcli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QuickDirtyStaticGitWebCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
