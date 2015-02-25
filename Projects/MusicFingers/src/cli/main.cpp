#include <QCoreApplication>

#include "musicfingerscli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MusicFingersCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
