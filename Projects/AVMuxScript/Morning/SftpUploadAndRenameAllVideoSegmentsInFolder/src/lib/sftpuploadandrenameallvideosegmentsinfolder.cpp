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
    QFileInfoList theFiles = m_DirContainingTheFiles.entryInfoList((QDir::Files | QDir::NoDotAndDotDot), QDir::Name);
    QListIterator<QFileInfo> it(theFiles);
    while(it.hasNext())
    {
        const QFileInfo &currentFile = it.next();
        SftpUploaderAndRenamerQueue_DestFilenameToRenameToAfterUpload_and_SrcFilenameToUpload_Type queueEntry;
        queueEntry.first = TODOreq; timestamp+ogg; //TODOreq: sort properly: segment-1.ogg, segment-10.ogg, segment-2.ogg. TODOreq: use dedicated "watched" folder to dodge race condition where ffmpeg segment uploader and this app both "rename to" the same timestamp ( exists() might return false, but the rename might still overwrite ). the hvbs "folder watcher" is single threaded (or at least needs to be), so that it's "exists()" and "rename()" checks aren't racy (when hvbs exists() returns true, simply +1 the timestamp until exists() returns false imo)
        queueEntry.second = currentFile.absoluteFilePath();
        emit enqueueFileForUploadAndRenameRequested(queueEntry);

    }
}
void SftpUploadAndRenameAllVideoSegmentsInFolder::handleFileUploadAndRenameSuccess(const QString &remoteTimestamp, const QString &localFilename)
{

}
