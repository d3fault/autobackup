#include "ffmpegsegmentuploadercli.h"

#include <QCoreApplication>
#include <QStringList>

FfmpegSegmentUploaderCli::FfmpegSegmentUploaderCli(QObject *parent)
    : QObject(parent)
    , m_StandardInputNotifier(new StandardInputNotifier(this))
    , m_FfmpegSegmentUploader(new FfmpegSegmentUploader(this))
    , m_StdOut(stdout)
    , m_StdErr(stderr)
{
    connect(m_StandardInputNotifier, SIGNAL(standardInputReceivedLine(QString)), this, SLOT(handleStandardInputReceived(QString)));

    connect(m_FfmpegSegmentUploader, SIGNAL(o(QString)), this, SLOT(handleO(QString)));
    connect(m_FfmpegSegmentUploader, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    connect(this, SIGNAL(tellSegmentInformationsRequested()), m_FfmpegSegmentUploader, SLOT(tellStatus()));
    connect(this, SIGNAL(stopUploadingFfmpegSegmentsRequested()), m_FfmpegSegmentUploader, SLOT(stopUploadingFfmpegSegments()));
    connect(m_FfmpegSegmentUploader, SIGNAL(stoppedUploadingFfmpegSegments()), QCoreApplication::instance(), SLOT(quit()), Qt::QueuedConnection); //hmm i think a custom object with a QFutureSynchronizer and a handful of QFutures passed to it would solve the.... oh nvm i think i need QFutureWatchers with it too in order to not block

    QStringList arguments = QCoreApplication::arguments();
    if(arguments.size() < 8 || arguments.size() > 9)
    {
        cliUsage();
        QMetaObject::invokeMethod(QCoreApplication::instance(), "quit", Qt::QueuedConnection); // "quitLater"? maybe i'm just doing something wrong and it's coincidence every time, but i tend to see a silent deadlock at "return app.exec()" whenever i don't make this a QueuedConnection
        return;
    }
    int argIndex = 1; //baller
    QString filenameOfSegmentsEntryList = arguments.at(argIndex++);
    QString localPath = arguments.at(argIndex++);
    QString remoteDestinationToUploadTo = arguments.at(argIndex++);
    QString remoteDestinationToMoveTo = arguments.at(argIndex++);
    QString userHostPathComboSftpArg = arguments.at(argIndex++);
    bool convertOk = false;
    qint64 segmentLengthSeconds = static_cast<qint64>(arguments.at(argIndex++).toInt(&convertOk)); //can probably take this out now that we're wrapping ffmpeg
    if(!convertOk)
    {
        cliUsage();
        handleO("segment length not a valid number");
        QMetaObject::invokeMethod(QCoreApplication::instance(), "quit", Qt::QueuedConnection);
        return;
    }
    QString ffmpegCommand;
    if(arguments.size() > argIndex)
    {
        ffmpegCommand = arguments.at(argIndex++);
    }
    QString sftpProcessPath;
    if(arguments.size() > argIndex)
    {
        sftpProcessPath = arguments.at(argIndex++);
    }

    QMetaObject::invokeMethod(m_FfmpegSegmentUploader, "startUploadingSegmentsOnceFfmpegAddsThemToTheSegmentsEntryList", Q_ARG(QString, filenameOfSegmentsEntryList), Q_ARG(qint64, segmentLengthSeconds), Q_ARG(QString, ffmpegCommand), Q_ARG(QString, localPath), Q_ARG(QString, remoteDestinationToUploadTo), Q_ARG(QString, remoteDestinationToMoveTo), Q_ARG(QString, userHostPathComboSftpArg), Q_ARG(QString, sftpProcessPath));
    //vs: m_FfmpegSegmentUploader->startUplo... -- above has benefit of not needing to be changed if m_FfmpegSegmentUploader is ever moved to another thread. the autoconnection makes it basically a direct call anyways <3. man i fucking love qt. who am i kidding though, this isn't performance code... guh...

    cliUserInterfaceMenu();
}
void FfmpegSegmentUploaderCli::cliUsage()
{
    handleO("Usage:");
    handleO("FfmpegSegmentUploaderCli segmentsEntryListFilename ffmpegWorkingDirWhereSegmentsWillBeWritten remoteDestinationToUploadTo remoteDestinationToMoveTo userHostPathComboSftpArg segmentLengthSeconds [ffmpegCmd=\"" DEFAULT_ANDOR_D3FAULT_FFMPEG_COMMAND_ZOMG_ROFL_PUN_OR_WAIT_NO_IDK_JUST_LOL_THO "\"] [sftpProcessPath=/usr/bin/sftp]");
}
void FfmpegSegmentUploaderCli::cliUserInterfaceMenu()
{
    handleO("Available Actions (H to show this again):");
    handleO(" 0 - Query ffmpeg segment status info, which includes:\n\t-Most recent segment entry\n\t-The size of the upload queue\n\t-The 'head' of the upload queue\n\t-The sftp connection status)");
    //TODOoptional: maybe an increase/decrease verbosity command as well
    handleO(" Q - Quit after all segments are uploaded (you need to stop ffmpeg first)");
}
void FfmpegSegmentUploaderCli::handleStandardInputReceived(const QString &standardInputLine)
{
    QString standardInputLineToLower = standardInputLine.toLower();
    if(standardInputLineToLower == "h")
    {
        cliUserInterfaceMenu();
    }
    else if(standardInputLineToLower == "0")
    {
        emit tellSegmentInformationsRequested();
    }
    else if(standardInputLineToLower == "q") //TODOreq: quit ffmpeg, wait for finish, wait for queue upload, then quit
    {
        handleO("ffmpeg segments uploader will finish once it's child ffmpeg process finishes and once all segments are uploaded...");
        emit stopUploadingFfmpegSegmentsRequested();
    }
    else
    {
        cliUserInterfaceMenu();
        handleO("Invalid selection: '" + standardInputLineToLower + "'");
    }
}
void FfmpegSegmentUploaderCli::handleO(const QString &msg)
{
    m_StdOut << msg << endl;
}
void FfmpegSegmentUploaderCli::handleE(const QString &msg)
{
    m_StdErr << msg << endl;
}
