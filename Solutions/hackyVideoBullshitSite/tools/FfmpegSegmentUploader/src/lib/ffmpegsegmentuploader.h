#ifndef FFMPEGSEGMENTUPLOADER_H
#define FFMPEGSEGMENTUPLOADER_H

#include <QObject>
#include <QTextStream>
#include <QQueue>
#include <QPair>
#include <QProcess>

class QFile;
class QFileSystemWatcher;

class FfmpegSegmentUploader : public QObject
{
    Q_OBJECT
public:
    explicit FfmpegSegmentUploader(QObject *parent = 0);
    ~FfmpegSegmentUploader();
private:
    QFile *m_FfmpegSegmentsEntryListFile;
    QFileSystemWatcher *m_FfmpegSegmentsEntryListFileWatcher;
    QTextStream m_FfmpegSegmentsEntryListFileTextStream;

    QString m_LocalPathWithSlashAppended;
    QString m_RemoteDestinationToUploadToWithSlashAppended;
    QString m_RemoteDestinationToMoveToWithSlashAppended;

    QString m_PreviousSegmentEntryTimestamp;
    QString m_PreviousSegmentEntry;
    QQueue<QPair<QString /*unixTimestamp*/, QString /*filename*/> > m_SegmentsQueuedForUpload;

    QProcess *m_SftpProcess;
    QString m_UserHostPathComboSftpArg;
    QString m_SftpProcessPath;
    QTextStream *m_SftpProcessTextStream;
    bool m_SftpIsReadyForCommands;
    bool m_SftpPutInProgressSoWatchForRenameCommandEcho;
    bool m_SftpWasToldToQuit;

    void startSftpProcessInBatchMode();
    static inline QString appendSlashIfMissing(QString stringInput)
    {
        if(!stringInput.endsWith("/"))
        {
            return stringInput.append("/");
        }
        return stringInput;
    }
signals:
    void d(const QString &);
    void stoppedUploadingFfmpegSegments();
public slots:
    void startUploadingSegmentsOnceFfmpegStartsEncodingTheNextOne(const QString &filenameOfSegmentsEntryList, const QString &localPath, const QString &remoteDestinationToUploadTo, const QString &remoteDestinationToMoveTo, const QString &userHostPathComboSftpArg, const QString &sftpProcessPath);
    void stopUploadingFfmpegSegments();
private slots:
    void handleSegmentsEntryListFileModified();
    void tryDequeueAndUploadSingleSegment();
    void handleSftpProcessReadyReadStandardOut();
    void handleSftpProcessReadyReadStandardError();
    void handleSftpProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // FFMPEGSEGMENTUPLOADER_H
