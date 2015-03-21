#include <QCoreApplication>

#include "directoriesofaudioandvideofilesmuxersyncercli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DirectoriesOfAudioAndVideoFilesMuxerSyncerCli cli;
    //QObject::connect(&cli, &DirectoriesOfAudioAndVideoFilesMuxerSyncerCli::exitRequested, qApp, &QCoreApplication::exit, Qt::QueuedConnection);
    Q_UNUSED(cli)

    return a.exec();
}
