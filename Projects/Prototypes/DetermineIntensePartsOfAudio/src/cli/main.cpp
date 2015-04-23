#include <QCoreApplication>

#include "determineintensepartsofaudiocli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DetermineIntensePartsOfAudioCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
