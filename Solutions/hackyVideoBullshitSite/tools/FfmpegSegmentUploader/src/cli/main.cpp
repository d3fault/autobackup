#include <QCoreApplication>

#include "ffmpegsegmentuploadercli.h"

//TODOoptional: on first connect, remove any partially written files. which leads me to TODOoptimization: hack the sftp source code to detect such cases and to then do an sha1sum of the partially written fail vs the same range in the file to be uploaded. if they match, just continue uploading (you were going to overwrite it anyways) from that left off point. hell it might already do that, but i don't know and i doubt it.
//^^i'm not going to do that optional remove because it's implied when i do the overwrite and just adds unneeded complexity (had:logics)

//1) touch segments list file (or truncate to zero). i think it might need to be initially created by ffmpeg, i'm seeing weird inconsistencies with this
//2) launch this app (segments file must exist)
//3) launch ffmpeg

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    FfmpegSegmentUploaderCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
