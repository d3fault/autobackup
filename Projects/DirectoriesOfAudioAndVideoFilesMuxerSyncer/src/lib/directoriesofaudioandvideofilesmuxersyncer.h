#ifndef DIRECTORIESOFAUDIOANDVIDEOFILESMUXERSYNCER_H
#define DIRECTORIESOFAUDIOANDVIDEOFILESMUXERSYNCER_H

#include <QObject>

#include <QDir>
#include <QMap>
#include <QScopedPointer>
#include <QTemporaryDir>
#include <QSharedPointer>
#include <QSet>

struct AudioFileMeta
{
    AudioFileMeta(const QFileInfo &audioFileInfo, int durationInMilliseconds)
        : AudioFileInfo(audioFileInfo)
        , DurationInMillseconds(durationInMilliseconds)
    { }
    QFileInfo AudioFileInfo;
    qint64 DurationInMillseconds;

    bool operator==(const AudioFileMeta &audioFileMeta) const;
};
inline uint qHash(const AudioFileMeta &key, uint seed)
{
    return qHash(key.AudioFileInfo.absoluteFilePath(), seed);
}
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
struct DirectoriesOfAudioAndVideoFilesMuxerSyncerMode
{
    enum DirectoriesOfAudioAndVideoFilesMuxerSyncerModeEnum
    {
          NormalMode
        , InteractivelyCalculateAudioDelaysMode
    };
};
class DirectoriesOfAudioAndVideoFilesMuxerSyncer : public QObject
{
    Q_OBJECT
public:
    explicit DirectoriesOfAudioAndVideoFilesMuxerSyncer(DirectoriesOfAudioAndVideoFilesMuxerSyncerMode::DirectoriesOfAudioAndVideoFilesMuxerSyncerModeEnum mode = DirectoriesOfAudioAndVideoFilesMuxerSyncerMode::NormalMode, QObject *parent = 0);
private:
    DirectoriesOfAudioAndVideoFilesMuxerSyncerMode::DirectoriesOfAudioAndVideoFilesMuxerSyncerModeEnum m_Mode;
    QList<QSharedPointer<VideoFileMetaAndIntersectingAudios> > m_VideoFileMetaAndIntersectingAudios;
    QSet<AudioFileMeta> m_AudioFileMetas;
    QScopedPointer<QListIterator<QSharedPointer<VideoFileMetaAndIntersectingAudios> > > m_VideoFilesIterator;
    QSharedPointer<VideoFileMetaAndIntersectingAudios> m_CurrentVideoFile;
    QScopedPointer<QTemporaryDir> m_TempDir;
    QDir m_MuxOutputDirectory;
    QString m_CurrentTruncatedPreviewedVideoFileBeingAnalyzedForCorrectAudioDelay; //only used when m_Mode == InteractivelyCalculateAudioDelaysMode
    qint64 m_AudioDelayMs;
    qint64 m_TruncateVideosToMsDuration_OrZeroToNotTruncate;
    QHash<QString /* video file name */, qint64 /* chosen audio delay ms */> m_InteractivelyCalculatedAudioDelays;
    bool m_UseCurrentAudioDelayForTheRestOfTheVideoFiles;
    QStringList m_VideoExtensions;
    QStringList m_AudioExtensions;

    void muxSyncNextVideoFileOrEmitDoneIfNoMore();
    void muxSyncCurrentVideoFile();
    bool showUserCurrentTruncatedVideoPreviewSoTheyCanAnalyzeAudioDelay();
    void useCurrentAudioDelayForCurrentVideo();
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
    void truncatedPreviewedVideoFileReadyForAudioDelayUserInput(qint64 currentAudioDelayMs);
    void audioDelaysFileCalculatedSoTellMeWhereToSaveIt();
    void doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(bool success);
public slots:
    void muxAndSyncDirectoryOfAudioWithDirectoryOfVideo(const QString &directoryOfAudioFiles, const QString &directoryOfVideoFiles, const QString &muxOutputDirectory, qint64 audioDelayMs = 0, qint64 truncateVideosToMsDuration_OrZeroToNotTruncate = 0);
    void muxAndSyncDirectoryOfAudioWithDirectoryOfVideo(const QDir &directoryOfAudioFiles, const QDir &directoryOfVideoFiles, const QDir &muxOutputDirectory, qint64 audioDelayMs = 0, qint64 truncateVideosToMsDuration_OrZeroToNotTruncate = 0);

    //interactive mode
    void handleUserWantsToPreviewThisAudioDelayAgain();
    void handleUserWantsToUseCurrentAudioDelayMs();
    void handleUserWantsToUseCurrentAudioDelayMsForTheRestOfTheVideoFiles();
    void handleUserInputtedNewAudioDelay_SoGenerateAndPreviewIt(qint64 newAudioDelayMs);
    void handleAudioDelaysOutputFilePathChosen(const QString &audioDelaysOutputSaveFilaPath);
};

#endif // DIRECTORIESOFAUDIOANDVIDEOFILESMUXERSYNCER_H
