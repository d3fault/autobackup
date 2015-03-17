#include "directoriesofaudioandvideofilesmuxersyncer.h"

#include <QDirIterator>
#include <QProcess>
#include <QTextStream>
#include <QDateTime>
#include <QMapIterator>

#define DUMP_PROCESS_OUTPUT(process) \
emit o(process.readAllStandardOutput()); \
emit e(process.readAllStandardError());

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
    QDirIterator allFilesInVideoDirIterator(directoryOfVideoFiles, (QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden), /* QDirIterator::Subdirectories*/); //nvm (complicates shit too much): I don't plan on using subdirectories, but just in case
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
    QDirIterator allFilesInAudioDirIterator(directoryOfAudioFiles, (QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden), /* QDirIterator::Subdirectories*/);
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
    QList<VideoFileMetaAndIntersectingAudios> videoFileMetaAndIntersectingAudios;
    Q_FOREACH(const QFileInfo &currentVideoFile, allVideoFilesInVideoDir)
    {
        int durationOfVideoFileInMs = probeDurationFromMediaFile(currentVideoFile.absoluteFilePath());
        videoFileMetaAndIntersectingAudios << VideoFileMetaAndIntersectingAudios(currentVideoFile, durationOfVideoFileInMs); //TODOreq: the video's last modified timestamp must be accurate before we get here. in my backup script, the filename holds the timestamp, and i haven't yet tested whether or not the last modified timestamp is accurate on the fs (since the file is being written to for a long duration)
    }

    //probe audio files durations, and link them to the video files they intersect with
    Q_FOREACH(const QFileInfo &currentAudioFile, allAudioFilesInAudioDir)
    {
        int durationOfAudioFileInMs = probeDurationFromMediaFile(currentAudioFile.absoluteFilePath());
        Q_FOREACH(VideoFileMetaAndIntersectingAudios &currentVideoFileMetaAndIntersectingAudios, videoFileMetaAndIntersectingAudios)
        {
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
    Q_FOREACH(const VideoFileMetaAndIntersectingAudios &currentVideoFile, videoFileMetaAndIntersectingAudios)
    {
        QStringList ffmpegArgs;
        ffmpegArgs << "-i" << currentVideoFile.VideoFileInfo.absoluteFilePath() << "-map 0";
        if(currentVideoFile.IntersectingAudioFiles.isEmpty())
        {
            //don't have audio stream
            ffmpegArgs << "-an";
        }
        else
        {
            //have audio stream

            int currentMapIndex = 1;
            QMapIterator<qint64 /* start timestamp of audio */, AudioFileMeta> intersectionAudioFilesIterator(currentVideoFile.IntersectingAudioFiles);
            while(intersectionAudioFilesIterator.hasNext());
            {
                intersectionAudioFilesIterator.next();
                TODO LEFT OFF
                ffmpegArgs << "-i" << intersectionAudioFilesIterator.value().AudioFileInfo.absoluteFilePath() << "-map" << QString::number(currentMapIndex);
                ++currentMapIndex;
            }

            ffmpegArgs << "-acodec" << "opus" << "-b:a" << "32k" << "-ac" <<  "1";
        }
        //mux -- TODOreq: lutyuv brightness? being outside maybe not necessary (idfk)
        QProcess ffmpegProcess;
        QString muxTargetDirectory_WithSlashAppended = muxOutputDirectory.absolutePath() + QDir::separator();
        ffmpegArgs << "-s" << "720x480" << "-b:v" << "275k" << "-vcodec" << "theora" << "-r" << "10" << "-f" << "segment" << "-segment_time" << "180" << "-segment_list_size" << "999999999" << "-segment_wrap" << "999999999" << "-segment_list" << QString(muxTargetDirectory_WithSlashAppended + "segmentEntryList.txt") << "-reset_timestamps" << "1" << QString(muxTargetDirectory_WithSlashAppended + "outsideSegment-%d.ogg");
        ffmpegProcess.start("ffmpeg", ffmpegArgs, QIODevice::ReadOnly);
        if(!ffmpegProcess.waitForStarted(-1))
        {
            DUMP_PROCESS_OUTPUT(ffmpegProcess)
            emit e("ffmpeg failed to start with these args: " + ffmpegArgs);
            emit doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(false);
            return;
        }
        if(!ffmpegProcess.waitForFinished(-1))
        {
            DUMP_PROCESS_OUTPUT(ffmpegProcess)
            emit e("ffmpeg failed to finish with these args: " + ffmpegArgs);
            emit doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(false);
            return;
        }
        DUMP_PROCESS_OUTPUT(ffmpegProcess)
        if(ffmpegProcess.exitCode() != 0 || ffmpegProcess.exitStatus() != QProcess::NormalExit)
        {
            emit e("ffmpeg process exitted abnormally with exit code: " + QString::number(ffmpegProcess.exitCode()) + " -- with args: " + ffmpegArgs);
            emit doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(false);
            return;
        }
    }
    emit o("done muxing and syncing directory of audio with directory of video -- everything OK");
    emit doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(true);
#endif
}
