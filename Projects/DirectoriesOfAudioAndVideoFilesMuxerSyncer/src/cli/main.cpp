#include <QCoreApplication>
#include <QStringList>

#include "directoriesofaudioandvideofilesmuxersyncercli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DirectoriesOfAudioAndVideoFilesMuxerSyncerCli cli(a.arguments());
    //QObject::connect(&cli, &DirectoriesOfAudioAndVideoFilesMuxerSyncerCli::exitRequested, qApp, &QCoreApplication::exit, Qt::QueuedConnection);
    Q_UNUSED(cli)

    return a.exec();
}
