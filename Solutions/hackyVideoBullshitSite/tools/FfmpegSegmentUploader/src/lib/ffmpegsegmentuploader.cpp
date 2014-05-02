#include "ffmpegsegmentuploader.h"

#include <QFile>
#include <QFileSystemWatcher> //was considering QSocketNotifier, but QFile::handle() doesn't work on windows -_-
#include <QDateTime>
#include <QStringList>

#include "sftpuploaderandrenamerqueue.h"

FfmpegSegmentUploader::FfmpegSegmentUploader(QObject *parent)
    : QObject(parent)
    , m_FfmpegSegmentsEntryListFile(0)
    , m_FfmpegSegmentsEntryListFileWatcher(0)
    , m_SftpUploaderAndRenamerQueue(0)
{ }
void FfmpegSegmentUploader::startUploadingSegmentsOnceFfmpegAddsThemToTheSegmentsEntryList(const QString &filenameOfSegmentsEntryList, qint64 segmentLengthSeconds, const QString &localPath, const QString &remoteDestinationToUploadTo, const QString &remoteDestinationToMoveTo, const QString &userHostPathComboSftpArg, const QString &sftpProcessPath)
{
    if(segmentLengthSeconds < 1)
    {
        emit d("segment length can't be less than 1");
        return;
    }
    m_SegmentLengthSeconds = segmentLengthSeconds;
    if(filenameOfSegmentsEntryList.isEmpty())
    {
        emit d("invalid filename of segments entry list: '" + filenameOfSegmentsEntryList + "'");
        return;
    }
    m_FilenameOfSegmentsEntryList = filenameOfSegmentsEntryList;
    if(!QFile::exists(m_FilenameOfSegmentsEntryList))
    {
        emit d("segment entry list file does not exist: '" + m_FilenameOfSegmentsEntryList + "'");
        return; //TODOreq: this, and the below return, and probably otherh places, should quit cleanly/now. do more than just return (sftp may be running for example)
    }
    if(m_FfmpegSegmentsEntryListFile)
        delete m_FfmpegSegmentsEntryListFile;
    m_FfmpegSegmentsEntryListFile = new QFile(m_FilenameOfSegmentsEntryList, this);
    if(!m_FfmpegSegmentsEntryListFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit d("failed to open file for reading: '" + m_FilenameOfSegmentsEntryList + "'");
        delete m_FfmpegSegmentsEntryListFile;
        m_FfmpegSegmentsEntryListFile = 0;
        return;
    }
    m_FfmpegSegmentsEntryListFileTextStream.setDevice(m_FfmpegSegmentsEntryListFile);
    m_FfmpegSegmentsEntryListFileTextStream.seek(0);

    if(m_FfmpegSegmentsEntryListFileWatcher)
        delete m_FfmpegSegmentsEntryListFileWatcher;
    m_FfmpegSegmentsEntryListFileWatcher = new QFileSystemWatcher(this);
    m_FfmpegSegmentsEntryListFileWatcher->addPath(filenameOfSegmentsEntryList);
    connect(m_FfmpegSegmentsEntryListFileWatcher, SIGNAL(fileChanged(QString)), this, SLOT(handleSegmentsEntryListFileModified()));

    if(m_SftpUploaderAndRenamerQueue)
        delete m_SftpUploaderAndRenamerQueue;
    m_SftpUploaderAndRenamerQueue = new SftpUploaderAndRenamerQueue(this);
    connect(m_SftpUploaderAndRenamerQueue, SIGNAL(o(QString)), this, SIGNAL(d(QString)));
    connect(m_SftpUploaderAndRenamerQueue, SIGNAL(e(QString)), this, SIGNAL(d(QString)));
    connect(m_SftpUploaderAndRenamerQueue, SIGNAL(sftpUploaderAndRenamerQueueStarted()), this, SLOT(handleSftpUploaderAndRenamerQueueStarted()));
    connect(m_SftpUploaderAndRenamerQueue, SIGNAL(statusGenerated(QString)), this, SIGNAL(d(QString)));
    connect(m_SftpUploaderAndRenamerQueue, SIGNAL(sftpUploaderAndRenamerQueueStopped()), this, SIGNAL(stoppedUploadingFfmpegSegments()));

    QMetaObject::invokeMethod(m_SftpUploaderAndRenamerQueue, "startSftpUploaderAndRenamerQueue", Q_ARG(QString, localPath), Q_ARG(QString, remoteDestinationToUploadTo), Q_ARG(QString, remoteDestinationToMoveTo), Q_ARG(QString, userHostPathComboSftpArg), Q_ARG(QString, sftpProcessPath));
}
void FfmpegSegmentUploader::tellStatus()
{
    QMetaObject::invokeMethod(m_SftpUploaderAndRenamerQueue, "tellStatus"); //it makes sense that m_SftpUploaderAndRenamerQueue tells its status to us, and we append some 'this'-related information to the string before it is emitted back to the caller... but atm all of the status being told is only related to m_SftpUploaderAndRenamerQueue. if 'this' ever wants to add onto it, the connection from "statusGenerated" to our own "d" should be changed to some slot of 'this'
}
void FfmpegSegmentUploader::stopUploadingFfmpegSegments()
{
    QMetaObject::invokeMethod(m_SftpUploaderAndRenamerQueue, "stopSftpUploaderAndRenamerQueue");
}
void FfmpegSegmentUploader::handleSftpUploaderAndRenamerQueueStarted()
{
    emit d("ffmpeg segment uploader starting");
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
