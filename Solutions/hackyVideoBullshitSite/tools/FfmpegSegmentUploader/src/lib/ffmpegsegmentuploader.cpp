#include "ffmpegsegmentuploader.h"

#include <QFile>
#include <QFileSystemWatcher> //was considering QSocketNotifier, but QFile::handle() doesn't work on windows -_-
#include <QDateTime>
#include <QStringList>
#include <QProcessEnvironment>
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>

#include "sftpuploaderandrenamerqueue.h"

FfmpegSegmentUploader::FfmpegSegmentUploader(QObject *parent)
    : QObject(parent)
    , m_FfmpegSegmentsEntryListFile(0)
    , m_FfmpegSessionDirWatcherAutoTransormingIntoSegmentsEntryListFileWatcher(0)
    , m_FfmpegProcess(new QProcess(this))
    , m_SftpUploaderAndRenamerQueue(0)
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
    connect(m_FfmpegProcess, SIGNAL(readyReadStandardError()), this, SLOT(handleFfmpegProecssStdErr()));
    connect(m_FfmpegProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(handleFfmpegProecssFinished(int,QProcess::ExitStatus)));
}
FfmpegSegmentUploader::~FfmpegSegmentUploader()
{
    if(m_FfmpegProcess->isOpen())
    {
        m_FfmpegProcess->terminate();
        emit o("waiting 30 seconds for ffmpeg to finish...");
        if(!m_FfmpegProcess->waitForFinished())
        {
            emit e("ffmpeg didn't finish within 30 seconds, so killing it");
            m_FfmpegProcess->kill();
        }
    }
}
void FfmpegSegmentUploader::startFfmpegSegmentUploader(qint64 segmentLengthSeconds, const QString &ffmpegCommand, const QString &localPath, const QString &remoteDestinationToUploadTo, const QString &remoteDestinationToMoveTo, const QString &userHostPathComboSftpArg, const QString &sftpProcessPath)
{
    if(segmentLengthSeconds < 10) //this used to be "1", but idk anything less than 10 just sounds stupid... and 1 sounds... unrealistic and/or impossible
    {
        emit e("segment length can't be less than 10 seconds");
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    }
    m_SegmentLengthSeconds = segmentLengthSeconds;
    m_FfmpegCommand = (ffmpegCommand.isEmpty() ? DEFAULT_ANDOR_D3FAULT_FFMPEG_COMMAND_ZOMG_ROFL_PUN_OR_WAIT_NO_IDK_JUST_LOL_THO : ffmpegCommand);
    if(!m_FfmpegCommand.contains(FfmpegSegmentUploader_FFMPEG_COMMAND_SEGMENT_LENGTH_STRING, Qt::CaseSensitive))
    {
        emit e("your ffmpeg command must contain the special string: " FfmpegSegmentUploader_FFMPEG_COMMAND_SEGMENT_LENGTH_STRING);
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    }
    m_FfmpegCommand.replace(FfmpegSegmentUploader_FFMPEG_COMMAND_SEGMENT_LENGTH_STRING, QString::number(segmentLengthSeconds), Qt::CaseSensitive);
    m_FfmpegSegmentUploaderSessionPath = appendSlashIfMissing(localPath) + QString::number(QDateTime::currentMSecsSinceEpoch()/1000) + QDir::separator();
    if(QFile::exists(m_FfmpegSegmentUploaderSessionPath))
    {
        emit e("wtf the session folder we were about to use (" + m_FfmpegSegmentUploaderSessionPath + ") already exists... is your clock set up right?");
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    }
    QDir whyTheFuckIsntMkPathStatic(m_FfmpegSegmentUploaderSessionPath);
    if(!whyTheFuckIsntMkPathStatic.mkpath(m_FfmpegSegmentUploaderSessionPath))
    {
        emit e("failed to make session folder for ffmpeg [segment uploader]: " + m_FfmpegSegmentUploaderSessionPath);
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    }
    m_FfmpegProcess->setWorkingDirectory(m_FfmpegSegmentUploaderSessionPath);
    if(!m_FfmpegCommand.contains(FfmpegSegmentUploader_FFMPEG_COMMAND_SEGMENT_ENTRY_LIST_FILE_REPLACEMENT_STRING, Qt::CaseSensitive))
    {
        emit e("your ffmpeg command must contain the special string: " FfmpegSegmentUploader_FFMPEG_COMMAND_SEGMENT_ENTRY_LIST_FILE_REPLACEMENT_STRING);
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
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
    connect(m_SftpUploaderAndRenamerQueue, SIGNAL(sftpUploaderAndRenamerQueueStopped()), this, SIGNAL(stoppedUploadingFfmpegSegments()));

    QMetaObject::invokeMethod(m_SftpUploaderAndRenamerQueue, "startSftpUploaderAndRenamerQueue", Q_ARG(QString, m_FfmpegSegmentUploaderSessionPath), Q_ARG(QString, remoteDestinationToUploadTo), Q_ARG(QString, remoteDestinationToMoveTo), Q_ARG(QString, userHostPathComboSftpArg), Q_ARG(QString, sftpProcessPath));
}
void FfmpegSegmentUploader::tellStatus()
{
    QMetaObject::invokeMethod(m_SftpUploaderAndRenamerQueue, "tellStatus"); //it makes sense that m_SftpUploaderAndRenamerQueue tells its status to us, and we append some 'this'-related information to the string before it is emitted back to the caller... but atm all of the status being told is only related to m_SftpUploaderAndRenamerQueue. if 'this' ever wants to add onto it, the connection from "statusGenerated" to our own "d" should be changed to some slot of 'this'
}
void FfmpegSegmentUploader::stopUploadingFfmpegSegments()
{
    if(m_FfmpegProcess->isOpen())
    {
        m_FfmpegProcess->terminate();
    }
    else
    {
        emit e("ffmpeg segment uploader told to stop, but the ffmpeg process it manages wasn't running");
        QMetaObject::invokeMethod(m_SftpUploaderAndRenamerQueue, "stopSftpUploaderAndRenamerQueue");
    }
}
void FfmpegSegmentUploader::handleSftpUploaderAndRenamerQueueStarted() //or i could just rename this method to "startFfmpeg" and yea. but this/that is ultimately a designEquals type question. i guess the signal saying "started" is enough, and this method should in fact be called "startFfmpeg". still, it's easier to see when you have UML and lines/arrows n shit...
{
    m_FfmpegProcess->start(m_FfmpegCommand, QIODevice::ReadOnly /*TODOoptimiztion: WriteOnly is most efficient, right? since only a 1-way channel needs to be opened? in this case i don't need write or read... aww shit nvm i need reading for reading stderr*/);
}
void FfmpegSegmentUploader::handleFfmpegProcessStarted()
{
    emit o("ffmpeg segment uploader started");
}
void FfmpegSegmentUploader::handleFfmpegProecssStdErr()
{
    QByteArray allStdErrBA = m_FfmpegProcess->readAllStandardError();
    QString allStdErrStr(allStdErrBA);
    emit e(allStdErrStr);
}
void FfmpegSegmentUploader::handleFfmpegSessionDirChangedSoMaybeTransformIntoFileWatcher(const QString &fileInDirThatChanged)
{
    if(fileInDirThatChanged == FfmpegSegmentUploader_FFMPEG_SEGMENT_ENTRY_LIST_FILENAME)
    {
        m_FfmpegSessionDirWatcherAutoTransormingIntoSegmentsEntryListFileWatcher->removePath(m_FfmpegSegmentUploaderSessionPath);
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
            QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
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
    QPair<QString,QString> newPair;
    newPair.second = m_FfmpegSegmentsEntryListFileTextStream.readLine();
    if(newPair.second.isEmpty())
        return; //idk my bff jill, but i saw sftp try to upload the localPath folder because 'second' was an empty string (no recursive flag = fails/exits)
    newPair.first = QString::number(QDateTime::currentDateTime().addSecs(-m_SegmentLengthSeconds).toMSecsSinceEpoch()/1000); //it's added to the segment entry list right when encoding finishes. idfk how i thought it was added right when encoding starts (thought i saw it happen (fucking saw it happen again and then stop happening one run later, WTFZ))... now gotta un-code dat shiz
    QMetaObject::invokeMethod(m_SftpUploaderAndRenamerQueue, "enqueueFileForUploadAndRename", Q_ARG(SftpUploaderAndRenamerQueueTimestampAndFilenameType, newPair));
    QMetaObject::invokeMethod(m_SftpUploaderAndRenamerQueue, "tryDequeueAndUploadSingleSegment");
}
void FfmpegSegmentUploader::handleFfmpegProecssFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(exitCode != 0 || exitStatus != QProcess::NormalExit)
    {
        emit e("ffmpeg process either crashed or exitted with non-zero exit code: " + QString::number(exitCode));
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    }
    emit o("ffmpeg exitted cleanly");

    //TO DOnereq: probably now wait on sftp upload queue to finish eating, now that ffmpeg has stopped giving it food
    QMetaObject::invokeMethod(m_SftpUploaderAndRenamerQueue, "stopSftpUploaderAndRenamerQueue");
}
