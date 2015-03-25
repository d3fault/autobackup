#include "sftpuploadandrenameallvideosegmentsinfolder.h"

#include <QFileInfo>
#include <QListIterator>

#include "sftpuploaderandrenamerqueue.h"

SftpUploadAndRenameAllVideoSegmentsInFolder::SftpUploadAndRenameAllVideoSegmentsInFolder(QObject *parent)
    : QObject(parent)
    , m_SftpUploaderAndRenamerQueue(0)
{ }
void SftpUploadAndRenameAllVideoSegmentsInFolder::startSftpUploadAndRenameAllVideoSegmentsInFolder(const QString &dirContainingTheFiles, const QString &remoteDestinationToUploadTo, const QString &remoteDestinationToMoveTo, const QString &userHostPathComboSftpArg, const QString &sftpProcessPath)
{
    m_DirContainingTheFiles.setPath(dirContainingTheFiles);
    if(!m_DirContainingTheFiles.exists())
    {
        emit e("dir does not exist: " + dirContainingTheFiles);
        return;
    }

    if(m_SftpUploaderAndRenamerQueue)
        delete m_SftpUploaderAndRenamerQueue;
    m_SftpUploaderAndRenamerQueue = new SftpUploaderAndRenamerQueue(this);

    connect(this, SIGNAL(enqueueFileForUploadAndRenameRequested(SftpUploaderAndRenamerQueue_DestFilenameToRenameToAfterUpload_and_SrcFilenameToUpload_Type)), m_SftpUploaderAndRenamerQueue, SLOT(enqueueFileForUploadAndRename(SftpUploaderAndRenamerQueue_DestFilenameToRenameToAfterUpload_and_SrcFilenameToUpload_Type)));

    connect(m_SftpUploaderAndRenamerQueue, SIGNAL(o(QString)), this, SIGNAL(o(QString)));
    connect(m_SftpUploaderAndRenamerQueue, SIGNAL(e(QString)), this, SIGNAL(e(QString)));
    connect(m_SftpUploaderAndRenamerQueue, SIGNAL(sftpUploaderAndRenamerQueueStarted()), this, SLOT(handleSftpUploaderAndRenamerQueueStarted()));
    connect(m_SftpUploaderAndRenamerQueue, SIGNAL(fileUploadAndRenameSuccess(QString,QString)), this, SLOT(handleFileUploadAndRenameSuccess(QString,QString)));
    connect(m_SftpUploaderAndRenamerQueue, SIGNAL(sftpUploaderAndRenamerQueueStopped()), this, SIGNAL(quitRequested()));
    connect(m_SftpUploaderAndRenamerQueue, SIGNAL(quitRequested()), this, SIGNAL(quitRequested()));

    QMetaObject::invokeMethod(m_SftpUploaderAndRenamerQueue, "startSftpUploaderAndRenamerQueue", Q_ARG(QString,remoteDestinationToUploadTo), Q_ARG(QString,remoteDestinationToMoveTo), Q_ARG(QString,userHostPathComboSftpArg), Q_ARG(QString,sftpProcessPath));
}
void SftpUploadAndRenameAllVideoSegmentsInFolder::handleSftpUploaderAndRenamerQueueStarted()
{
    //simply enqueueeueue the directory of files, but giving them good dest filenames (unix timestamps + ".ogg") needs to be calculated
    QFileInfoList theFiles = m_DirContainingTheFiles.entryInfoList((QDir::Files | QDir::NoDotAndDotDot), (QDir::Time | QDir::Reversed));
    QListIterator<QFileInfo> it(theFiles);
    bool first = true;
    qint64 unixTimestampSeconds;
    while(it.hasNext())
    {
        const QFileInfo &currentFile = it.next();
        if(!currentFile.isFile())
            continue;
        if(currentFile.suffix().toLower() != "ogg") //ignore the segments.txt file
            continue;
        if(first)
        {
            first = false;
            QStringList fileNameSplitAtHyphen = currentFile.completeBaseName().split("-");
            if(fileNameSplitAtHyphen.size() != 2)
            {
                emit e("failed to split filename at hyphen: " + currentFile.fileName());
                return;
            }
            bool convertOk = false;
            unixTimestampSeconds = fileNameSplitAtHyphen.first().toLongLong(&convertOk);
            if(!convertOk)
            {
                emit e("failed to parse unix timestamp from left side of hyphen in filename: " + currentFile.fileName());
                return;
            }
        }
        else
        {
            unixTimestampSeconds += (60*3); //each video segment is 3 minutes, we have to calculate/fudge the timestamps accordingly. TODOoptional: cli arg for segment duration
        }

        SftpUploaderAndRenamerQueue_DestFilenameToRenameToAfterUpload_and_SrcFilenameToUpload_Type queueEntry;
        queueEntry.first = QString::number(unixTimestampSeconds) + ".ogg"; //TODOreq: use dedicated initial-upload + "watched" folders to dodge race condition where ffmpeg segment uploader and this app both "rename to" the same timestamp ( exists() might return false, but the rename might still overwrite ). the hvbs "folder watcher" is single threaded (or at least needs to be), so that it's "exists()" and "rename()" checks aren't racy (when hvbs exists() returns true, simply +1 the timestamp until exists() returns false imo)
        queueEntry.second = currentFile.absoluteFilePath();
        emit enqueueFileForUploadAndRenameRequested(queueEntry); //TODOrandom-design-thoughts: having this be a signal is kind of ugly because it isn't part of this class's "public api". Perhaps it should be a QMetaObject::invokeMethod instead, to keep our public api clean/accurate. I just now caught myself looking at the signal in the header (i was about to start coding the "cli" (user of this public api)) and was all like "wtf y is dis here? did i code sumfin wrong?". I tend to like the emit usage, just easier to type... but meta invoke's have the benefit of not uglifying my API. Also, signal emits might be faster since the method-string-name needs only to be resolved to the actual method once (might be wrong here (but with teh Qt5 connect syntax, it's definitely right since there is never a string to be resolved :-P))... not that that last point is even relevant in this network-io bound app xD
    }
    QMetaObject::invokeMethod(m_SftpUploaderAndRenamerQueue, "changeSftpUploaderAndRenamerQueueState", Q_ARG(SftpUploaderAndRenamerQueue::SftpUploaderAndRenamerQueueStateEnum, SftpUploaderAndRenamerQueue::SftpUploaderAndRenamerQueueState_StopWhenAllUploadsFinishSuccessfully));
}
void SftpUploadAndRenameAllVideoSegmentsInFolder::handleFileUploadAndRenameSuccess(const QString &remoteTimestamp, const QString &localFilename)
{
    emit o("uploaded '" + localFilename + "' and renamed it to '" + remoteTimestamp + ".ogg'");
}
