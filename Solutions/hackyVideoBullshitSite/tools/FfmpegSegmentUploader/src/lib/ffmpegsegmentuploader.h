#ifndef FFMPEGSEGMENTUPLOADER_H
#define FFMPEGSEGMENTUPLOADER_H

#include <QObject>
#include <QTextStream>
#include <QQueue>
#include <QPair>
#include <QProcess>
#include <QTimer>

#define FFMPEG_COMMAND_SEGMENT_ENTRY_LIST_FILE_REPLACEMENT_STRING "%SEGMENT_ENTRY_LIST_FILE%"
#define FFMPEG_COMMAND_SEGMENT_LENGTH_STRING "%SEGMENT_LENGTH_SECONDS%"

#define DEFAULT_ANDOR_D3FAULT_FFMPEG_COMMAND_ZOMG_ROFL_PUN_OR_WAIT_NO_IDK_JUST_LOL_THO "/usr/local/bin/ffmpeg -f alsa -i hw:0 -f video4linux2 -s 720x480 -channel 1 -i /dev/video0 -acodec opus -b:a 32k -ac 1 -b:v 110k -vcodec theora -r 10 -map 0 -map 1 -vf crop=720:452:0:17 -f segment -segment_time " FFMPEG_COMMAND_SEGMENT_LENGTH_STRING " -segment_list_size 999999999 -segment_wrap 999999999 -segment_list " FFMPEG_COMMAND_SEGMENT_ENTRY_LIST_FILE_REPLACEMENT_STRING " -reset_timestamps 1 videoSegment-%d.ogg"

class SftpUploaderAndRenamerQueue;

class QFile;
class QFileSystemWatcher;
class QProcess;

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

    QString m_FfmpegCommand;
    QProcess *m_FfmpegProcess;

    SftpUploaderAndRenamerQueue *m_SftpUploaderAndRenamerQueue;
signals:
    void o(const QString &);
    void e(const QString &);
    void stoppedUploadingFfmpegSegments();
public slots:
    void startUploadingSegmentsOnceFfmpegAddsThemToTheSegmentsEntryList(const QString &filenameOfSegmentsEntryList, qint64 segmentLengthSeconds, const QString &ffmpegCommand, const QString &localPath, /*pass through args follow*/ const QString &remoteDestinationToUploadTo, const QString &remoteDestinationToMoveTo, const QString &userHostPathComboSftpArg, const QString &sftpProcessPath);
    void tellStatus();
    void stopUploadingFfmpegSegments();
private slots:
    void handleSftpUploaderAndRenamerQueueStarted();
    void handleFfmpegProcessStarted();
    void handleFfmpegProecssStdErr();
    void handleSegmentsEntryListFileModified();
    void handleFfmpegProecssFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // FFMPEGSEGMENTUPLOADER_H
