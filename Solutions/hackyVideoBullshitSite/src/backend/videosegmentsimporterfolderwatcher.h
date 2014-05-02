#ifndef VIDEOSEGMENTSIMPORTERFOLDERWATCHER_H
#define VIDEOSEGMENTSIMPORTERFOLDERWATCHER_H

#include <QObject>
#include <QDateTime>
#include <QDir>

class QFileSystemWatcher;
class SftpUploaderAndRenamerQueue;

class VideoSegmentsImporterFolderWatcher : public QObject
{
    Q_OBJECT
public:
    explicit VideoSegmentsImporterFolderWatcher(QObject *parent = 0);
private:
    QFileSystemWatcher *m_DirectoryWatcher;
    QString m_VideoSegmentsImporterFolderToWatchWithSlashAppended;
    QDir m_VideoSegmentsImporterFolderScratchSpace;
    QString m_VideoSegmentsImporterFolderToMoveToWithSlashAppended;

    int m_CurrentYearFolder;
    int m_CurrentDayOfYearFolder;

    inline QString appendSlashIfNeeded(QString inputString) { return inputString.endsWith("/") ? inputString : inputString.append("/"); } //always easier than a pri include

    SftpUploaderAndRenamerQueue *m_SftpUploaderAndRenamerQueue;
signals:
    void o(const QString &);
    void e(const QString &);
public slots:
    void initializeAndStart(const QString &videoSegmentsImporterFolderToWatch, const QString &videoSegmentsImporterFolderScratchSpace, const QString &videoSegmentsImporterFolderToMoveTo, const QString &neighborPropagationRemoteDestinationToUploadTo, const QString &neighborPropagationRemoteDestinationToMoveTo, const QString &neighborPropagationUserHostPathComboSftpArg, const QString &sftpProcessPath);
private slots:
    void handleDirectoryChanged(const QString &path);
    void handleSftpUploaderAndRenamerQueueStarted();
};

#endif // VIDEOSEGMENTSIMPORTERFOLDERWATCHER_H
