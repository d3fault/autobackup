#ifndef DIRECTORIESOFAUDIOANDVIDEOFILESMUXERSYNCER_H
#define DIRECTORIESOFAUDIOANDVIDEOFILESMUXERSYNCER_H

#include <QObject>

#include <QDir>
#include <QMap>
#include <QScopedPointer>
#include <QTemporaryDir>
#include <QSharedPointer>
#include <QSet>

#define DirectoriesOfAudioAndVideoFilesMuxerSyncer_SPECIAL_STRING_REPLACE_BASE_NAME "%VIDEOBASENAME%"

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
    explicit DirectoriesOfAudioAndVideoFilesMuxerSyncer(QObject *parent = 0);
private:
    DirectoriesOfAudioAndVideoFilesMuxerSyncerMode::DirectoriesOfAudioAndVideoFilesMuxerSyncerModeEnum m_Mode;
    QList<QSharedPointer<VideoFileMetaAndIntersectingAudios> > m_VideoFileMetaAndIntersectingAudios;
    QSet<AudioFileMeta> m_AudioFileMetas;
    QScopedPointer<QListIterator<QSharedPointer<VideoFileMetaAndIntersectingAudios> > > m_VideoFilesIterator;
    QSharedPointer<VideoFileMetaAndIntersectingAudios> m_CurrentVideoFile;
    QScopedPointer<QTemporaryDir> m_TempDir;
    QStringList m_AdditionalFfmpegArgs;
    QDir m_MuxOutputDirectory; //unused when m_Mode == InteractivelyCalculateAudioDelaysMode
    QString m_CurrentTruncatedPreviewedVideoFileBeingAnalyzedForCorrectAudioDelay; //only used when m_Mode == InteractivelyCalculateAudioDelaysMode
    QString m_MuxToExtWithDot_OrEmptyStringIfToUseSrcVideoExt;
    qint64 m_AudioDelayMs;
    qint64 m_TruncateVideosToMsDuration_OrZeroToNotTruncate;
    QHash<QString /* video file name */, qint64 /* chosen audio delay ms */> m_InteractivelyCalculatedAudioDelays;
    bool m_UseCurrentAudioDelayForTheRestOfTheVideoFiles;
    QString m_AudioDelaysInputFile_OrEmptyStringIfNoneProvided;
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
    void muxAndSyncDirectoryOfAudioWithDirectoryOfVideo(DirectoriesOfAudioAndVideoFilesMuxerSyncerMode::DirectoriesOfAudioAndVideoFilesMuxerSyncerModeEnum mode, const QString &directoryOfAudioFiles, const QString &directoryOfVideoFiles, const QString &muxOutputDirectory, const QStringList &additionalFfmpegArgs = QStringList(), const QString &muxToExt_OrEmptyStringIfToUseSrcVideoExt = QString(), qint64 audioDelayMs = 0, qint64 truncateVideosToMsDuration_OrZeroToNotTruncate = 0, const QString &audioDelaysInputFile_OrEmptyStringIfNoneProvided = QString());
    void muxAndSyncDirectoryOfAudioWithDirectoryOfVideo(DirectoriesOfAudioAndVideoFilesMuxerSyncerMode::DirectoriesOfAudioAndVideoFilesMuxerSyncerModeEnum mode, const QDir &directoryOfAudioFiles, const QDir &directoryOfVideoFiles, const QDir &muxOutputDirectory, const QStringList &additionalFfmpegArgs = QStringList(), const QString &muxToExt_OrEmptyStringIfToUseSrcVideoExt = QString(), qint64 audioDelayMs = 0, qint64 truncateVideosToMsDuration_OrZeroToNotTruncate = 0, const QString &audioDelaysInputFile_OrEmptyStringIfNoneProvided = QString());

    //interactive mode
    void handleUserWantsToPreviewThisAudioDelayAgain();
    void handleUserWantsToUseCurrentAudioDelayMs();
    void handleUserWantsToUseCurrentAudioDelayMsForTheRestOfTheVideoFiles();
    void handleUserInputtedNewAudioDelay_SoGenerateAndPreviewIt(qint64 newAudioDelayMs);
    void handleAudioDelaysOutputFilePathChosen(const QString &audioDelaysOutputSaveFilaPath);
};

#endif // DIRECTORIESOFAUDIOANDVIDEOFILESMUXERSYNCER_H
