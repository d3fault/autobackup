#ifndef VIDEOSEGMENTSIMPORTERFOLDERWATCHER_H
#define VIDEOSEGMENTSIMPORTERFOLDERWATCHER_H

#include <QObject>
#include <QDateTime>
#include <QDir>

#include "sftpuploaderandrenamerqueue.h"

#define VideoSegmentsImporterFolderWatcher_DONT_PROPAGATE_TO_NEIGHBOR "null@dev.poopybutt"

class QFileSystemWatcher;

class VideoSegmentsImporterFolderWatcher : public QObject
{
    Q_OBJECT
public:
    explicit VideoSegmentsImporterFolderWatcher(QObject *parent = 0);
private:
    QFileSystemWatcher *m_DirectoryWatcher;
    QString m_VideoSegmentsImporterFolderToWatchWithSlashAppended;
    QString m_VideoSegmentsImporterFolderScratchSpacePathWithSlashAppended;
    //QDir m_VideoSegmentsImporterFolderScratchSpace;
    QString m_VideoSegmentsImporterFolderToMoveToWithSlashAppended;    

    bool m_PropagateToNeighbor;
    SftpUploaderAndRenamerQueue *m_SftpUploaderAndRenamerQueue;

    bool jitEnsureFolderExists(const QString &absoluteFolderPathToMaybeJitCreate);
    QString ensureMoveWontOverwrite_ByAddingSecondsIfItWouldHave(const QString &folderToMoveTo_WithSlashAppended, long long desiredFilename);
    void maybePropagateToNeighbor(const QString &timestampUsedInRename, const QString &localFilenameToPropagate);
    void beginStoppingVideoNeighborPropagation(SftpUploaderAndRenamerQueue::SftpUploaderAndRenamerQueueStateEnum newSftpUploaderAndRenamerQueueState);

    inline QString appendSlashIfNeeded(QString inputString) { return inputString.endsWith("/") ? inputString : inputString.append("/"); } //always easier than a pri include
signals:
    void o(const QString &);
    void e(const QString &);
    void tellNeighborPropagationInformationRequested();
    void sftpUploaderAndRenamerQueueStateChangedRequested(SftpUploaderAndRenamerQueue::SftpUploaderAndRenamerQueueStateEnum newSftpUploaderAndRenamerQueueState);
    void videoSegmentsImporterFolderWatcherFinishedPropagatingToNeighbors();
public slots:
    void initializeAndStart(const QString &videoSegmentsImporterFolderToWatch, const QString &videoSegmentsImporterFolderScratchSpace, const QString &videoSegmentsImporterFolderToMoveTo, const QString &neighborPropagationRemoteDestinationToUploadTo, const QString &neighborPropagationRemoteDestinationToMoveTo, const QString &neighborPropagationUserHostPathComboSftpArg, const QString &sftpProcessPath);

    void stopCleanlyOnceVideoSegmentNeighborPropagatationFinishes();
    void stopCleanlyOnceVideoSegmentNeighborPropagatationFinishesUnlessDc();
    void stopNow();
private slots:
    void handleDirectoryChanged();
    void handleSftpUploaderAndRenamerQueueStarted();
};

#endif // VIDEOSEGMENTSIMPORTERFOLDERWATCHER_H
