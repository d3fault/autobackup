#include <QCoreApplication>

#include "ffmpegsegmentuploadercli.h"

//TODOoptimization: libtorrent
//TODOoptional: only if segment upload is successful, queue for delete maybe 24 hours later? these aren't the master copies, but maybe needed if a server crashes before propagation. deleting after 24 hours will keep the hdd from filling up, which would require manual intervention
//^^perhaps as a cli flag, omitted = no delete. could even make this a runtime changeable param via menu shits (been a while since i read an int value from stdin)

//example command: ./FfmpegSegmentUploaderCli /run/shm/ffmpegTemp /run/shm/incomingFromUploader/uploadScratch /run/shm/incomingFromUploader/watchedFolderToMoveTo user@192.168.57.10 15     ----- ffmpegTemp need not exist, but the two folders on remote destination must
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    FfmpegSegmentUploaderCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
