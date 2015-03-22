#include "directoriesofaudioandvideofilesmuxersyncer.h"

#include <QDirIterator>
#include <QProcess>
#include <QTextStream>
#include <QDateTime>
#include <QMapIterator>
#include <QTemporaryDir>
#include <QSetIterator>
#include <QSettings>

#define DirectoriesOfAudioAndVideoFilesMuxerSyncer_VIDEO_PREVIEWER "mplayer" /*TODOoptional: allow specifying via command line*/

#define DUMP_PROCESS_OUTPUT(process) \
emit o(process.readAllStandardOutput()); \
emit e(process.readAllStandardError());

static QString stringListToString(const QStringList &input)
{
    QString ret;
    bool first = true;
    Q_FOREACH(const QString &lel, input)
    {
        ret.append((first ? QString("") : QString(", ")) + lel);
        first = false;
    }
    return ret;
}

#define RUN_FFMPEG(ffmpegArgz) \
ffmpegProcess.start("ffmpeg", ffmpegArgz, QIODevice::ReadOnly); \
if(!ffmpegProcess.waitForStarted(-1)) \
{ \
    DUMP_PROCESS_OUTPUT(ffmpegProcess) \
    emit e("ffmpeg failed to start with these args: " + stringListToString(ffmpegArgz)); \
    emit doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(false); \
    return; \
} \
if(!ffmpegProcess.waitForFinished(-1)) \
{ \
    DUMP_PROCESS_OUTPUT(ffmpegProcess) \
    emit e("ffmpeg failed to finish with these args: " + stringListToString(ffmpegArgz)); \
    emit doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(false); \
    return; \
} \
DUMP_PROCESS_OUTPUT(ffmpegProcess) \
if(ffmpegProcess.exitCode() != 0 || ffmpegProcess.exitStatus() != QProcess::NormalExit) \
{ \
    emit e("ffmpeg process exitted abnormally with exit code: " + QString::number(ffmpegProcess.exitCode()) + " -- with args: " + stringListToString(ffmpegArgz)); \
    emit doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(false); \
    return; \
}

#define APPEND_SILENCE_PREPEND_ARGS_TO_FFMPEG_ARGSLIST(theArgs, silenceSeconds) \
QString filterComplex("'aevalsrc=0:d=" + QString::number(silenceSeconds, 'f') + "[slug];[slug][0]concat=n=2:v=0:a=1[out]'"); \
theArgs << "-filter_complex" << filterComplex << "-map" << "[out]";

#define EMIT_ERROR_AND_RETURN_IF_RENAME_FAILS(renameFrom, renameTo) \
if(!QFile::rename(renameFrom, renameTo)) \
{ \
    emit e("failed to rename: '" + renameFrom + "' to '" + renameTo + "'"); \
    emit doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(false); \
    return; \
}

#define EMIT_ERROR_AND_RETURN_IF_REMOVE_FAILS(toRemove) \
if(!QFile::remove(toRemove)) \
{ \
    emit e("failed to remove: " + toRemove); \
    emit doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(false); \
    return; \
}

#define CONCAT_ONTO_OR_BECOME_AUDIO_FILE(toConcatOntoOrBecome, audioSegment) \
if(QFile::exists(toConcatOntoOrBecome)) \
{ \
    /*concat. TO DOnereq(separate ffmpeg calls): -t must account for the duration we're concatting onto (but this isn't the case for "create", obviously). this also applies to concatting silence*/ \
    /*ffmpeg -i firstInput.flac -i secondInput.flac -filter_complex '[0:0][1:0]concat=n=2:v=0:a=1[out]' -map '[out]' output.flac*/ \
    QString concatenatedOutput = tempDir_WithSlashAppended + "concatenatedTemp.flac"; \
    QStringList ffmpegAudioConcatArgs; \
    ffmpegAudioConcatArgs << "-i" << toConcatOntoOrBecome << "-i" << audioSegment << "-filter_complex" << "[0:0][1:0]concat=n=2:v=0:a=1[out]" << "-map" << "[out]" << concatenatedOutput; \
    RUN_FFMPEG(ffmpegAudioConcatArgs) \
    /*now remove audioFileBeingBuilt and audioSegment, then rename concatenatedOutput to audioFileBeingBuilt*/ \
    EMIT_ERROR_AND_RETURN_IF_REMOVE_FAILS(audioSegment) \
    EMIT_ERROR_AND_RETURN_IF_REMOVE_FAILS(toConcatOntoOrBecome) \
    EMIT_ERROR_AND_RETURN_IF_RENAME_FAILS(concatenatedOutput, toConcatOntoOrBecome) \
} \
else \
{ \
    /*create. just 'become' it*/ \
    EMIT_ERROR_AND_RETURN_IF_RENAME_FAILS(audioSegment, toConcatOntoOrBecome) \
}

//this version considers video to be the more important of the two. I thought about having black screens when there's audio only, and even single stream audio only files, but considering it's a visual website the *videos* will be posted on, I'm going to make hasVideoAtThisMomentInTime (pseudo, obviously not gonna code it like that (because there would literally be infinite checks. LITERALLY)) the threshold for output. I'm going to try my best to always have audio whenever I have video, to minimize the silence
//can't decide if this should be async or not! my use of it in import script is kinda making it sync, fuckit
DirectoriesOfAudioAndVideoFilesMuxerSyncer::DirectoriesOfAudioAndVideoFilesMuxerSyncer(DirectoriesOfAudioAndVideoFilesMuxerSyncerMode::DirectoriesOfAudioAndVideoFilesMuxerSyncerModeEnum mode, QObject *parent)
    : QObject(parent)
    , m_Mode(mode)
{
    m_VideoExtensions.append("m4v");
    m_VideoExtensions.append("3gp");
    m_VideoExtensions.append("3gpp");
    m_VideoExtensions.append("mp4");
    m_VideoExtensions.append("mov");
    m_VideoExtensions.append("wmv");
    m_VideoExtensions.append("3g2");
    m_VideoExtensions.append("avi");
    m_VideoExtensions.append("mpg");
    m_VideoExtensions.append("mpeg");
    m_VideoExtensions.append("ogv");
    m_VideoExtensions.append("mkv");
    m_VideoExtensions.append("webm");
    m_VideoExtensions.append("flv");

    //m_VideoExtensions.append("h264"); //TO DOnereq(muxing to mkv during copy xD): special params since no container


    m_AudioExtensions.append("m4a");
    m_AudioExtensions.append("wav");
    m_AudioExtensions.append("mp3");
    m_AudioExtensions.append("wma");
    //m_AudioExtensions.append("ogg"); --bleh could be av or just v or just a (you bastards. TODOoptional: ffprobe dat shiz and look at teh streamz, or require a and v folders to be different)
    m_AudioExtensions.append("flac");
    m_AudioExtensions.append("opus");
}
void DirectoriesOfAudioAndVideoFilesMuxerSyncer::muxSyncNextVideoFileOrEmitDoneIfNoMore()
{
    if(m_VideoFilesIterator->hasNext())
    {
        m_CurrentVideoFile = m_VideoFilesIterator->next();
        muxSyncCurrentVideoFile();
        return;
    }

    if(m_Mode == DirectoriesOfAudioAndVideoFilesMuxerSyncerMode::NormalMode)
    {
        emit o("done muxing and syncing directory of audio with directory of video -- everything OK");
        emit doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(true);
        return;
    }
    else // m_Mode == DirectoriesOfAudioAndVideoFilesMuxerSyncerMode::InteractivelyCalculateAudioDelaysMode
    {
        emit audioDelaysFileCalculatedSoTellMeWhereToSaveIt(); //yea i could have gotten it in my constructor or even as an arg to muxAndSyncDirectoryOfAudioWithDirectoryOfVideo... but I'm thinking ahead: it's better in a GUI situation to ask for the filename to save to at the very end
        return;
    }
}
void DirectoriesOfAudioAndVideoFilesMuxerSyncer::muxSyncCurrentVideoFile()
{
    //[re-]find intersecting audio segments
    m_CurrentVideoFile->IntersectingAudioFiles.clear();
    QSetIterator<AudioFileMeta> audioFileMetaIterator(m_AudioFileMetas);
    while(audioFileMetaIterator.hasNext())
    {
        const AudioFileMeta &currentAudioFileMeta = audioFileMetaIterator.next();
        if(!m_AudioDelaysInputFile_OrEmptyStringIfNoneProvided.isEmpty())
        {
            QSettings audioDelaysInputFile(m_AudioDelaysInputFile_OrEmptyStringIfNoneProvided, QSettings::IniFormat); //TODOoptimization: might be better to not re-instantiate this over and over, but actually i think qsettings might handle it intelligently (unsure, dgaf atm)
            m_AudioDelayMs = audioDelaysInputFile.value(m_CurrentVideoFile->VideoFileInfo.fileName(), 0);
        }
        qint64 startTimestampOfAudioMs = currentAudioFileMeta.AudioFileInfo.lastModified().toMSecsSinceEpoch() + m_AudioDelayMs; //NOTE: it's crucial that any other use of "start timestamp of audio" does not use the last modified timestamp in AudioFileMeta.AudioFileInfo [alone]. The Audio Delay must be incorporated, and the easiest way to ensure it is is to simply use the key from the map IntersectingAudioFiles
        qint64 durationOfAudioFileInMs = currentAudioFileMeta.DurationInMillseconds;
        if(timespansIntersect(m_CurrentVideoFile->VideoFileInfo.lastModified().toMSecsSinceEpoch(), m_CurrentVideoFile->DurationInMillseconds, startTimestampOfAudioMs, durationOfAudioFileInMs))
        {
            if(m_CurrentVideoFile->IntersectingAudioFiles.contains(startTimestampOfAudioMs))
            {
                emit e("two audio files found with same start time"); //for now we don't check the more likely case of two audio files intersecting... but if they're the same start time that's different (and will probably never happen)
                emit doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(false);
                return;
            }
            m_CurrentVideoFile->IntersectingAudioFiles.insert(startTimestampOfAudioMs, AudioFileMeta(currentAudioFileMeta.AudioFileInfo, durationOfAudioFileInMs)); //reconstruction of AudioFileMeta intentional to ensure deep copy, just in case (i don't think it's necessary, but eh doesn't hurt either)
        }
    }

    //now dooeeeet
    m_TempDir.reset(new QTemporaryDir());
    if(!m_TempDir->isValid())
    {
        emit e("failed to get temp dir:" + m_TempDir->path());
        emit doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(false);
        return;
    }
    QString tempDir_WithSlashAppended = appendSlashIfNeeded(m_TempDir->path());
    qint64 videoStartTimestampMSecs = m_CurrentVideoFile->VideoFileInfo.lastModified().toMSecsSinceEpoch();
    qint64 videoEndTimestampMs = (videoStartTimestampMSecs + m_CurrentVideoFile->DurationInMillseconds);
    QProcess ffmpegProcess;
    QStringList ffmpegArgs;
    ffmpegArgs << "-i" << m_CurrentVideoFile->VideoFileInfo.absoluteFilePath();
    QStringList mapAudio;
    if(m_CurrentVideoFile->IntersectingAudioFiles.isEmpty())
    {
        //don't have audio stream
        ffmpegArgs << "-an";
    }
    else
    {
        //have audio stream
        mapAudio << "-map" << "1";

        QMapIterator<qint64 /* start timestamp of audio */, AudioFileMeta> currentAudioFile(m_CurrentVideoFile->IntersectingAudioFiles);
        QString audioFileBeingBuilt(tempDir_WithSlashAppended + m_CurrentVideoFile->VideoFileInfo.completeBaseName() + ".flac"); //maybe use hq opus at temporary format instead of flac? oh one hand, i don't want to decode, encode, decode, encode (using flac means i decode -> encode). on the other hand, i might run out of hdd space if i've been recording all day! rofl stupid problems, little wiggle room. i calculated it, ~24 hrs of .flac would take ~4gb, so I'm aight
        //qint64 audioFileBeingBuiltDurationMs = 0; //TODOoptional: at the end of iterating, Q_ASSERT this is same length as video (actually it can be shorter, ffmpeg doesn't care about audio tracks ending too soon.... so nvm i guess :-/)
        qint64 audioFileBeingBuilt_HaveAudioUpToTimestampMs = videoStartTimestampMSecs; //this is kinda like 'duration' in earlier/ifdef'd out attempt, but is a unix timestamp instead
        while(currentAudioFile.hasNext())
        {
            currentAudioFile.next();
            QStringList ffmpegAudioArgs;

            //-ss (only for the first, and only if needed)
            //-t (if needed)
            //prepend silence (if needed)

            //all three of these affect each other (fml)
            //which should i calculate first? it quickly becomes spaghetti code. KISS (easier said than done)
            //i think i shouldn't prepend silence before onto the next audio file, but append silence onto the audioFileBeingBuilt and then consider every ms up to the next audio file to already be accounted for. that's just KISS imo (but is arguably less elegant (fuckit)) <----- YES.
            //TO DOnereq: there are multiple places where audioFileBeingBuilt will get created for the FIRST time (if -ss is used, we don't prepend silence (audioFileBeingBuilt is first created for first audio file). if -ss is not used, we [probably] do prepend silence (audioFileBeingBuilt is first created when prepending silence). and there's a rare case where we neither prepend silence nor use -ss (the audio and video start at exactly the same ms, in which case audioFileBeingBuilt is first created for the first audio file) -- the alternative is to use the "prepend silence onto next audio file" solution. TO DOnereq(related): we DON'T need to add silence if there are no more audio files. i think my while(hasNext) iterator already does this for me actually

            qint64 audioStartTimestampMs = currentAudioFile.key();
            qint64 audioEndTimestampMs = audioStartTimestampMs + currentAudioFile.value().DurationInMillseconds;

            //append silence onto audioFileBeingBuilt before proceeding, simply for sanity in calculations
            if(audioFileBeingBuilt_HaveAudioUpToTimestampMs < audioStartTimestampMs) //no -ss is implied/deduced via this calculation
            {
                //need to put silence onto audioFileBeingBuilt until audioStartTimestampMs
                qint64 silenceMs = audioStartTimestampMs - audioFileBeingBuilt_HaveAudioUpToTimestampMs;
                double silence_Seconds = static_cast<double>(silenceMs) / 1000.0;

                //make silence
                QString audioSilence = tempDir_WithSlashAppended + "tempAudioSegmentAboutToBecomeAudioFileBeingBuiltOrConcatenatedOntoAudioFileBeingBuilt.flac";
                QStringList audioSilenceArgs;
                QString silenceFilter("aevalsrc=0:d=" + QString::number(silence_Seconds, 'f'));
                audioSilenceArgs << "-f" << "lavfi" << "-i" << silenceFilter << "-ac" << "1" << audioSilence;
                RUN_FFMPEG(audioSilenceArgs)

                //concat onto or become audioFileBeingBuilt
                CONCAT_ONTO_OR_BECOME_AUDIO_FILE(audioFileBeingBuilt, audioSilence)

                audioFileBeingBuilt_HaveAudioUpToTimestampMs += silenceMs;
            }

            //we know at this point that audioFileBeingBuilt_HaveAudioUpToTimestampMs points to audioStartTimestampMs (the resulting audio file might be off by a few ms, but whatever close enough for now). ACTUALLY, audioFileBeingBuilt_HaveAudioUpToTimestampMs might be pointing at videoStartTimestampMSecs if the audio started before video (-ss needed)

            //-t
            qint64 t_Ms = 0;
            if(audioEndTimestampMs > videoEndTimestampMs)
            {
                //audio extended past video, use -t
                t_Ms = videoEndTimestampMs - audioFileBeingBuilt_HaveAudioUpToTimestampMs; //-ss offset sidestepped/accounted-for
                audioFileBeingBuilt_HaveAudioUpToTimestampMs += t_Ms; //random: i think whenever we have -t, we no longer even need "haveAudioUpTo", because it is the last audio file xD
            }
            else
            {
                audioFileBeingBuilt_HaveAudioUpToTimestampMs += (audioEndTimestampMs-audioFileBeingBuilt_HaveAudioUpToTimestampMs); //-ss offset sidestepped/accounted-for
            }

            //-ss
            if(audioStartTimestampMs < videoStartTimestampMSecs)
            {
                //audio started before video, use -ss. ONLY THE FIRST AUDIO FILE WILL GET HERE.
                qint64 ss_Ms = videoStartTimestampMSecs-audioStartTimestampMs;
                double ss_Seconds = static_cast<double>(ss_Ms) / 1000.0;
                ffmpegAudioArgs << "-ss" << QString::number(ss_Seconds, 'f');
            }


            ffmpegAudioArgs << "-i" << currentAudioFile.value().AudioFileInfo.absoluteFilePath(); //-i must come after -ss
            if(t_Ms != 0)
            {
                double t_Seconds = static_cast<double>(t_Ms) / 1000.0;
                ffmpegAudioArgs << "-t" << QString::number(t_Seconds, 'f');
            }
            ffmpegAudioArgs << "-ac" << "1"; //output to 1 channel audio, despite our source having 2 (if I change this to two, my silence generator needs to also change to 2 (probably)) -- this must come after -i

            //make audio segment (possibly a noop if -t and -ss weren't used, in which case all it did was store into flac)
            QString audioSegment = tempDir_WithSlashAppended + "tempAudioSegmentAboutToBecomeAudioFileBeingBuiltOrConcatenatedOntoAudioFileBeingBuilt.flac";
            ffmpegAudioArgs << audioSegment;
            RUN_FFMPEG(ffmpegAudioArgs)

            //concat onto or become audioFileBeingBuilt
            CONCAT_ONTO_OR_BECOME_AUDIO_FILE(audioFileBeingBuilt, audioSegment)
        }
        QStringList muxOutputAudioCodec;
        if(m_Mode == DirectoriesOfAudioAndVideoFilesMuxerSyncerMode::NormalMode)
        {
            muxOutputAudioCodec << "-acodec" << "opus" << "-b:a" << "32k";
        }
        else //m_Mode == DirectoriesOfAudioAndVideoFilesMuxerSyncerMode::InteractivelyCalculateAudioDelaysMode
        {
            muxOutputAudioCodec << "-acodec" << "copy"; //it's already flac
        }
        ffmpegArgs << "-i" << audioFileBeingBuilt << muxOutputAudioCodec << "-ac" <<  "1";
    }
    //mux -- TODOreq: lutyuv brightness? being outside maybe not necessary (idfk) -- also: noir for night time shenigans (a realtime preview would come in handy too)!
    QString muxTargetDirectory_WithSlashAppended = (m_Mode == DirectoriesOfAudioAndVideoFilesMuxerSyncerMode::NormalMode ? (m_MuxOutputDirectory.absolutePath() + QDir::separator()) : (appendSlashIfNeeded(m_TempDir->path())));
    QStringList muxOutputFormat;
    QString muxOutputAbsoluteFilePath = muxTargetDirectory_WithSlashAppended + m_CurrentVideoFile->VideoFileInfo.completeBaseName();
    QStringList muxOutputVideoCodec;
    if(m_Mode == DirectoriesOfAudioAndVideoFilesMuxerSyncerMode::NormalMode)
    {
        muxOutputFormat << "segment" << "-segment_time" << "180" << "-segment_list_size" << "999999999" << "-segment_wrap" << "999999999" << "-segment_list" << QString(muxTargetDirectory_WithSlashAppended + m_CurrentVideoFile->VideoFileInfo.completeBaseName() + "-segmentEntryList.txt") << "-reset_timestamps" << "1"; //TODOreq: since i'm now going to be calling this app using a QProcess from my import script, I should move this segment format crap to there and introduce an --pass-ffmpeg-arg.... arg... to this app. Maybe I should require them to specify an extension (not to be confused with 'format', since 'mkv' is not a valid ffmpeg format, YET FFMPEG CAN GUESS MATROSKA BASED ON THE .MKV EXTENSION *@#(*@#&(* ) for the muxed files (and continue using the video file's complete base name for the prefix). There's a lot of flexibility here (allowing them to provide a template for an output filename seems... confusing... since there are multiple output filenames (best to just stick with video base name imo)), but this app is pretty useless to most others with the format hardcoded to segment@3mins (not that it'd be hard to change, but most wouldn't want to (don't blame em))
        muxOutputAbsoluteFilePath += "-%d.ogg";
        muxOutputVideoCodec << "-s" << "720x480" << "-b:v" << "275k" << "-vcodec" << "theora" << "-r" << "10";
    }
    else //m_Mode == DirectoriesOfAudioAndVideoFilesMuxerSyncerMode::InteractivelyCalculateAudioDelaysMode
    {
        muxOutputFormat << "matroska";
        muxOutputAbsoluteFilePath += ".mkv";
        muxOutputVideoCodec << "-vcodec" << "copy";
    }
    ffmpegArgs << muxOutputVideoCodec << "-f" << muxOutputFormat << "-map" << "0" << mapAudio;
    if(m_TruncateVideosToMsDuration_OrZeroToNotTruncate > 0)
    {
        double truncateVideoToSecondsDuration = static_cast<double>(m_TruncateVideosToMsDuration_OrZeroToNotTruncate) / 1000.0;
        ffmpegArgs << "-t" << QString::number(truncateVideoToSecondsDuration, 'f');
    }
    ffmpegArgs << muxOutputAbsoluteFilePath;
    RUN_FFMPEG(ffmpegArgs)
    emit o("muxed video using " + QString::number(m_CurrentVideoFile->IntersectingAudioFiles.size()) + " audio files");
    if(m_Mode == DirectoriesOfAudioAndVideoFilesMuxerSyncerMode::NormalMode)
    {
        muxSyncNextVideoFileOrEmitDoneIfNoMore();
        return;
    }
    else //m_Mode == DirectoriesOfAudioAndVideoFilesMuxerSyncerMode::InteractivelyCalculateAudioDelaysMode
    {
        if(!m_UseCurrentAudioDelayForTheRestOfTheVideoFiles)
        {
            m_CurrentTruncatedPreviewedVideoFileBeingAnalyzedForCorrectAudioDelay = muxOutputAbsoluteFilePath;
            if(!showUserCurrentTruncatedVideoPreviewSoTheyCanAnalyzeAudioDelay())
                return;
            emit truncatedPreviewedVideoFileReadyForAudioDelayUserInput(m_AudioDelayMs);
            return;
        }
        else
        {
            useCurrentAudioDelayForCurrentVideo();
            muxSyncNextVideoFileOrEmitDoneIfNoMore();
            return;
        }
    }
}
bool DirectoriesOfAudioAndVideoFilesMuxerSyncer::showUserCurrentTruncatedVideoPreviewSoTheyCanAnalyzeAudioDelay()
{
    QProcess videoPreviewProcess;
    QStringList videoPreviewArgs;
    videoPreviewArgs << m_CurrentTruncatedPreviewedVideoFileBeingAnalyzedForCorrectAudioDelay;
    videoPreviewProcess.start(DirectoriesOfAudioAndVideoFilesMuxerSyncer_VIDEO_PREVIEWER, videoPreviewArgs, QIODevice::ReadOnly);
    if(!videoPreviewProcess.waitForStarted(-1))
    {
        emit e("failed to start: " DirectoriesOfAudioAndVideoFilesMuxerSyncer_VIDEO_PREVIEWER);
        emit doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(false);
        return false;
    }
    if(!videoPreviewProcess.waitForFinished(-1))
    {
        emit e("failed to finish: " DirectoriesOfAudioAndVideoFilesMuxerSyncer_VIDEO_PREVIEWER);
        emit doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(false);
        return false;
    }
    if(videoPreviewProcess.exitCode() != 0 || videoPreviewProcess.exitStatus() != QProcess::NormalExit)
    {
        emit e(DirectoriesOfAudioAndVideoFilesMuxerSyncer_VIDEO_PREVIEWER " finished abnormally with exit code: " + QString::number(videoPreviewProcess.exitCode()));
        return false;
    }
    return true;
}
void DirectoriesOfAudioAndVideoFilesMuxerSyncer::useCurrentAudioDelayForCurrentVideo()
{
    if(m_AudioDelayMs != 0) //no need to store 0, since that's what it initializes to. TO DOnereq: when iterating the "audio delay ms from file" and encountering a video file without an entry in that file, don't use "previous audio delay [from audio delay file]" but instead use 0. if, however, the previous audio delay was typed in during this session, we should still use that audio delay as the default for the current file. ahh this is easier than i thought. for a moment i thought i was allowing interactive mode at the same time as supplying an audio delays input... which would make things a bit more complicated (still doable but eh)
    {
        m_InteractivelyCalculatedAudioDelays.insert(m_CurrentVideoFile->VideoFileInfo.fileName(), m_AudioDelayMs);
    }
}
bool DirectoriesOfAudioAndVideoFilesMuxerSyncer::isVideoFile(const QFileInfo &fileToCheck)
{
    if(!fileToCheck.isFile())
        return  false;
    QString extensionLowercase = fileToCheck.suffix().toLower();
    if(m_VideoExtensions.contains(extensionLowercase))
    {
        return true;
    }
    return false;
}
bool DirectoriesOfAudioAndVideoFilesMuxerSyncer::isAudioFile(const QFileInfo &fileToCheck)
{
    if(!fileToCheck.isFile())
        return  false;
    QString extensionLowercase = fileToCheck.suffix().toLower();
    if(m_AudioExtensions.contains(extensionLowercase))
    {
        return true;
    }
    return false;
}
qint64 DirectoriesOfAudioAndVideoFilesMuxerSyncer::probeDurationFromMediaFile(const QString &mediaFile)
{
    //BEGIN CODE JACKED FROM SAMPLES PIANO KEYBOARD
    QProcess ffProbeProcess(this);
    QStringList ffProbeArgs;
    ffProbeArgs << "-show_format" << mediaFile;
    ffProbeProcess.start("ffprobe", ffProbeArgs, QIODevice::ReadOnly);
    if(!ffProbeProcess.waitForStarted())
    {
        emit e(ffProbeProcess.errorString());
        emit e("ffprobe failed to start");
        return -1;
    }
    if(!ffProbeProcess.waitForFinished(-1))
    {
        emit e(ffProbeProcess.errorString());
        emit e("ffprobe failed to finish");
        return -1;
    }
    if(ffProbeProcess.exitCode() != 0 || ffProbeProcess.exitStatus() != QProcess::NormalExit)
    {
        emit e(ffProbeProcess.errorString());
        emit e(ffProbeProcess.readAll());
        emit e("ffprobe exitted abnormally");
        return -1;
    }

    QTextStream ffProbeTextStream(&ffProbeProcess);
    QString currentFfProbeLine;
    bool inFormatSection = false;
    while(!ffProbeTextStream.atEnd())
    {
        currentFfProbeLine = ffProbeTextStream.readLine().trimmed();
        if(inFormatSection)
        {
            if(currentFfProbeLine == "[/FORMAT]")
            {
                inFormatSection = false;
            }
            else if(currentFfProbeLine.startsWith("duration="))
            {
                if(currentFfProbeLine.contains("N/A"))
                {
                    return -1;
                }
                QStringList splitAtEqualSign = currentFfProbeLine.split("=", QString::SkipEmptyParts);
                if(splitAtEqualSign.size() != 2)
                {
                    return -1;
                }
                bool ok = false;
                QString rightHalf = splitAtEqualSign.at(1);
                double parsedDurationSeconds = rightHalf.toDouble(&ok);
                if(!ok)
                {
                    return -1;
                }
                return (parsedDurationSeconds*1000);
            }
        }
        else if(currentFfProbeLine == "[FORMAT]")
        {
            inFormatSection = true;
        }
    }
    return -1;
    //END CODE JACKED FROM SAMPLES PIANO KEYBOARD
}
bool DirectoriesOfAudioAndVideoFilesMuxerSyncer::timespansIntersect(qint64 timespan_0_StartTimeMs, qint64 timespan_0_DurationInMillseconds, qint64 timespan_1_StartTimeMs, qint64 timespan_1_DurationInMillseconds)
{
    //the use of equals signs in method makes it "eh exclusive". if the timespans intersect only for 1 ms (one ends just as the other begins), we DON'T count it as an intersection. add equals signs to the gt and lt comparisons to count those as intersections

    if((timespan_0_StartTimeMs + timespan_0_DurationInMillseconds) < timespan_1_StartTimeMs)
    {
        //timespan 0 is before timespan 1
        return false;
    }
    if(timespan_0_StartTimeMs > (timespan_1_StartTimeMs + timespan_1_DurationInMillseconds))
    {
        //timespan 0 is after timespan 1
        return false;
    }
    //since we've elimited every other option, we can deduce that they intersect
    return true;
}
void DirectoriesOfAudioAndVideoFilesMuxerSyncer::muxAndSyncDirectoryOfAudioWithDirectoryOfVideo(const QString &directoryOfAudioFiles, const QString &directoryOfVideoFiles, const QString &muxOutputDirectory, qint64 audioDelayMs, qint64 truncateVideosToMsDuration_OrZeroToNotTruncate, const QString &audioDelaysInputFile_OrEmptyStringIfNoneProvided)
{
    QDir directoryOfAudioFilesDir(directoryOfAudioFiles);
    QDir directoryOfVideoFilesDir(directoryOfVideoFiles);
    QDir muxOutputDirectoryDir(muxOutputDirectory);
    /*woooo qsettings ezier -- QFile *audioDelaysInputFile_OrZeroIfNoneProvided = 0;
    if(!audioDelaysInputFile_OrEmptyStringIfNoneProvided.isEmpty())
    {
        audioDelaysInputFile_OrZeroIfNoneProvided = new QFile(audioDelaysInputFile_OrEmptyStringIfNoneProvided, this);
        if(!audioDelaysInputFile_OrZeroIfNoneProvided->open())
    }*/
    muxAndSyncDirectoryOfAudioWithDirectoryOfVideo(directoryOfAudioFilesDir, directoryOfVideoFilesDir, muxOutputDirectoryDir, audioDelayMs, truncateVideosToMsDuration_OrZeroToNotTruncate, audioDelaysInputFile_OrEmptyStringIfNoneProvided);
}
void DirectoriesOfAudioAndVideoFilesMuxerSyncer::muxAndSyncDirectoryOfAudioWithDirectoryOfVideo(const QDir &directoryOfAudioFiles, const QDir &directoryOfVideoFiles, const QDir &muxOutputDirectory, qint64 audioDelayMs, qint64 truncateVideosToMsDuration_OrZeroToNotTruncate, const QString &audioDelaysInputFile_OrEmptyStringIfNoneProvided)
{
    m_MuxOutputDirectory = muxOutputDirectory;
    m_AudioDelayMs = audioDelayMs;
    m_TruncateVideosToMsDuration_OrZeroToNotTruncate = truncateVideosToMsDuration_OrZeroToNotTruncate;
    m_VideoFileMetaAndIntersectingAudios.clear();
    m_InteractivelyCalculatedAudioDelays.clear();
    m_UseCurrentAudioDelayForTheRestOfTheVideoFiles = false;
    m_AudioDelaysInputFile_OrEmptyStringIfNoneProvided = audioDelaysInputFile_OrEmptyStringIfNoneProvided;

    if(!directoryOfAudioFiles.exists())
    {
        emit e("directory of audio files does not exist:" + directoryOfAudioFiles.absolutePath());
        emit doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(false);
        return;
    }
    if(!directoryOfVideoFiles.exists())
    {
        emit e("directory of video files does not exist:" + directoryOfVideoFiles.absolutePath());
        emit doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(false);
        return;
    }

    if(!m_MuxOutputDirectory.exists())
    {
        if(!m_MuxOutputDirectory.mkpath(m_MuxOutputDirectory.absolutePath()))
        {
            emit e("failed to make mux output directory: " + m_MuxOutputDirectory.absolutePath());
            emit doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(false);
            return;
        }
    }
    else if(!m_MuxOutputDirectory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden).isEmpty())
    {
        emit e("mux output directory is not empty: " + m_MuxOutputDirectory.absolutePath());
        emit doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(false);
        return;
    }

    //make list of video files
    QDirIterator allFilesInVideoDirIterator(directoryOfVideoFiles.absolutePath(), (QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden)/*, QDirIterator::Subdirectories*/); //nvm (complicates shit too much): I don't plan on using subdirectories, but just in case
    QFileInfoList allVideoFilesInVideoDir;
    while(allFilesInVideoDirIterator.hasNext())
    {
        allFilesInVideoDirIterator.next();
        const QFileInfo &currentFile = allFilesInVideoDirIterator.fileInfo();
        if(isVideoFile(currentFile))
        {
            allVideoFilesInVideoDir << currentFile;
        }
    }
    //make list of audio files
    QDirIterator allFilesInAudioDirIterator(directoryOfAudioFiles.absolutePath(), (QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden)/*, QDirIterator::Subdirectories*/);
    QFileInfoList allAudioFilesInAudioDir;
    while(allFilesInAudioDirIterator.hasNext())
    {
        allFilesInAudioDirIterator.next();
        const QFileInfo &currentFile = allFilesInAudioDirIterator.fileInfo();
        if(isAudioFile(currentFile))
        {
            allAudioFilesInAudioDir << currentFile;
        }
    }

    //probe video files durations
    Q_FOREACH(const QFileInfo &currentVideoFile, allVideoFilesInVideoDir)
    {
        qint64 durationOfVideoFileInMs = probeDurationFromMediaFile(currentVideoFile.absoluteFilePath());
        if(m_TruncateVideosToMsDuration_OrZeroToNotTruncate > 0)
        {
            durationOfVideoFileInMs = qMin(m_TruncateVideosToMsDuration_OrZeroToNotTruncate, durationOfVideoFileInMs);
        }
        m_VideoFileMetaAndIntersectingAudios << QSharedPointer<VideoFileMetaAndIntersectingAudios>(new VideoFileMetaAndIntersectingAudios(currentVideoFile, durationOfVideoFileInMs)); //TODOreq: the video's last modified timestamp must be accurate before we get here. in my backup script, the filename holds the timestamp, and i haven't yet tested whether or not the last modified timestamp is accurate on the fs (since the file is being written to for a long duration)
    }

    //probe audio files durations
    Q_FOREACH(const QFileInfo &currentAudioFile, allAudioFilesInAudioDir)
    {
        qint64 durationOfAudioFileInMs = probeDurationFromMediaFile(currentAudioFile.absoluteFilePath());
        m_AudioFileMetas.insert(AudioFileMeta(currentAudioFile, durationOfAudioFileInMs));
    }

    m_VideoFilesIterator.reset(new QListIterator<QSharedPointer<VideoFileMetaAndIntersectingAudios> >(m_VideoFileMetaAndIntersectingAudios));
    muxSyncNextVideoFileOrEmitDoneIfNoMore();
}
void DirectoriesOfAudioAndVideoFilesMuxerSyncer::handleUserWantsToPreviewThisAudioDelayAgain()
{
    showUserCurrentTruncatedVideoPreviewSoTheyCanAnalyzeAudioDelay();
}
void DirectoriesOfAudioAndVideoFilesMuxerSyncer::handleUserWantsToUseCurrentAudioDelayMs()
{
    useCurrentAudioDelayForCurrentVideo();
    muxSyncNextVideoFileOrEmitDoneIfNoMore();
}
void DirectoriesOfAudioAndVideoFilesMuxerSyncer::handleUserWantsToUseCurrentAudioDelayMsForTheRestOfTheVideoFiles()
{
    useCurrentAudioDelayForCurrentVideo();
    m_UseCurrentAudioDelayForTheRestOfTheVideoFiles = true;
    muxSyncNextVideoFileOrEmitDoneIfNoMore();
}
void DirectoriesOfAudioAndVideoFilesMuxerSyncer::handleUserInputtedNewAudioDelay_SoGenerateAndPreviewIt(qint64 newAudioDelayMs)
{
    //TO DOnereq: do i need to re-find intersecting audio files??? if yes, i should at least NOT have to re-scan the fs (inefficient), nor should i have to re-probe the audio files (even more inefficient)
    //^honestly it really depends how big the gap is. if it's eh small then prolly not... but if it's omghuge then yea. i'd say the proper way is to re-detect intersections, but the quicker way is to not ;-P. for now i'm gonna not. not because i made a decision on it, but because i need to KISS at first. oops i accidentally proper'd it the first tiem ;-P

    m_AudioDelayMs = newAudioDelayMs;
    muxSyncCurrentVideoFile();
}
void DirectoriesOfAudioAndVideoFilesMuxerSyncer::handleAudioDelaysOutputFilePathChosen(const QString &audioDelaysOutputSaveFilaPath)
{
    //if(m_InteractivelyCalculatedAudioDelays.isEmpty())
    //^^QSettings already does lazy file creation (no values set = no file created), and I'm willing to bet it -- oh right same constructor, OF COURSE it doesn't care if the file doesn't exist...

    //custom datastream? custom textstream? naaawwww qsettings ftw
    QSettings audioDelaysOutputFile(audioDelaysOutputSaveFilaPath, QSettings::IniFormat);
    QHashIterator<QString /* video fileName */, qint64 /* chosen audio delay ms */> audioDelaysIterator(m_InteractivelyCalculatedAudioDelays);
    while(audioDelaysIterator.hasNext())
    {
        audioDelaysIterator.next();
        audioDelaysOutputFile.setValue(audioDelaysIterator.key(), audioDelaysIterator.value());
    }
    emit o("successfully wrote audio delays to: " + audioDelaysOutputSaveFilaPath);
    emit doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(true);
}
bool AudioFileMeta::operator==(const AudioFileMeta &audioFileMeta) const
{
    return (this->AudioFileInfo == audioFileMeta.AudioFileInfo);
}
