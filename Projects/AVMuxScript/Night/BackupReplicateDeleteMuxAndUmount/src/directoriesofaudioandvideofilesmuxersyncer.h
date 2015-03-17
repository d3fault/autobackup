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
    int probeDurationFromMediaFile(const QString &mediaFile);
    static bool timespansIntersect(qint64 timespan_0_StartTimeMs, qint64 timespan_0_DurationInMillseconds, qint64 timespan_1_StartTimeMs, qint64 timespan_1_DurationInMillseconds);
signals:
    void doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(bool success);
public slots:
    void muxAndSyncDirectoryOfAudioWithDirectoryOfVideo(const QDir &directoryOfAudioFiles, const QDir &directoryOfVideoFiles, const QDir &muxOutputDirectory);
};
struct AudioFileMeta
{
    AudioFileMeta(const QFileInfo &audioFileInfo, int durationInMilliseconds)
        : AudioFileInfo(audioFileInfo)
        , DurationInMillseconds(durationInMilliseconds)
    { }
    QFileInfo AudioFileInfo;
    int DurationInMillseconds;
};
struct VideoFileMetaAndIntersectingAudios
{
    VideoFileMetaAndIntersectingAudios(const QFileInfo &videoFileInfo, int durationInMillseconds)
        : VideoFileInfo(videoFileInfo)
        , DurationInMillseconds(durationInMillseconds)
    { }
    QFileInfo VideoFileInfo;
    int DurationInMillseconds;
    QMap<qint64 /* audio file start time in milliseconds*/, AudioFileMeta> IntersectingAudioFiles; //map just for sorting
};

#endif // DIRECTORIESOFAUDIOANDVIDEOFILESMUXERSYNCER_H
