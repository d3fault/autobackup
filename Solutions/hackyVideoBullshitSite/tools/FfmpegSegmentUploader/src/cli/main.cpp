#include <QCoreApplication>

#include "ffmpegsegmentuploadercli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    FfmpegSegmentUploaderCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
