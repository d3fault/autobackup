#include "directoriesofaudioandvideofilesmuxersyncer.h"

#include <QDirIterator>
#include <QProcess>
#include <QTextStream>
#include <QDateTime>
#include <QMapIterator>
#include <QTemporaryDir>
#include <QSharedPointer>
#include <QTemporaryFile>

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
theArgs << "-filter_complex" << filterComplex << "-map" << "'[out]'";

#define EMIT_ERROR_AND_RETURN_IF_RENAME_FAILS(renameFrom, renameTo) \
if(!QFile::rename(renameFrom, renameTo)) \
{ \
    emit e("failed to rename: '" + renameFrom + "' to '" + renameTo + "'"); \
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
    ffmpegAudioConcatArgs << "-i" << toConcatOntoOrBecome << "-i" << audioSegment << "-filter_complex" << "'[0:0][1:0]concat=n=2:v=0:a=1[out]'" << "-map" << "'[out]'" << concatenatedOutput; \
    RUN_FFMPEG(ffmpegAudioConcatArgs) \
    /*now remove audioFileBeingBuilt, then rename concatenatedOutput to audioFileBeingBuilt*/ \
    if(!QFile::remove(toConcatOntoOrBecome)) \
    { \
        emit e("failed to remove: " + toConcatOntoOrBecome); \
        emit doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(false); \
        return; \
    } \
    EMIT_ERROR_AND_RETURN_IF_RENAME_FAILS(concatenatedOutput, toConcatOntoOrBecome) \
} \
else \
{ \
    /*create. just 'become' it*/ \
    EMIT_ERROR_AND_RETURN_IF_RENAME_FAILS(audioSegment, toConcatOntoOrBecome) \
}

//this version considers video to be the more important of the two. I thought about having black screens when there's audio only, and even single stream audio only files, but considering it's a visual website the *videos* will be posted on, I'm going to make hasVideoAtThisMomentInTime (pseudo, obviously not gonna code it like that (because there would literally be infinite checks. LITERALLY)) the threshold for output. I'm going to try my best to always have audio whenever I have video, to minimize the silence
DirectoriesOfAudioAndVideoFilesMuxerSyncer::DirectoriesOfAudioAndVideoFilesMuxerSyncer(QObject *parent)
    : QObject(parent)
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
    //m_AudioExtensions.append("ogg"); --bleh could be av or just a (you bastards)
    m_AudioExtensions.append("flac");
    m_AudioExtensions.append("opus");
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
int DirectoriesOfAudioAndVideoFilesMuxerSyncer::probeDurationFromMediaFile(const QString &mediaFile)
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
void DirectoriesOfAudioAndVideoFilesMuxerSyncer::muxAndSyncDirectoryOfAudioWithDirectoryOfVideo(const QDir &directoryOfAudioFiles, const QDir &directoryOfVideoFiles, const QDir &muxOutputDirectory)
{
#if 1
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

    if(!muxOutputDirectory.exists())
    {
        if(!muxOutputDirectory.mkpath(muxOutputDirectory.absolutePath()))
        {
            emit e("failed to make mux output directory: " + muxOutputDirectory.absolutePath());
            emit doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(false);
            return;
        }
    }
    else if(!muxOutputDirectory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden).isEmpty())
    {
        emit e("mux output directory is not empty: " + muxOutputDirectory.absolutePath());
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
    QList<QSharedPointer<VideoFileMetaAndIntersectingAudios> > videoFileMetaAndIntersectingAudios;
    Q_FOREACH(const QFileInfo &currentVideoFile, allVideoFilesInVideoDir)
    {
        int durationOfVideoFileInMs = probeDurationFromMediaFile(currentVideoFile.absoluteFilePath());
        videoFileMetaAndIntersectingAudios << QSharedPointer<VideoFileMetaAndIntersectingAudios>(new VideoFileMetaAndIntersectingAudios(currentVideoFile, durationOfVideoFileInMs)); //TODOreq: the video's last modified timestamp must be accurate before we get here. in my backup script, the filename holds the timestamp, and i haven't yet tested whether or not the last modified timestamp is accurate on the fs (since the file is being written to for a long duration)
    }

    //probe audio files durations, and link them to the video files they intersect with
    Q_FOREACH(const QFileInfo &currentAudioFile, allAudioFilesInAudioDir)
    {
        int durationOfAudioFileInMs = probeDurationFromMediaFile(currentAudioFile.absoluteFilePath());
        Q_FOREACH(QSharedPointer<VideoFileMetaAndIntersectingAudios> currentVideoFileMetaAndIntersectingAudiosSharedPointer, videoFileMetaAndIntersectingAudios)
        {
            VideoFileMetaAndIntersectingAudios &currentVideoFileMetaAndIntersectingAudios = *currentVideoFileMetaAndIntersectingAudiosSharedPointer.data();
            qint64 startTimestampOfAudioMs = currentAudioFile.lastModified().toMSecsSinceEpoch();
            if(timespansIntersect(currentVideoFileMetaAndIntersectingAudios.VideoFileInfo.lastModified().toMSecsSinceEpoch(), currentVideoFileMetaAndIntersectingAudios.DurationInMillseconds, startTimestampOfAudioMs, durationOfAudioFileInMs))
            {
                if(currentVideoFileMetaAndIntersectingAudios.IntersectingAudioFiles.contains(startTimestampOfAudioMs))
                {
                    emit e("two audio files found with same start time"); //for now we don't check the more likely case of two audio files intersecting... but if they're the same start time that's different (and will probably never happen)
                    emit doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(false);
                    return;
                }
                currentVideoFileMetaAndIntersectingAudios.IntersectingAudioFiles.insert(startTimestampOfAudioMs, AudioFileMeta(currentAudioFile, durationOfAudioFileInMs));
            }
        }
    }

    //TODOreq: what should i do if no audio intersects a video file? at the very least, i should NOT mux in silent audio
    //TODOreq: i think there is only going to be 1x itsoffset per video file. ACTUALLY, per day. ACTUALLY, it will be a constant itsoffset (WOOT, ezier) as long as the two clocks don't drift apart. ACTUALLY, since i have to re-set the time on boot for the video device every time, I do need an itsoffset. The itsoffset will be valid for as long as I keep the video device on for (assuming I don't keep it on long enough for drift to factor in heh)
    Q_FOREACH(QSharedPointer<VideoFileMetaAndIntersectingAudios> currentVideoFileMetaAndIntersectingAudiosSharedPointer, videoFileMetaAndIntersectingAudios)
    {
        const VideoFileMetaAndIntersectingAudios &currentVideoFile = *currentVideoFileMetaAndIntersectingAudiosSharedPointer.data();
        QTemporaryDir tempDir;
        if(!tempDir.isValid())
        {
            emit e("failed to get temp dir:" + tempDir.path());
            emit doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(false);
            return;
        }
        QString tempDir_WithSlashAppended = appendSlashIfNeeded(tempDir.path());
        qint64 videoStartTimestampMSecs = currentVideoFile.VideoFileInfo.lastModified().toMSecsSinceEpoch();
        qint64 videoEndTimestampMs = (videoStartTimestampMSecs + currentVideoFile.DurationInMillseconds);
        QProcess ffmpegProcess;
        QStringList ffmpegArgs;
        ffmpegArgs << "-i" << currentVideoFile.VideoFileInfo.absoluteFilePath() << "-map" << "0";
        if(currentVideoFile.IntersectingAudioFiles.isEmpty())
        {
            //don't have audio stream
            ffmpegArgs << "-an";
        }
        else
        {
            //have audio stream

            //bool firstAudioSegment = true; //TODOreq: when muxing with video, check if this is still true... in which case there wasn't any audio after all? should never happen since we already accounted for it and the ones in the list DO intersect... so eh mb nvm
            QMapIterator<qint64 /* start timestamp of audio */, AudioFileMeta> currentAudioFile(currentVideoFile.IntersectingAudioFiles);
            QString audioFileBeingBuilt(tempDir_WithSlashAppended + currentVideoFile.VideoFileInfo.completeBaseName() + ".flac"); //maybe use hq opus at temporary format instead of flac? oh one hand, i don't want to decode, encode, decode, encode (using flac means i decode -> encode). on the other hand, i might run out of hdd space if i've been recording all day! rofl stupid problems, little wiggle room. i calculated it, ~24 hrs of .flac would take ~4gb, so I'm aight
            //qint64 audioFileBeingBuiltDurationMs = 0; //TODOoptional: at the end of iterating, Q_ASSERT this is same length as video (actually it can be shorter, ffmpeg doesn't care about audio tracks ending too soon.... so nvm i guess :-/)
            qint64 audioFileBeingBuilt_HaveAudioUpToTimestampMs = videoStartTimestampMSecs; //this is kinda like 'duration' in earlier/ifdef'd out attempt, but is a unix timestamp instead
            while(currentAudioFile.hasNext());
            {
                currentAudioFile.next();
                QStringList ffmpegAudioArgs;
#if 0 //lost
                if(firstAudioSegment)
                {
                    firstAudioSegment = false; //the first audio file might have started before the video, in which case we need to to -ss until the timestamp of the video. TO DOnereq: the first audio file might even extend beyond the video (meaning it's the only audio), in which case -t must be used to end when the video does
                    qint64 ss_Milliseconds = videoStartTimestampMSecs - currentAudioFile.key();  //TO DOnereq: just before insertion into command, check to see if it's still zero and then don't insert it if it is. one case where that'd happen is if the first audio segment and video both had the exact same start time (which is what we AIM for)
                    double ss_Seconds = static_cast<double>(ss_Milliseconds) / 1000.0;
                    qint64 audioEndTimestampMs = (currentAudioFile.key() + currentAudioFile.value().DurationInMillseconds);
                    audioFileBeingBuiltDurationMs += currentAudioFile.value().DurationInMillseconds;
                    qint64 t_Ms; //TO DOnereq: just before insertion into command, check to see if it's still zero and then don't insert it if it is. TODOreq: account for silence prepended
                    //calculate '-t' if necessary
                    bool t_IsNecessary = (audioEndTimestampMs > videoEndTimestampMs);
                    if(currentAudioFile.key() <= videoStartTimestampMSecs)
                    {
                        //first audio segment started before video, use -ss

                        if(ss_Milliseconds > 0)
                        {
                            audioFileBeingBuiltDurationMs -= ss_Milliseconds;
                            ffmpegAudioArgs << "-ss" << QString::number(ss_Seconds, 'f');
                            if(t_IsNecessary)
                            {
                                //t_Ms = intersectingAudioFilesIterator.value().DurationInMillseconds; //wrong?
                                //t_Ms -= ss_Milliseconds;
                                /*
                                a:[--------]
                                v:  [---]
                                olol ez just length of v xD
                                */
                                t_Ms = videoEndTimestampMs - videoStartTimestampMSecs;
                            }
                        }
                    }
                    else if(currentAudioFile.key() > videoStartTimestampMSecs)
                    {
                        //first audio segment started after video, so prepend silence
                        //prepend 69 seconds of audio command: ffmpeg -i input.mp3 -filter_complex 'aevalsrc=0:d=69[slug];[slug][0]concat=n=2:v=0:a=1[out]' -map '[out]' output.flac
                        qint64 silenceDurationMs = currentAudioFile.key() - videoStartTimestampMSecs;
                        if(t_IsNecessary)
                        {
                            t_Ms = videoEndTimestampMs - currentAudioFile.key();
                            t_Ms += silenceDurationMs;
                            /*
                            a:   [------]
                            v:[-------]
                            damn is so much ezier when i have visuals: t = videoEndTimestampMs - audioStartTimestmap; -- but yes, we do need to += silenceDurationMs -- so really it's just the length of v, but that only is valid for first audio file, when the subsequent audio files then nvm
                            oh look at that, it's what i had. good
                            */
                        }
                        audioFileBeingBuiltDurationMs += silenceDurationMs;
                        double silenceDurationSeconds = static_cast<double>(silenceDurationMs) / 1000.0;
                        APPEND_SILENCE_PREPEND_ARGS_TO_FFMPEG_ARGSLIST(ffmpegAudioArgs, silenceDurationSeconds)
                    }
                    if(t_IsNecessary)
                    {
                        audioFileBeingBuiltDurationMs -= (audioEndTimestampMs - videoEndTimestampMs); //subtracting the part that -t cuts off
                        double t_Seconds = static_cast<double>(t_Ms) / 1000.0;
                        ffmpegAudioArgs << QString::number(t_Seconds, 'f');
                    }
                    //the input (-i) must come after the -ss
                    ffmpegAudioArgs << "-i" << currentAudioFile.value().AudioFileInfo.absoluteFilePath() << audioFileBeingBuilt;
                    RUN_FFMPEG(ffmpegAudioArgs)
                }
                else
                {
                    //subsequent audio segments
                    //there is no chance of this/these audio segment[s] starting before the video starts
                    //there is a high chance of needing to prepend silence, and no chance of using "-ss". a small but possible chance we don't need to prepend silence. -t might apply, and it needs to take into account all the audio segments that came before it TODOreq

                    bool t_IsNecessary = ((currentAudioFile.key() + currentAudioFile.value().DurationInMillseconds) > videoEndTimestampMs);
                    if((videoStartTimestampMSecs + audioFileBeingBuiltDurationMs) < currentAudioFile.key())
                    {
                        //prepend silence
                        qint64 silenceDurationMs = currentAudioFile.key() - (videoStartTimestampMSecs + audioFileBeingBuiltDurationMs);
                        audioFileBeingBuiltDurationMs += silenceDurationMs;
                        double silenceDurationSeconds = static_cast<double>(silenceDurationMs) / 1000.0;
                        APPEND_SILENCE_PREPEND_ARGS_TO_FFMPEG_ARGSLIST(ffmpegAudioArgs, silenceDurationSeconds)
                    }
                    else
                    {
                        //whoa the audio file started RIGHT AS the one before it ended, heh pretty unlikely. i don't think i need to even do anything xD
                        //TODOoptional: if i am using 2x audio recording devices, then there's good chance of [INTENTIONAL(battery dying, etc)] audio overlap (which would bring us to this block of code, and we'd need to use -ss on one of the audio segments). also note that if i ever do implement that, i need to factor in -ss into the usage of -t (which the "firstAudioSegment" already does)
                    }

                    if()
                    {
                        //audio extends beyond video, use -t
                    }


                    //TODOreq: whether as is or (more likely) prepended with silence, we need to concat this onto audioFileBeingBuilt
                }
            }
#endif

            //-ss (only for the first, and only if needed)
            //-t (if needed)
            //prepend silence (if needed)

            //all three of these affect each other (fml)
            //which should i calculate first? it quickly becomes spaghetti code. KISS (easier said than done)
            //i think i shouldn't prepend silence before onto the next audio file, but append silence onto the audioFileBeingBuilt and then consider every ms up to the next audio file to already be accounted for. that's just KISS imo (but is arguably less elegant (fuckit)) <----- YES.
            //TODOreq: there are multiple places where audioFileBeingBuilt will get created for the FIRST time (if -ss is used, we don't prepend silence (audioFileBeingBuilt is first created for first audio file). if -ss is not used, we [probably] do prepend silence (audioFileBeingBuilt is first created when prepending silence). and there's a rare case where we neither prepend silence nor use -ss (the audio and video start at exactly the same ms, in which case audioFileBeingBuilt is first created for the first audio file) -- the alternative is to use the "prepend silence onto next audio file" solution. TODOreq(related): we DON'T need to add silence if there are no more audio files. i think my while(hasNext) iterator already does this for me actually

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
                QString silenceFilter("'aevalsrc=0:d=" + QString::number(silence_Seconds, 'f') + "'");
                audioSilenceArgs << "-f" << "-lavfi" << silenceFilter << "-ac" << "1" << audioSilence;
                RUN_FFMPEG(audioSilenceArgs)

                //concat onto or become audioFileBeingBuilt
                CONCAT_ONTO_OR_BECOME_AUDIO_FILE(audioFileBeingBuilt, audioSilence)

                audioFileBeingBuilt_HaveAudioUpToTimestampMs += silenceMs;
            }

#if 0 //close, but needs a refactor utilizing+modifying audioFileBeingBuilt_HaveAudioUpToTimestampMs, which didn't exist when I first wrote it
            //-t
            qint64 t_Ms = 0;
            if(audioEndTimestampMs > videoEndTimestampMs)
            {
                //audio extended past video, use -t
                /*
                a:     [---------]
                v:-----------]
                want:  ^_____^
                t=videoEndTimestampMs-audioStartTimestampMs
                indeed, the a may have started before the v, but when calculating -ss below, -t can be modified
                */
                t_Ms = videoEndTimestampMs - audioStartTimestampMs;
            }

            //-ss
            if(currentAudioFile.key() < videoStartTimestampMSecs)
            {
                //audio started before video, use -ss. ONLY THE FIRST AUDIO FILE WILL GET HERE. NVM: we need to calculate -t first, because we need to modify -t based on -ss
                /*
                a:[--------------]
                v:    [------]
                want: ^______^
                OR:
                a:[--------------]
                v:    [---------------]
                want: ^_______________^
                OR:
                a:[--------------]
                v:    [----------]
                want: ^__________^

                NOPE: first: ss=videoEndTimestampMs-videoStartTimestampMSecs
                NOPE: second: ss=videoEndTimestampMs-videoStartTimestampMSecs
                NOPE: third: ss=videoEndTimestampMs-videoStartTimestampMSecs
                wtf? all 3 are the same
                oh wait that calculates what we WANT, not -ss
                but.... all three are:
                ss=videoStartTimestampMSecs-audioStartTimestampMs;
                */
                qint64 ss_Ms = videoStartTimestampMSecs-audioStartTimestampMs;
                //t_Ms -= ss_Ms;
                //now, do we have to modify -t? or is that only for prepended silence? guh i forget (*looks at old spaghetti code* (wish me luck). yep that didn't help too messy). there's no getting around it, i need to re-determine it. nope. wait yes. fffff i hate math
                /*
                a:3-11
                v:5-9
                ss:2
                t:?
                NOPE. i was right the first time, we don't need to fuck with t
                */
                double ss_Seconds = static_cast<double>(ss_Ms) / 1000.0;
                ffmpegAudioArgs << "-ss" << QString::number(ss_Seconds, 'f');
            }
#endif
#if 0 //lost again fml
            //-t
            qint64 t_Ms = 0;
            if(audioEndTimestampMs > videoEndTimestampMs)
            {
                //audio extended past video, use -t
                /*
                a:     [---------]
                v:-----------]
                want:  ^_____^
                t=videoEndTimestampMs-audioStartTimestampMs
                indeed, the a may have started before the v, but when calculating -ss below, -t can be modified
                i don't think this changed at all (implicitly) when I utilized audioFileBeingBuilt_HaveAudioUpToTimestampMs, but maybe it did? #badatmath. oh right i don't need to CHANGE anything, just need to ADD the modification of audioFileBeingBuilt_HaveAudioUpToTimestampMs
                */
                t_Ms = videoEndTimestampMs - audioStartTimestampMs;
                audioFileBeingBuilt_HaveAudioUpToTimestampMs += t_Ms; //well that was easy
            }
            else
            {
                audioFileBeingBuilt_HaveAudioUpToTimestampMs += currentAudioFile.value().DurationInMillseconds;
            }

            //-ss
            if(currentAudioFile.key() < videoStartTimestampMSecs)
            {
                //audio started before video, use -ss. ONLY THE FIRST AUDIO FILE WILL GET HERE. NVM: we need to calculate -t first, because we need to modify -t based on -ss
                /*
                a:[--------------]
                v:    [------]
                want: ^______^
                OR:
                a:[--------------]
                v:    [---------------]
                want: ^_______________^
                OR:
                a:[--------------]
                v:    [----------]
                want: ^__________^

                NOPE: first: ss=videoEndTimestampMs-videoStartTimestampMSecs
                NOPE: second: ss=videoEndTimestampMs-videoStartTimestampMSecs
                NOPE: third: ss=videoEndTimestampMs-videoStartTimestampMSecs
                wtf? all 3 are the same
                oh wait that calculates what we WANT, not -ss
                but.... all three are:
                ss=videoStartTimestampMSecs-audioStartTimestampMs;
                */
                qint64 ss_Ms = videoStartTimestampMSecs-audioStartTimestampMs;
                //t_Ms -= ss_Ms;
                //now, do we have to modify -t? or is that only for prepended silence? guh i forget (*looks at old spaghetti code* (wish me luck). yep that didn't help too messy). there's no getting around it, i need to re-determine it. nope. wait yes. fffff i hate math
                /*
                a:3-11
                v:5-9
                ss:2
                t:?
                NOPE. i was right the first time, we don't need to fuck with t
                */
                double ss_Seconds = static_cast<double>(ss_Ms) / 1000.0;
                ffmpegAudioArgs << "-ss" << QString::number(ss_Seconds, 'f');
                audioFileBeingBuilt_HaveAudioUpToTimestampMs -= ss_Ms;
            }
#endif

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
            if(currentAudioFile.key() < videoStartTimestampMSecs)
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
        ffmpegArgs << "-i" << audioFileBeingBuilt << "-acodec" << "opus" << "-b:a" << "32k" << "-ac" <<  "1" << "-map" << "1";
    }
    //mux -- TODOreq: lutyuv brightness? being outside maybe not necessary (idfk)
    QString muxTargetDirectory_WithSlashAppended = muxOutputDirectory.absolutePath() + QDir::separator();
    ffmpegArgs << "-s" << "720x480" << "-b:v" << "275k" << "-vcodec" << "theora" << "-r" << "10" << "-f" << "segment" << "-segment_time" << "180" << "-segment_list_size" << "999999999" << "-segment_wrap" << "999999999" << "-segment_list" << QString(muxTargetDirectory_WithSlashAppended + "segmentEntryList.txt") << "-reset_timestamps" << "1" << QString(muxTargetDirectory_WithSlashAppended + "outsideSegment-%d.ogg");
    RUN_FFMPEG(ffmpegArgs)
    emit o("done muxing and syncing directory of audio with directory of video -- everything OK");
    emit doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(true);
#endif
}
