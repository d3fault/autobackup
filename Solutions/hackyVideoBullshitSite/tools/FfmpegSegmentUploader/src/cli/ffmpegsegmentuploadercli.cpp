#include "ffmpegsegmentuploadercli.h"

#include <QCoreApplication>
#include <QStringList>

FfmpegSegmentUploaderCli::FfmpegSegmentUploaderCli(QObject *parent) :
    QObject(parent), m_StandardInputNotifier(new StandardInputNotifier(this)), m_FfmpegSegmentUploader(new FfmpegSegmentUploader(this)), m_StdOut(stdout)
{
    connect(m_StandardInputNotifier, SIGNAL(standardInputReceivedLine(QString)), this, SLOT(handleStandardInputReceived(QString)));

    connect(m_FfmpegSegmentUploader, SIGNAL(d(QString)), this, SLOT(handleD(QString)));
    connect(this, SIGNAL(tellSegmentInformationsRequested()), m_FfmpegSegmentUploader, SLOT(tellStatus()));
    connect(this, SIGNAL(stopUploadingFfmpegSegmentsRequested()), m_FfmpegSegmentUploader, SLOT(stopUploadingFfmpegSegments()));
    connect(m_FfmpegSegmentUploader, SIGNAL(stoppedUploadingFfmpegSegments()), QCoreApplication::instance(), SLOT(quit())); //hmm i think a custom object with a QFutureSynchronizer and a handful of QFutures passed to it would solve the.... oh nvm i think i need QFutureWatchers with it too in order to not block

    QStringList arguments = QCoreApplication::arguments();
    if(arguments.size() < 6 || arguments.size() > 7)
    {
        cliUsage();
        QMetaObject::invokeMethod(QCoreApplication::instance(), "quit");
        return;
    }
    int argIndex = 1; //baller
    QString filenameOfSegmentsEntryList = arguments.at(argIndex++);
    QString localPath = arguments.at(argIndex++);
    QString remoteDestinationToUploadTo = arguments.at(argIndex++);
    QString remoteDestinationToMoveTo = arguments.at(argIndex++);
    QString userHostPathComboSftpArg = arguments.at(argIndex++);
    QString sftpProcessPath;
    if(arguments.size() > 6) //TODOoptional: this if based on argIndex vs size(), so i can add some in between without changing the 6 hardcode <3
    {
        sftpProcessPath = arguments.at(argIndex++);
    }

    QMetaObject::invokeMethod(m_FfmpegSegmentUploader, "startUploadingSegmentsOnceFfmpegStartsEncodingTheNextOne", Q_ARG(QString, filenameOfSegmentsEntryList), Q_ARG(QString, localPath), Q_ARG(QString, remoteDestinationToUploadTo), Q_ARG(QString, remoteDestinationToMoveTo), Q_ARG(QString, userHostPathComboSftpArg), Q_ARG(QString, sftpProcessPath));
    //vs: m_FfmpegSegmentUploader->startUplo... -- above has benefit of not needing to be changed if m_FfmpegSegmentUploader is ever moved to another thread. the autoconnection makes it basically a direct call anyways <3. man i fucking love qt. who am i kidding though, this isn't performance code... guh...

    cliUserInterfaceMenu();
}
void FfmpegSegmentUploaderCli::cliUsage()
{
    handleD("Usage:");
    handleD("FfmpegSegmentUploaderCli filenameOfSegmentsEntryList localPathWhereSegmentsAreWritten remoteDestinationToUploadTo remoteDestinationToMoveTo userHostPathComboSftpArg [sftpProcessPath=/usr/bin/sftp]");
}
void FfmpegSegmentUploaderCli::cliUserInterfaceMenu()
{
    handleD("Available Actions (H to show this again):");
    handleD("\t0\t- Display ffmpeg segment status (most recent segment entry (which is probably still being encoded), the size of the upload queue, the 'head' of the upload queue, and the sftp connection status)");
    handleD("\tq\t- Quit after all segments are uploaded (you need to stop ffmpeg first)"); //TODOreq: decide if "quit now", "quit after current upload finishes", or "quit when queue is next seen empty", or all three :-P. the last one implies that ffmpeg is stopped separately/before
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
    else if(standardInputLineToLower == "q")
    {
        handleD("Cleanly stopping ffmpeg segments uploader once all segments are uploaded...");
        emit stopUploadingFfmpegSegmentsRequested();
    }
    else
    {
        cliUserInterfaceMenu();
        handleD("Invalid selection: '" + standardInputLineToLower + "'");
    }
}
void FfmpegSegmentUploaderCli::handleD(const QString &msg)
{
    m_StdOut << msg << endl;
}
