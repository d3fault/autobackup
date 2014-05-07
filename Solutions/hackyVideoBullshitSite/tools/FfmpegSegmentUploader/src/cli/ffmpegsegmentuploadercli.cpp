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
    connect(this, SIGNAL(stopUploadingFfmpegSegmentsCleanlyRequested()), m_FfmpegSegmentUploader, SLOT(stopUploadingFfmpegSegmentsCleanly()));
    connect(this, SIGNAL(stopUploadingFfmpegSegmentsCleanlyUnlessDcRequested()), m_FfmpegSegmentUploader, SLOT(stopUploadingFfmpegSegmentsCleanlyUnlessDc()));
    connect(this, SIGNAL(stopUploadingFfmpegSegmentsNowRequested()), m_FfmpegSegmentUploader, SLOT(stopUploadingFfmpegSegmentsNow()));
    connect(m_FfmpegSegmentUploader, SIGNAL(quitRequested()), qApp, SLOT(quit()), Qt::QueuedConnection);
    connect(m_FfmpegSegmentUploader, SIGNAL(stoppedUploadingFfmpegSegments()), QCoreApplication::instance(), SLOT(quit()), Qt::QueuedConnection); //hmm i think a custom object with a QFutureSynchronizer and a handful of QFutures passed to it would solve the.... oh nvm i think i need QFutureWatchers with it too in order to not block

    //TODOreq: applies to most all of my cli arg taking apps: qt/etc(desktop-env) take args too so i need to use "-flag <arg>" type shit and not depend on count (and probably not ordering). MAYBE Qt/etc strip their relevant args from argv before my app gets it, but idfk tbh
    //^ACTUALLY the doc of cli parser hints that qt strips it's shit out of QCore::args(), which is perfect!

    QStringList arguments = QCoreApplication::arguments();
    if(arguments.size() < 6 || arguments.size() > 8)
    {
        cliUsage();
        QMetaObject::invokeMethod(QCoreApplication::instance(), "quit", Qt::QueuedConnection); // "quitLater"? maybe i'm just doing something wrong and it's coincidence every time, but i tend to see a silent deadlock at "return app.exec()" whenever i don't make this a QueuedConnection
        return;
    }
    int argIndex = 1; //baller
    QString localPath = arguments.at(argIndex++);
    QString remoteDestinationToUploadTo = arguments.at(argIndex++);
    QString remoteDestinationToMoveTo = arguments.at(argIndex++);
    QString userHostPathComboSftpArg = arguments.at(argIndex++);
    bool convertOk = false;
    qint64 segmentLengthSeconds = static_cast<qint64>(arguments.at(argIndex++).toInt(&convertOk));
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

    QMetaObject::invokeMethod(m_FfmpegSegmentUploader, "startFfmpegSegmentUploader", Q_ARG(qint64, segmentLengthSeconds), Q_ARG(QString, ffmpegCommand), Q_ARG(QString, localPath), Q_ARG(QString, remoteDestinationToUploadTo), Q_ARG(QString, remoteDestinationToMoveTo), Q_ARG(QString, userHostPathComboSftpArg), Q_ARG(QString, sftpProcessPath));
    //vs: m_FfmpegSegmentUploader->startUplo... -- above has benefit of not needing to be changed if m_FfmpegSegmentUploader is ever moved to another thread. the autoconnection makes it basically a direct call anyways <3. man i fucking love qt. who am i kidding though, this isn't performance code... guh...

    cliUserInterfaceMenu();
}
void FfmpegSegmentUploaderCli::cliUsage()
{
    handleO("Usage:");
    handleO("FfmpegSegmentUploaderCli ffmpegWorkingDirWhereSegmentsWillBeWritten remoteDestinationUploadScratch remoteDestinationWatchedFolderToMoveTo userHostPathComboSftpArg segmentLengthSeconds [ffmpegCmd=\"" DEFAULT_ANDOR_D3FAULT_FFMPEG_COMMAND_ZOMG_ROFL_PUN_OR_WAIT_NO_IDK_JUST_LOL_THO "\"] [sftpProcessPath=/usr/bin/sftp]");
}
void FfmpegSegmentUploaderCli::cliUserInterfaceMenu()
{
    QString cliUsageStr =   "Available Actions (H to show this again):\n"
                            " 1   - Query ffmpeg segment status info, which includes:\n\t-Most recent segment entry\n\t-The size of the upload queue\n\t-The 'head' of the upload queue\n\t-The sftp connection status)\n"
                            " Q   - Stop recording and Quit after all segments are uploaded (sftp will retry indefinitely)\n"
                            " QQ  - Stop recording and Quit after all segments are uploaded, unless sftp connection is dead or dies beforehand\n"
                            " QQQ - Stop recording and Quit now (use Q or QQ if you can)\n";
    handleO(cliUsageStr);
    //TODOoptional: maybe an increase/decrease verbosity command as well
}
void FfmpegSegmentUploaderCli::handleStandardInputReceived(const QString &standardInputLine)
{
    QString standardInputLineToLower = standardInputLine.toLower();
    if(standardInputLineToLower == "h")
    {
        cliUserInterfaceMenu();
    }
    else if(standardInputLineToLower == "1")
    {
        emit tellSegmentInformationsRequested();
    }
    else if(standardInputLineToLower == "q")
    {
        handleO("ffmpeg segments uploader will quit once it's child ffmpeg process finishes and once all segments are uploaded...");
        emit stopUploadingFfmpegSegmentsCleanlyRequested();
    }
    else if(standardInputLineToLower == "qq") //TODOreq: Q can become QQ, Q and QQ can become QQQ. TODOoptional: QQ can become Q
    {
        handleO("ffmpeg segments uploader will quit once it's child ffmpeg process finishes and once all segments are uploaded... unless the sftp connection is dead or dies beforehand");
        emit stopUploadingFfmpegSegmentsCleanlyUnlessDcRequested();
    }
    else if(standardInputLineToLower == "qqq") //TODOreq: destruction/last-minute-cleanup/whatever uses this method
    {
        disconnect(m_StandardInputNotifier, SIGNAL(standardInputReceivedLine(QString)));
        handleO("ffmpeg segments uploader will quit once it's child ffmpeg process finishes (pending and future uploads will be cancelled)");
        emit stopUploadingFfmpegSegmentsNowRequested();
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
