#ifndef DIRECTORIESOFAUDIOANDVIDEOFILESMUXERSYNCER_H
#define DIRECTORIESOFAUDIOANDVIDEOFILESMUXERSYNCER_H

#include <QObject>

#include <QDir>
#include <QMap>

class DirectoriesOfAudioAndVideoFilesMuxerSyncer : public QObject
{
    Q_OBJECT
public:
    explicit DirectoriesOfAudioAndVideoFilesMuxerSyncer(QObject *parent = 0);
private:
    QStringList m_VideoExtensions;
    QStringList m_AudioExtensions;

    bool isVideoFile(const QFileInfo &fileToCheck);
    bool isAudioFile(const QFileInfo &fileToCheck);
    qint64 probeDurationFromMediaFile(const QString &mediaFile);
    static bool timespansIntersect(qint64 timespan_0_StartTimeMs, qint64 timespan_0_DurationInMillseconds, qint64 timespan_1_StartTimeMs, qint64 timespan_1_DurationInMillseconds);
    static inline QString appendSlashIfNeeded(const QString &inputString)
    {
        if(inputString.endsWith("/"))
            return inputString;
        return inputString + "/";
    }
signals:
    void o(const QString &msg);
    void e(const QString &msg);
    void doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(bool success);
public slots:
    void muxAndSyncDirectoryOfAudioWithDirectoryOfVideo(const QString &directoryOfAudioFiles, const QString &directoryOfVideoFiles, const QString &muxOutputDirectory, qint64 audioDelayMs = 0, qint64 truncateVideosToMsDuration_OrZeroToNotTruncate = 0);
    void muxAndSyncDirectoryOfAudioWithDirectoryOfVideo(const QDir &directoryOfAudioFiles, const QDir &directoryOfVideoFiles, const QDir &muxOutputDirectory, qint64 audioDelayMs = 0, qint64 truncateVideosToMsDuration_OrZeroToNotTruncate = 0);
};
struct AudioFileMeta
{
    AudioFileMeta(const QFileInfo &audioFileInfo, int durationInMilliseconds)
        : AudioFileInfo(audioFileInfo)
        , DurationInMillseconds(durationInMilliseconds)
    { }
    QFileInfo AudioFileInfo;
    qint64 DurationInMillseconds;
};
struct VideoFileMetaAndIntersectingAudios
{
    VideoFileMetaAndIntersectingAudios(const QFileInfo &videoFileInfo, int durationInMillseconds)
        : VideoFileInfo(videoFileInfo)
        , DurationInMillseconds(durationInMillseconds)
    { }
    QFileInfo VideoFileInfo;
    qint64 DurationInMillseconds;
    QMap<qint64 /* audio file start time in milliseconds*/, AudioFileMeta> IntersectingAudioFiles; //map just for sorting
};

#endif // DIRECTORIESOFAUDIOANDVIDEOFILESMUXERSYNCER_H
