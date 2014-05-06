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
    enum SftpUploaderAndRenamerQueueStateEnum //perhaps all these states go in IBusinessObject
    {
        SftpUploaderAndRenamerQueueState_NotYetStarted = 0,
        SftpUploaderAndRenamerQueueState_Started = 1,
        SftpUploaderAndRenamerQueueState_StopWhenAllUploadsFinishSuccessfully = 2,
        SftpUploaderAndRenamerQueueState_StopWhenAllUploadsFinishSuccessfullyUnlessDc = 3,
        SftpUploaderAndRenamerQueueState_StopNow = 4 //enum value perhaps not necessary
    };
    explicit SftpUploaderAndRenamerQueue(QObject *parent = 0);
    ~SftpUploaderAndRenamerQueue();
private:
    QQueue<SftpUploaderAndRenamerQueueTimestampAndFilenameType> m_SegmentsQueuedForUpload;
    QTimer *m_FiveSecondRetryDequeueAndUploadTimer;
    QTimer *m_Sftp30secondKillerTimer;

    QString m_RemoteDestinationToUploadToWithSlashAppended;
    QString m_RemoteDestinationToMoveToWithSlashAppended;

    QProcess *m_SftpProcess;
    QString m_UserHostPathComboSftpArg;
    QString m_SftpProcessPath;
    QTextStream *m_SftpProcessTextStream;
    bool m_SftpIsReadyForCommands;
    bool m_SftpPutInProgressSoWatchForRenameCommandEcho;

    SftpUploaderAndRenamerQueueStateEnum m_SftpUploaderAndRenamerQueueState;

    void setTheStateEnumValue(SftpUploaderAndRenamerQueueStateEnum newSftpUploaderAndRenamerQueueState);
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
    void quitRequested();
    void sftpUploaderAndRenamerQueueStopped();
public slots:
    void startSftpUploaderAndRenamerQueue(const QString &remoteDestinationToUploadTo, const QString &remoteDestinationToMoveTo, const QString &userHostPathComboSftpArg, const QString &sftpProcessPath);
    void enqueueFileForUploadAndRename(SftpUploaderAndRenamerQueueTimestampAndFilenameType timestampAndFilenameToEnqueueForUpload);
    void tellStatus();

    void changeSftpUploaderAndRenamerQueueState(SftpUploaderAndRenamerQueue::SftpUploaderAndRenamerQueueStateEnum newSftpUploaderAndRenamerQueueState);
private slots:
    void startSftpProcessInBatchMode();
    void tryDequeueAndUploadSingleSegment();
    void handleSftpProcessStarted();
    void handleSftpProcessReadyReadStandardOut();
    void handleSftpProcessReadyReadStandardError();
    void killSftpIfStillRunning();
    void handleSftpProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // SFTPUPLOADERANDRENAMERQUEUE_H
