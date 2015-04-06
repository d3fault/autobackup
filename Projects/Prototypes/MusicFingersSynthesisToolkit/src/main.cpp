#include <QCoreApplication>

#include "musicfingerssynthesistoolkitcli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MusicFingersSynthesisToolkitCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
