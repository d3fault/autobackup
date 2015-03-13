#ifndef SFTPUPLOADANDRENAMEALLVIDEOSEGMENTSINFOLDER_H
#define SFTPUPLOADANDRENAMEALLVIDEOSEGMENTSINFOLDER_H

#include <QObject>
#include <QDir>

#include "sftpuploaderandrenamerqueue.h"

class SftpUploadAndRenameAllVideoSegmentsInFolder : public QObject
{
    Q_OBJECT
public:
    explicit SftpUploadAndRenameAllVideoSegmentsInFolder(QObject *parent = 0);
private:
    SftpUploaderAndRenamerQueue *m_SftpUploaderAndRenamerQueue;
    QDir m_DirContainingTheFiles;
signals:
    void o(const QString &msg);
    void e(const QString &msg);
    void enqueueFileForUploadAndRenameRequested(SftpUploaderAndRenamerQueue_DestFilenameToRenameToAfterUpload_and_SrcFilenameToUpload_Type);
    void quitRequested();
public slots:
    void startSftpUploadAndRenameAllVideoSegmentsInFolder(const QString &dirContainingTheFiles, const QString &remoteDestinationToUploadTo, const QString &remoteDestinationToMoveTo, const QString &userHostPathComboSftpArg, const QString &sftpProcessPath);
private slots:
    void handleSftpUploaderAndRenamerQueueStarted();
    void handleFileUploadAndRenameSuccess(const QString &remoteTimestamp, const QString &localFilename);
};

#endif // SFTPUPLOADANDRENAMEALLVIDEOSEGMENTSINFOLDER_H
