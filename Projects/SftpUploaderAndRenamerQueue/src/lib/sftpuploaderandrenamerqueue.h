#ifndef SFTPUPLOADERANDRENAMERQUEUE_H
#define SFTPUPLOADERANDRENAMERQUEUE_H

#include <QObject>
#include <QQueue>
#include <QProcess>

class QTextStream;
class QTimer;

typedef QPair<QString/*unixTimestamp*/,QString/*filename*/> SftpUploaderAndRenamerQueueTimestampAndFilenameType;

class SftpUploaderAndRenamerQueue : public QObject
{
    Q_OBJECT
public:
    explicit SftpUploaderAndRenamerQueue(QObject *parent = 0);
    ~SftpUploaderAndRenamerQueue();
private:
    QQueue<SftpUploaderAndRenamerQueueTimestampAndFilenameType> m_SegmentsQueuedForUpload;
    QTimer *m_FiveSecondRetryTimer;

    QString m_LocalPathWithSlashAppended;
    QString m_RemoteDestinationToUploadToWithSlashAppended;
    QString m_RemoteDestinationToMoveToWithSlashAppended;

    QProcess *m_SftpProcess;
    QString m_UserHostPathComboSftpArg;
    QString m_SftpProcessPath;
    QTextStream *m_SftpProcessTextStream;
    bool m_SftpIsReadyForCommands;
    bool m_SftpPutInProgressSoWatchForRenameCommandEcho;
    bool m_SftpWasToldToQuit;

    void stopSftpProcess();
    static inline QString appendSlashIfMissing(QString stringInput)
    {
        if(!stringInput.endsWith("/"))
        {
            return stringInput.append("/");
        }
        return stringInput;
    }
signals:
    void o(const QString &);
    void e(const QString &);
    void sftpUploaderAndRenamerQueueStarted();
    void statusGenerated(const QString &);
    void sftpUploaderAndRenamerQueueStopped();
public slots:
    void startSftpUploaderAndRenamerQueue(const QString &localPath, const QString &remoteDestinationToUploadTo, const QString &remoteDestinationToMoveTo, const QString &userHostPathComboSftpArg, const QString &sftpProcessPath);
    void enqueueFileForUploadAndRename(QPair<QString, QString> timestampAndFilenameToEnqueueForUpload);
    void tellStatus();
    void stopSftpUploaderAndRenamerQueue();
private slots:
    void startSftpProcessInBatchMode();
    void tryDequeueAndUploadSingleSegment();
    void handleSftpProcessStarted();
    void handleSftpProcessReadyReadStandardOut();
    void handleSftpProcessReadyReadStandardError();
    void handleSftpProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // SFTPUPLOADERANDRENAMERQUEUE_H
