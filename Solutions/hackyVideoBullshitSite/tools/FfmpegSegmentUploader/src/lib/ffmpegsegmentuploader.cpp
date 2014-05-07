#include "ffmpegsegmentuploader.h"

#include <QFile>
#include <QFileSystemWatcher> //was considering QSocketNotifier, but QFile::handle() doesn't work on windows -_-
#include <QDateTime>
#include <QStringList>
#include <QProcessEnvironment>
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QTimer>

FfmpegSegmentUploader::FfmpegSegmentUploader(QObject *parent)
    : QObject(parent)
    , m_FfmpegSegmentsEntryListFile(0)
    , m_FfmpegSessionDirWatcherAutoTransormingIntoSegmentsEntryListFileWatcher(0)
    , m_FfmpegProcess(new QProcess(this))
    , m_FfmpegProcessKiller30secTimer(new QTimer(this))
    , m_SftpUploaderAndRenamerQueue(0)
    , m_HowToStopSftpUploaderAndRenamerQueueOnceFfmpegFinishes(SftpUploaderAndRenamerQueue::SftpUploaderAndRenamerQueueState_StopWhenAllUploadsFinishSuccessfully)
{
    //copy/pasting this is getting old (still doesn't seem to warrant a file), but it sure beats typing LD_LIBRARY_PATH shit before invoking _THIS_ application. i'm going to RUN this program way more times than i'm going to code it ;-) (parody/nod at 'commits are merged way more than they are written' or whatever that git quote was)
    QProcessEnvironment ffmpegProcessEnvironenment = m_FfmpegProcess->processEnvironment();
    QString ldLibraryPath = ffmpegProcessEnvironenment.value("LD_LIBRARY_PATH");
    QString ldPathToMakeHighPriority = "/usr/local/lib/";
    if(!ldLibraryPath.trimmed().isEmpty())
    {
        ldPathToMakeHighPriority = ldPathToMakeHighPriority + ":";
    }
    ldLibraryPath = ldPathToMakeHighPriority + ldLibraryPath;
    ffmpegProcessEnvironenment.insert("LD_LIBRARY_PATH", ldLibraryPath);
    m_FfmpegProcess->setProcessEnvironment(ffmpegProcessEnvironenment);

    connect(m_FfmpegProcess, SIGNAL(started()), this, SLOT(handleFfmpegProcessStarted()));
    connect(m_FfmpegProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(handleFfmpegProecssStdErr()));
    connect(m_FfmpegProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(handleFfmpegProecssFinished(int,QProcess::ExitStatus)));

    m_FfmpegProcessKiller30secTimer->setSingleShot(true);
    m_FfmpegProcessKiller30secTimer->setInterval(29000 /*same length as waitForFinished, minus 1 that we actually give waitForFinished*/);
    connect(m_FfmpegProcessKiller30secTimer, SIGNAL(timeout()), this, SLOT(killFfmpegProcessIfStillRunning()));
}
FfmpegSegmentUploader::~FfmpegSegmentUploader()
{
    if(m_FfmpegProcess->state() != QProcess::NotRunning)
    {
        emit o("ffmpeg process is being terminated from ffmpeg segment uploader destructor, so there is probably a bug in the stopping code");
        m_FfmpegProcess->terminate();
        if(!m_FfmpegProcess->waitForFinished())
        {
            emit o("ffmpeg process is being killed from ffmpeg segment uploader destructor, so there is probably a bug in the stopping code and perhaps ffmpeg itself");
            m_FfmpegProcess->kill();
        }
    }
}
void FfmpegSegmentUploader::startFfmpegSegmentUploader(qint64 segmentLengthSeconds, const QString &ffmpegCommand, const QString &localPath, const QString &remoteDestinationToUploadTo, const QString &remoteDestinationToMoveTo, const QString &userHostPathComboSftpArg, const QString &sftpProcessPath)
{
    if(segmentLengthSeconds < 10) //this used to be "1", but idk anything less than 10 just sounds stupid... and 1 sounds... unrealistic and/or impossible
    {
        emit e("segment length can't be less than 10 seconds");
        emit quitRequested();
        return;
    }
    m_SegmentLengthSeconds = segmentLengthSeconds;
    m_FfmpegCommand = (ffmpegCommand.isEmpty() ? DEFAULT_ANDOR_D3FAULT_FFMPEG_COMMAND_ZOMG_ROFL_PUN_OR_WAIT_NO_IDK_JUST_LOL_THO : ffmpegCommand);
    if(!m_FfmpegCommand.contains(FfmpegSegmentUploader_FFMPEG_COMMAND_SEGMENT_LENGTH_STRING, Qt::CaseSensitive))
    {
        emit e("your ffmpeg command must contain the special string: " FfmpegSegmentUploader_FFMPEG_COMMAND_SEGMENT_LENGTH_STRING);
        emit quitRequested();
        return;
    }
    m_FfmpegCommand.replace(FfmpegSegmentUploader_FFMPEG_COMMAND_SEGMENT_LENGTH_STRING, QString::number(segmentLengthSeconds), Qt::CaseSensitive);
    m_FfmpegSegmentUploaderSessionPath = appendSlashIfMissing(localPath) + QString::number(QDateTime::currentMSecsSinceEpoch()/1000) + QDir::separator();
    if(QFile::exists(m_FfmpegSegmentUploaderSessionPath))
    {
        emit e("wtf the session folder we were about to use (" + m_FfmpegSegmentUploaderSessionPath + ") already exists... is your clock set up right?");
        emit quitRequested();
        return;
    }
    QDir whyTheFuckIsntMkPathStatic(m_FfmpegSegmentUploaderSessionPath);
    if(!whyTheFuckIsntMkPathStatic.mkpath(m_FfmpegSegmentUploaderSessionPath))
    {
        emit e("failed to make session folder for ffmpeg [segment uploader]: " + m_FfmpegSegmentUploaderSessionPath);
        emit quitRequested();
        return;
    }
    m_FfmpegProcess->setWorkingDirectory(m_FfmpegSegmentUploaderSessionPath);
    if(!m_FfmpegCommand.contains(FfmpegSegmentUploader_FFMPEG_COMMAND_SEGMENT_ENTRY_LIST_FILE_REPLACEMENT_STRING, Qt::CaseSensitive))
    {
        emit e("your ffmpeg command must contain the special string: " FfmpegSegmentUploader_FFMPEG_COMMAND_SEGMENT_ENTRY_LIST_FILE_REPLACEMENT_STRING);
        emit quitRequested();
        return;
    }
    m_FfmpegCommand.replace(FfmpegSegmentUploader_FFMPEG_COMMAND_SEGMENT_ENTRY_LIST_FILE_REPLACEMENT_STRING, "\"" FfmpegSegmentUploader_FFMPEG_SEGMENT_ENTRY_LIST_FILENAME "\"", Qt::CaseSensitive);

    if(m_FfmpegSessionDirWatcherAutoTransormingIntoSegmentsEntryListFileWatcher)
        delete m_FfmpegSessionDirWatcherAutoTransormingIntoSegmentsEntryListFileWatcher;
    m_FfmpegSessionDirWatcherAutoTransormingIntoSegmentsEntryListFileWatcher = new QFileSystemWatcher(this);
    connect(m_FfmpegSessionDirWatcherAutoTransormingIntoSegmentsEntryListFileWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(handleFfmpegSessionDirChangedSoMaybeTransformIntoFileWatcher(QString)));
    connect(m_FfmpegSessionDirWatcherAutoTransormingIntoSegmentsEntryListFileWatcher, SIGNAL(fileChanged(QString)), this, SLOT(handleSegmentsEntryListFileModified()));
    m_FfmpegSessionDirWatcherAutoTransormingIntoSegmentsEntryListFileWatcher->addPath(m_FfmpegSegmentUploaderSessionPath);

    if(m_SftpUploaderAndRenamerQueue)
        delete m_SftpUploaderAndRenamerQueue;
    m_SftpUploaderAndRenamerQueue = new SftpUploaderAndRenamerQueue(this);
    connect(m_SftpUploaderAndRenamerQueue, SIGNAL(o(QString)), this, SIGNAL(o(QString)));
    connect(m_SftpUploaderAndRenamerQueue, SIGNAL(e(QString)), this, SIGNAL(e(QString)));
    connect(m_SftpUploaderAndRenamerQueue, SIGNAL(sftpUploaderAndRenamerQueueStarted()), this, SLOT(handleSftpUploaderAndRenamerQueueStarted()));
    connect(m_SftpUploaderAndRenamerQueue, SIGNAL(statusGenerated(QString)), this, SIGNAL(o(QString)));
    connect(this, SIGNAL(sftpUploaderAndRenamerQueueStateChangeRequested(SftpUploaderAndRenamerQueue::SftpUploaderAndRenamerQueueStateEnum)), m_SftpUploaderAndRenamerQueue, SLOT(changeSftpUploaderAndRenamerQueueState(SftpUploaderAndRenamerQueue::SftpUploaderAndRenamerQueueStateEnum)), Qt::QueuedConnection); //probably worthless that it's queued, but the hope is that it gives the ffmpeg process more time (or perhaps event loop cycles?) to flush the final segment filename to the segment list file
    connect(m_SftpUploaderAndRenamerQueue, SIGNAL(quitRequested()), this, SIGNAL(quitRequested())); //re: IBusinessObject: wondering if i need stopped signal since i have this, but eh quitRequested is as of right now intended more for fatal error type cases. BUT errors might still want to be ignored by the parent. stopped is like normal usage type shit... even though in this case they are both the same and are indirectly connected to qApp::quit!
    connect(m_SftpUploaderAndRenamerQueue, SIGNAL(sftpUploaderAndRenamerQueueStopped()), this, SIGNAL(stoppedUploadingFfmpegSegments()));

    m_HowToStopSftpUploaderAndRenamerQueueOnceFfmpegFinishes = SftpUploaderAndRenamerQueue::SftpUploaderAndRenamerQueueState_StopWhenAllUploadsFinishSuccessfully;

    QMetaObject::invokeMethod(m_SftpUploaderAndRenamerQueue, "startSftpUploaderAndRenamerQueue", Q_ARG(QString, remoteDestinationToUploadTo), Q_ARG(QString, remoteDestinationToMoveTo), Q_ARG(QString, userHostPathComboSftpArg), Q_ARG(QString, sftpProcessPath));
}
void FfmpegSegmentUploader::tellStatus()
{
    QMetaObject::invokeMethod(m_SftpUploaderAndRenamerQueue, "tellStatus"); //it makes sense that m_SftpUploaderAndRenamerQueue tells its status to us, and we append some 'this'-related information to the string before it is emitted back to the caller... but atm all of the status being told is only related to m_SftpUploaderAndRenamerQueue. if 'this' ever wants to add onto it, the connection from "statusGenerated" to our own "d" should be changed to some slot of 'this'
}
void FfmpegSegmentUploader::stopUploadingFfmpegSegmentsCleanly()
{
    m_HowToStopSftpUploaderAndRenamerQueueOnceFfmpegFinishes = SftpUploaderAndRenamerQueue::SftpUploaderAndRenamerQueueState_StopWhenAllUploadsFinishSuccessfully; //TODOreq: safe if already stopped/never started/whatever
    stopFfmpegProcess();
}
void FfmpegSegmentUploader::stopUploadingFfmpegSegmentsCleanlyUnlessDc()
{
    m_HowToStopSftpUploaderAndRenamerQueueOnceFfmpegFinishes = SftpUploaderAndRenamerQueue::SftpUploaderAndRenamerQueueState_StopWhenAllUploadsFinishSuccessfullyUnlessDc;
    stopFfmpegProcess();
}
void FfmpegSegmentUploader::stopUploadingFfmpegSegmentsNow()
{
    m_HowToStopSftpUploaderAndRenamerQueueOnceFfmpegFinishes = SftpUploaderAndRenamerQueue::SftpUploaderAndRenamerQueueState_StopNow;
    stopFfmpegProcess();
    //TODOreq: sftp's QProcess::terminate kills active upload? needs to. if not, gotta kill :(
}
void FfmpegSegmentUploader::handleSftpUploaderAndRenamerQueueStarted() //or i could just rename this method to "startFfmpeg" and yea. but this/that is ultimately a designEquals type question. i guess the signal saying "started" is enough, and this method should in fact be called "startFfmpeg". still, it's easier to see when you have UML and lines/arrows n shit...
{
    m_FfmpegProcess->start(m_FfmpegCommand, QIODevice::ReadOnly /*TODOoptimiztion: WriteOnly is most efficient, right? since only a 1-way channel needs to be opened? in this case i don't need write or read... aww shit nvm i need reading for reading stderr*/);
}
void FfmpegSegmentUploader::handleFfmpegProcessStarted()
{
    emit o("ffmpeg has started capturing");
}
void FfmpegSegmentUploader::handleFfmpegProecssStdErr()
{
    QByteArray allStdErrBA = m_FfmpegProcess->readAllStandardOutput();
    QString allStdErrStr(allStdErrBA);
    emit e(allStdErrStr);
}
void FfmpegSegmentUploader::handleFfmpegSessionDirChangedSoMaybeTransformIntoFileWatcher(const QString &fileInDirThatChanged)
{
    if(QFile::exists(m_FfmpegSegmentUploaderSessionPath + FfmpegSegmentUploader_FFMPEG_SEGMENT_ENTRY_LIST_FILENAME))
    {
        m_FfmpegSessionDirWatcherAutoTransormingIntoSegmentsEntryListFileWatcher->removePath(fileInDirThatChanged);
        disconnect(m_FfmpegSessionDirWatcherAutoTransormingIntoSegmentsEntryListFileWatcher, SIGNAL(directoryChanged(QString)));

        if(m_FfmpegSegmentsEntryListFile)
            delete m_FfmpegSegmentsEntryListFile;
        QString filenameOfSegmentsEntryList = m_FfmpegSegmentUploaderSessionPath + FfmpegSegmentUploader_FFMPEG_SEGMENT_ENTRY_LIST_FILENAME;
        m_FfmpegSegmentsEntryListFile = new QFile(filenameOfSegmentsEntryList, this);
        if(!m_FfmpegSegmentsEntryListFile->open(QIODevice::ReadOnly | QIODevice::Text))
        {
            emit e("failed to open file for reading: '" + filenameOfSegmentsEntryList + "'");
            delete m_FfmpegSegmentsEntryListFile;
            m_FfmpegSegmentsEntryListFile = 0;
            emit quitRequested();
            return;
        }
        m_FfmpegSegmentsEntryListFileTextStream.setDevice(m_FfmpegSegmentsEntryListFile);
        m_FfmpegSegmentsEntryListFileTextStream.seek(0);

        m_FfmpegSessionDirWatcherAutoTransormingIntoSegmentsEntryListFileWatcher->addPath(filenameOfSegmentsEntryList);
        handleSegmentsEntryListFileModified(); //missed one, so...
    }
}
void FfmpegSegmentUploader::handleSegmentsEntryListFileModified()
{
    QString newLine = m_FfmpegSegmentsEntryListFileTextStream.readLine();
    if(newLine.isEmpty())
        return; //idk my bff jill, but i saw sftp try to upload the localPath folder because 'second' was an empty string (no recursive flag = fails/exits)
    QPair<QString,QString> newPair;
    newPair.second = m_FfmpegSegmentUploaderSessionPath + newLine;
    newPair.first = QString::number(QDateTime::currentDateTime().addSecs(-m_SegmentLengthSeconds).toMSecsSinceEpoch()/1000); //it's added to the segment entry list right when encoding finishes. idfk how i thought it was added right when encoding starts (thought i saw it happen (fucking saw it happen again and then stop happening one run later, WTFZ))... now gotta un-code dat shiz
    QMetaObject::invokeMethod(m_SftpUploaderAndRenamerQueue, "enqueueFileForUploadAndRename", Q_ARG(SftpUploaderAndRenamerQueueTimestampAndFilenameType, newPair));
}
void FfmpegSegmentUploader::killFfmpegProcessIfStillRunning()
{
    if(m_FfmpegProcess->state() != QProcess::NotRunning)
    {
        if(!m_FfmpegProcess->waitForFinished(1000))
        {
            emit e("ffmpeg didn't finish within 30 seconds, so killing it");
            m_FfmpegProcess->kill();
        }
    }
}
void FfmpegSegmentUploader::handleFfmpegProecssFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    emit sftpUploaderAndRenamerQueueStateChangeRequested(m_HowToStopSftpUploaderAndRenamerQueueOnceFfmpegFinishes);
    if((exitCode != 0 && exitCode != 255) || exitStatus != QProcess::NormalExit)
    {
        emit e("ffmpeg process either crashed or exitted with non-zero exit code: " + QString::number(exitCode));
        emit quitRequested();
        return;
    }
    emit o("ffmpeg exitted cleanly");
}
