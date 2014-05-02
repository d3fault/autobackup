#ifndef FFMPEGSEGMENTUPLOADER_H
#define FFMPEGSEGMENTUPLOADER_H

#include <QObject>
#include <QTextStream>
#include <QQueue>
#include <QPair>
#include <QProcess>
#include <QTimer>

class SftpUploaderAndRenamerQueue;

class QFile;
class QFileSystemWatcher;

class FfmpegSegmentUploader : public QObject
{
    Q_OBJECT
public:
    explicit FfmpegSegmentUploader(QObject *parent = 0);
private:
    QString m_FilenameOfSegmentsEntryList;
    QFile *m_FfmpegSegmentsEntryListFile;
    QFileSystemWatcher *m_FfmpegSegmentsEntryListFileWatcher;
    QTextStream m_FfmpegSegmentsEntryListFileTextStream;

    qint64 m_SegmentLengthSeconds;

    SftpUploaderAndRenamerQueue *m_SftpUploaderAndRenamerQueue;
signals:
    void d(const QString &);
    void stoppedUploadingFfmpegSegments();
public slots:
    void startUploadingSegmentsOnceFfmpegAddsThemToTheSegmentsEntryList(const QString &filenameOfSegmentsEntryList, qint64 segmentLengthSeconds, /*pass through args follow*/ const QString &localPath, const QString &remoteDestinationToUploadTo, const QString &remoteDestinationToMoveTo, const QString &userHostPathComboSftpArg, const QString &sftpProcessPath);
    void tellStatus();
    void stopUploadingFfmpegSegments();
private slots:
    void handleSftpUploaderAndRenamerQueueStarted();
    void handleSegmentsEntryListFileModified();
};

#endif // FFMPEGSEGMENTUPLOADER_H
