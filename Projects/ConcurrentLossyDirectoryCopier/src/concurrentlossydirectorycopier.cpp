#include "concurrentlossydirectorycopier.h"

#define FFMPEG_PROCESS_PATH "/usr/local/bin/ffmpeg"
#define CWEBP_PROCESS_PATH "/usr/bin/cwebp"
#define FFPROBE_PROCESS_PATH "/usr/local/bin/ffprobe"

bool fileNameEndsWithOneOfTheseExentions(QString fileName, QList<QString> fileNameExtensions)
{
    QListIterator<QString> it(fileNameExtensions);
    while(it.hasNext())
    {
        QString currentFileNameExtension = it.next();
        if(fileName.toLower().endsWith(QString("." + currentFileNameExtension)))
        {
            return true;
        }
    }
    return false;
}
double getValueFromOutputLine(const QString &outputLine)
{
    QStringList splitAtEqualSign = outputLine.split("=", QString::SkipEmptyParts);
    if(splitAtEqualSign.size() != 2)
    {
        return -1.0;
    }
    bool ok = false;
    QString rightHalf = splitAtEqualSign.at(1);
    double ret = rightHalf.toDouble(&ok);
    if(!ok)
    {
        return -1.0;
    }
    return ret;
}
void deleteFileIfExists(const QString &pathToDelete)
{
    if(QFile::exists(pathToDelete))
    {
        if(!QFile::remove(pathToDelete))
        {
            qDebug() << "failed to delete a failed conversion output";
        }
    }
}
void singleLossyCopyOperation(CopyOperation copyOperation)
{
#ifdef D3FAULT_GIVES_A_FUCK_ABOUT_FRAGMENTATION_SDFOSDUFLJEWROU
    if(copyOperation.CopyOperationType == CopyOperation::CopyFallback)
    {
        SingleReadSingleWriteMutex.queueForPlainCopyButAlwaysPrioritizeNonPlainCopies(copyOperation);
        return;
    }

    QString workingDirFilePath = SingleReadSingleWriteMutex.readToWorkingDir(copyOperation); //prioritized above plain copies
#endif

    qDebug() << "Starting a Conversion[/Copy] On Thread: " << QThread::currentThread();

    bool converted = false;
    if(copyOperation.CopyOperationType != CopyOperation::CopyFallback)
    {
        QProcess ffmpegProcess;
        ffmpegProcess.setProcessChannelMode(QProcess::MergedChannels);

        QProcessEnvironment ffmpegProcessEnvironenment = ffmpegProcess.processEnvironment();
        QString ldLibraryPath = ffmpegProcessEnvironenment.value("LD_LIBRARY_PATH");
        QString ldPathToMakeHighPriority = "/usr/local/lib/";
        if(!ldLibraryPath.trimmed().isEmpty())
        {
            ldPathToMakeHighPriority = ldPathToMakeHighPriority + ":";
        }
        ldLibraryPath = ldPathToMakeHighPriority + ldLibraryPath;
        ffmpegProcessEnvironenment.insert("LD_LIBRARY_PATH", ldLibraryPath);
        ffmpegProcess.setProcessEnvironment(ffmpegProcessEnvironenment);

        switch(copyOperation.CopyOperationType)
        {
        case CopyOperation::Video:
            {
                QPair<int,int> newAudioAndVideoBitrates = ConcurrentLossyDirectoryCopier::calculateOrGuessNewAudioAndVideoBitrates(copyOperation.SourceFilePath, 0.6); //TODOreq: after conversion, throw up a warning if the output is STILL bigger than the input
                if(newAudioAndVideoBitrates.first < 0 || newAudioAndVideoBitrates.second <= 0)
                {
                    qDebug() << "Internal error when calculating/guessing bitrate for:" << copyOperation.SourceFilePath;
                    //return? nah, just fall back to copy <3
                    break;
                }

                QStringList ffmpegArgs;
                ffmpegArgs << "-i" << copyOperation.SourceFilePath;
                if(newAudioAndVideoBitrates.first != 0)
                {
                    ffmpegArgs << "-acodec" << "opus" << "-b:a" << QString(QString::number(newAudioAndVideoBitrates.first) + "k");
                }
                else
                {
                    ffmpegArgs << "-an";
                }
                ffmpegArgs << "-vcodec" << "vp9" << "-b:v" << QString(QString::number(newAudioAndVideoBitrates.second) + "k") << "-f" << "webm" << "-strict" << "-2" << QString(copyOperation.DestinationFilePath + ".webm");

                ffmpegProcess.start(FFMPEG_PROCESS_PATH, ffmpegArgs);
                if(!ffmpegProcess.waitForStarted())
                {
                    qDebug() << "failed to start ffmpeg";
                }
                else if(!ffmpegProcess.waitForFinished(-1))
                {
                    qDebug() << "ffmpeg failed to finish";
                }
                else
                {
                    int ffmpegExitCode = ffmpegProcess.exitCode();
                    if(ffmpegExitCode != 0 || ffmpegProcess.exitStatus() != QProcess::NormalExit)
                    {
                        qDebug() << "ffmpeg did not exit properly and returned: " << QString::number(ffmpegExitCode);
                    }
                    else
                    {
                        //TODOreq: check output less than size of input
                        //TODOreq: output processed bytes and type spent encoding (don't record plain copy times, they're "near instant" by comparison)... this so i can calculate as it's running the estimated time left
                        converted = true;
                        break;
                    }
                }
                qDebug() << ffmpegProcess.readAll();
                deleteFileIfExists(copyOperation.DestinationFilePath + ".webm");
            }
            break;
        case CopyOperation::Audio: //TODOoptional: we can/should do "60% of input bitrate" calculating/guessing like for video (and it would be mostly copy/paste work), but tbh I'm just too fucking lazy atm... 32k for everything it is :)
            {
                QStringList ffmpegArgs;
                ffmpegArgs << "-i" << copyOperation.SourceFilePath << "-acodec" << "opus" << "-b:a" << "32k" << "-vn" << "-f" << "opus" << QString(copyOperation.DestinationFilePath + ".opus");

                ffmpegProcess.start(FFMPEG_PROCESS_PATH, ffmpegArgs);
                if(!ffmpegProcess.waitForStarted())
                {
                    qDebug() << "failed to start ffmpeg";
                }
                else if(!ffmpegProcess.waitForFinished(-1))
                {
                    qDebug() << "ffmpeg failed to finish";
                }
                else
                {
                    int ffmpegExitCode = ffmpegProcess.exitCode();
                    if(ffmpegExitCode != 0 || ffmpegProcess.exitStatus() != QProcess::NormalExit)
                    {
                        qDebug() << "ffmpeg did not exit properly and returned: " << QString::number(ffmpegExitCode);
                    }
                    else
                    {
                        converted = true;
                        break;
                    }
                }
                qDebug() << ffmpegProcess.readAll();
                deleteFileIfExists(copyOperation.DestinationFilePath + ".opus");
            }
            break;
        case CopyOperation::RawPcm22050s16leAudio:
            {
                QStringList ffmpegArgs;
                ffmpegArgs << "-f" << "s16le" << "-ar" << "22050" << "-ac" << "1" << "-i" << copyOperation.SourceFilePath << "-acodec" << "opus" << "-b:a" << "32k" << "-vn" << "-f" << "opus" << QString(copyOperation.DestinationFilePath + ".opus");

                ffmpegProcess.start(FFMPEG_PROCESS_PATH, ffmpegArgs);
                if(!ffmpegProcess.waitForStarted())
                {
                    qDebug() << "failed to start ffmpeg";
                }
                else if(!ffmpegProcess.waitForFinished(-1))
                {
                    qDebug() << "ffmpeg failed to finish";
                }
                else
                {
                    int ffmpegExitCode = ffmpegProcess.exitCode();
                    if(ffmpegExitCode != 0 || ffmpegProcess.exitStatus() != QProcess::NormalExit)
                    {
                        qDebug() << "ffmpeg did not exit properly and returned: " << QString::number(ffmpegExitCode);
                    }
                    else
                    {
                        converted = true;
                        break;
                    }
                }
                qDebug() << ffmpegProcess.readAll();
                deleteFileIfExists(copyOperation.DestinationFilePath + ".opus");
            }
            break;
        case CopyOperation::StillImage:
            {
                //yo
                //first convert to .png, so cwebp can handle it
                QStringList ffmpegArgs;
                QString tempPngFilePath = "/run/shm/tempLossyDirectoryCopierPng_thread-" + QString::number((quintptr)QThread::currentThread()) + ".png";
                QString sourceFilePathToLower = copyOperation.SourceFilePath.toLower();
                if(sourceFilePathToLower.endsWith("png") || sourceFilePathToLower.endsWith("jpg") || sourceFilePathToLower.endsWith("jpeg"))
                {
                    tempPngFilePath = copyOperation.SourceFilePath;
                    ffmpegArgs << "-formats"; //hack for pngs/jpgs and to make ffmpeg not error out (while testing (on another machine) for some reason ffmpeg isn't able to either decode or encode pngs, so this is my solution to that weird bug i don't know how to fix (libpng is installed.. so wtf) (was wasteful to convert a png/jpg to a png anyways since webp accepts them already ^_^))
                    //DOH, just realized after more testing that I still need "png encode" for every other format xD
                    //wtf this is stupid. the video/audio seem to be working fine (and the png/jpgs work via this hack), so idk why ffmpeg can't [de|en]code pngs. I'm calling this "done" (ish), only so that I can say it's the coolest app I ever whipped out of my ass in a single span awake (started at like 5pm, is now 7am). Obviously still looking for the solution because I need to run it! I should note that the main reason I made it concurrent instead of just using the non-concurrent version was so that I could encode my files in one go BEFORE December 25th. I calculated that my 1 core 3ghz CPU (i386 devbox) would take like 10-15 days, and that I could quarter it if I was running it on 4 cores (amd64 "live"/test box (new)). I guess I could have made the app be stop/resume-able, but what fun is that?
                    //Fffff, I didn't have zlib1g-dev installed because I didn't install libcurl4-openssl-dev on the amd64 "live"/test box (AND WHY WOULD I?)... because I was trying to minimize ram usage since there's no disk installed (I'm running directory converter onto an external both as source and destination (fuckit))
                }
                else
                {
                    ffmpegArgs << "-i" << copyOperation.SourceFilePath << tempPngFilePath << "-y";
                }

                ffmpegProcess.start(FFMPEG_PROCESS_PATH, ffmpegArgs);
                if(!ffmpegProcess.waitForStarted())
                {
                    qDebug() << "failed to start ffmpeg";
                }
                else if(!ffmpegProcess.waitForFinished(-1))
                {
                    qDebug() << "ffmpeg failed to finish";
                }
                else
                {
                    int ffmpegExitCode = ffmpegProcess.exitCode();
                    if(ffmpegExitCode != 0 || ffmpegProcess.exitStatus() != QProcess::NormalExit)
                    {
                        qDebug() << "ffmpeg did not exit properly and returned: " << QString::number(ffmpegExitCode);
                    }
                    else
                    {
                        //now if convert to png was successful, call cwebp
                        QProcess cWebPprocess;
                        cWebPprocess.setProcessChannelMode(QProcess::MergedChannels);
                        QStringList cwebpArgs;
                        cwebpArgs << tempPngFilePath << "-q" << "60" << "-o" << QString(copyOperation.DestinationFilePath + ".webp");

                        cWebPprocess.start(CWEBP_PROCESS_PATH, cwebpArgs);
                        if(!cWebPprocess.waitForStarted())
                        {
                            qDebug() << "failed to start cwebp";
                        }
                        else if(!cWebPprocess.waitForFinished(-1))
                        {
                            qDebug() << "cwebp failed to finish";
                        }
                        else
                        {
                            int cWebPexitCode = cWebPprocess.exitCode();
                            if(cWebPexitCode == 0 && cWebPprocess.exitStatus() == QProcess::NormalExit)
                            {
                                converted = true;
                                break;
                            }
                            else
                            {
                                qDebug() << "cwebp did not exit properly and returned: " << QString::number(cWebPexitCode);
                            }
                        }
                        qDebug() << cWebPprocess.readAll();
                        deleteFileIfExists(QString(copyOperation.DestinationFilePath + ".webp"));
                    }
                }
                qDebug() << ffmpegProcess.readAll();
                qDebug() << "ffmpeg failed to convert an image to png:" << copyOperation.SourceFilePath;
            }
            break;
        default:
            break;
        }
    }

    if(!converted)
    {
        qDebug() << "...Falling back to copy...";
        if(!QFile::copy(copyOperation.SourceFilePath, copyOperation.DestinationFilePath))
        {
            qDebug() << "Failed to Copy: " << copyOperation.SourceFilePath;
            //TODOoptional: error out n shit
        }
    }

    qDebug() << "Finished a Conversion[/Copy] On Thread: " << QThread::currentThread();
}

ConcurrentLossyDirectoryCopier::ConcurrentLossyDirectoryCopier(QObject *parent) :
    QObject(parent)
{
    //TODOoptimization: generate these lists at runtime based on the result of "ffmpeg -codecs" xD

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
    m_VideoExtensions.append("webm"); //TODOoptimization: don't re-encode vp9! -- however this is to catch vp8, so yea
    m_VideoExtensions.append("flv");

    m_SpecialCaseRawAudioExtensions.append("pcm");

    m_AudioExtensions.append("m4a");
    m_AudioExtensions.append("wav");
    m_AudioExtensions.append("mp3");
    m_AudioExtensions.append("wma");
    m_AudioExtensions.append("ogg"); //TODOoptimization: ogg can be video/audio too (PSA: ONLY USE .OGV EXTENSION FOR VIDEO)
    m_AudioExtensions.append("flac");

    m_ImageExtensions.append("jpg");
    m_ImageExtensions.append("png");
    m_ImageExtensions.append("bmp");
    //m_ImageExtensions.append("gif"); //TODOoptimization: if(animatedGif) dontConvert() else convert()
    m_ImageExtensions.append("jpeg");
    m_ImageExtensions.append("tif");
    m_ImageExtensions.append("tiff");

    connect(&m_FutureForMapWatcher, SIGNAL(finished()), this, SLOT(handleFutureForMapWatcherFinished()));
    connect(&m_FutureForMapWatcher, SIGNAL(progressValueChanged(int)), this, SIGNAL(progressValueUpdated(int)));
    connect(&m_FutureForMapWatcher, SIGNAL(canceled()), this, SLOT(handleFutureForMapCancelled()));
}
//returns -1,-1 only on error (otherwise results are at least gussed within range), BUT the output might be bigger than the input if we resorted to "guessing" xD
QPair<int, int> ConcurrentLossyDirectoryCopier::calculateOrGuessNewAudioAndVideoBitrates(const QString &videoFilePath, double newBitratePercentTarget)
{
    //Format.bit_rate doesn't seem to be reliable (0.0000) (and even if it were, it's the bitrate for both audio and video together)

    //if Format.nb_streams is < 1, well yea we fucking error out or emit an error idfk
    //if Format.nb_streams is < 2 and we aren't able to parse the bitrate, we assume it's video and calculate it's bitrate easily from duration*size
    //if Format.nb_streams is > 2, we pretend as if it's 2 and then emit a warning/error
    //if Format.nb_streams == 2 and we aren't able to parse the bitrates, we guess/assume an audio bitrate of 128k and then calculate the video bitrate using duration*size, where size accounts for the audio bitrate guess

    //of course, we then multiply those bitrates by newBitratePercentTarget
    //and then make sure they're above hardcoded minimums and below hardcoded maximums

    QProcess ffProbeProcess;
    ffProbeProcess.setProcessChannelMode(QProcess::MergedChannels);

    QProcessEnvironment ffProbeProcessEnvironenment = ffProbeProcess.processEnvironment();
    QString ldLibraryPath = ffProbeProcessEnvironenment.value("LD_LIBRARY_PATH");
    QString ldPathToMakeHighPriority = "/usr/local/lib/";
    if(!ldLibraryPath.trimmed().isEmpty())
    {
        ldPathToMakeHighPriority = ldPathToMakeHighPriority + ":";
    }
    ldLibraryPath = ldPathToMakeHighPriority + ldLibraryPath;
    ffProbeProcessEnvironenment.insert("LD_LIBRARY_PATH", ldLibraryPath);
    ffProbeProcess.setProcessEnvironment(ffProbeProcessEnvironenment);

    QStringList ffProbeArgs;
    ffProbeArgs << "-show_format" << videoFilePath;

    QPair<int,int> ret;
    ret.first = -1;
    ret.second = -1;

    ffProbeProcess.start(FFPROBE_PROCESS_PATH, ffProbeArgs);
    if(!ffProbeProcess.waitForStarted())
    {
        qDebug() << ffProbeProcess.readAll();
        qDebug() << "failed to start ffprobe";
        return ret;
    }
    if(!ffProbeProcess.waitForFinished(-1))
    {
        qDebug() << ffProbeProcess.readAll();
        qDebug() << "ffprobe failed to finish";
        return ret;
    }

    int ffprobeExitCode = ffProbeProcess.exitCode();
    if(ffprobeExitCode != 0 || ffProbeProcess.exitStatus() != QProcess::NormalExit)
    {
        qDebug() << ffProbeProcess.readAll();
        qDebug() << "ffprobe did not exit properly and returned: " << QString::number(ffprobeExitCode);
        return ret;
    }

    QTextStream ffProbeTextStream(&ffProbeProcess);
    QString currentFfProbeLine;
    bool inFormatSection = false;
    int formatNbStreams = -1;
    int formatDuration = -1;
    int formatSize = -1;
    while(!ffProbeTextStream.atEnd())
    {
        currentFfProbeLine = ffProbeTextStream.readLine().trimmed();
        if(inFormatSection)
        {
            if(currentFfProbeLine == "[/FORMAT]")
            {
                inFormatSection = false;
            }
            else
            {
                if(currentFfProbeLine.startsWith("nb_streams="))
                {
                    if(formatNbStreams != -1)
                    {
                        qDebug() << "found 2x nb_streams entries: " << videoFilePath;
                        goto errDetectingBitrateFfffff;
                    }

                    if(currentFfProbeLine.contains("N/A"))
                    {
                        qDebug() << "returned nb_streams N/A: " << videoFilePath;
                        goto errDetectingBitrateFfffff;
                    }

                    double currentLineValue = getValueFromOutputLine(currentFfProbeLine);
                    if(currentLineValue <= 0.0)
                    {
                        qDebug() << "nb_streams <= 0: " << videoFilePath;
                        goto errDetectingBitrateFfffff;
                    }
                    formatNbStreams = currentLineValue;
                }
                else if(currentFfProbeLine.startsWith("duration="))
                {
                    if(formatDuration != -1)
                    {
                        qDebug() << "found 2x durations: " << videoFilePath;
                        goto errDetectingBitrateFfffff;
                    }

                    if(currentFfProbeLine.contains("N/A"))
                    {
                        qDebug() << "returned duration N/A: " << videoFilePath;
                        goto errDetectingBitrateFfffff;
                    }

                    double currentLineValue = getValueFromOutputLine(currentFfProbeLine);
                    if(currentLineValue <= 0.0)
                    {
                        qDebug() << "duration <= 0: " << videoFilePath;
                        goto errDetectingBitrateFfffff;
                    }
                    formatDuration = currentLineValue;
                }
                else if(currentFfProbeLine.startsWith("size="))
                {
                    if(formatSize != -1)
                    {
                        qDebug() << "found 2x sizes: " << videoFilePath;
                        goto errDetectingBitrateFfffff;
                    }

                    if(currentFfProbeLine.contains("N/A"))
                    {
                        qDebug() << "returned size N/A: " << videoFilePath;
                        goto errDetectingBitrateFfffff;
                    }

                    double currentLineValue = getValueFromOutputLine(currentFfProbeLine);
                    if(currentLineValue <= 0.0)
                    {
                        qDebug() << "size <= 0: " << videoFilePath;
                        goto errDetectingBitrateFfffff;
                    }
                    formatSize = currentLineValue;
                }
            }
        }
        else
        {
            if(currentFfProbeLine == "[FORMAT]")
            {
                inFormatSection = true;
            }
            else
            {
                if(currentFfProbeLine.startsWith("Stream #"))
                {
                    QStringList splitAtColons = currentFfProbeLine.split(":", QString::SkipEmptyParts);
                    if(splitAtColons.size() > 3)
                    {
                        QString streamType = splitAtColons.at(2).trimmed().toLower();
                        if(streamType == "audio")
                        {
                            if(ret.first == -1)
                            {
                                QStringList audioStreamInfoSplitAtCommas = splitAtColons.at(3).split(",", QString::SkipEmptyParts);
                                if(audioStreamInfoSplitAtCommas.size() > 4)
                                {
                                    ret.first = parsedBitrateCheckForKbsStringAndGreaterThanZero_or_NegativeTwo(audioStreamInfoSplitAtCommas.at(4));
                                }
                                else
                                {
                                    ret.first = -2; //just so we error if finding another audio stream, but we still want to use guess mode
                                }
                            }
                            else
                            {
                                qDebug() << "already found audio stream so wtf in file: " << videoFilePath;
                                goto errDetectingBitrateFfffff;
                            }
                        }
                        else if(streamType == "video")
                        {
                            if(ret.second == -1)
                            {
                                QStringList videoStreamInfoSplitAtCommas = splitAtColons.at(3).split(",", QString::SkipEmptyParts);
                                if(videoStreamInfoSplitAtCommas.size() > 3)
                                {
                                    ret.second = parsedBitrateCheckForKbsStringAndGreaterThanZero_or_NegativeTwo(videoStreamInfoSplitAtCommas.at(3));
                                }
                                else
                                {
                                    ret.second = -2;
                                }
                            }
                            else
                            {
                                qDebug() << "already found video stream so wtf in file: " << videoFilePath;
                                goto errDetectingBitrateFfffff;
                            }
                        }
                    }
                    else
                    {
                        qDebug() << "split at colons didn't give us size 4 for file: " << videoFilePath;
                        goto errDetectingBitrateFfffff;
                    }
                }
            }
        }
    }
    goto didntErrorDetectingBitrate;

errDetectingBitrateFfffff:
    ret.first = -1;
    ret.second = -1;
    return ret;

didntErrorDetectingBitrate:
    if(formatDuration < 0 || formatNbStreams < 1 || formatSize < 0)
    {
        qDebug() << "duration, nb_streams, or size was invalid: " << videoFilePath;
        goto errDetectingBitrateFfffff;
    }
    if(ret.first <= 0 && formatNbStreams < 2)
    {
        ret.first = 0;
    }

    //guess any value missing...
    if(ret.first <= 0 && ret.second <= 0)
    {
        //guess mode activate (both)
        ret.first = 128; //huge assumption but eh fuck it
        ret.second = guessMissingKiloBitRateBasedOn_KnownKiloBitRate_Duration_and_SizeBytes(ret.first, formatDuration, formatSize);
    }
    else if(ret.first <= 0 && ret.second > 0)
    {
        //guess mode activate (audio)
        ret.first = guessMissingKiloBitRateBasedOn_KnownKiloBitRate_Duration_and_SizeBytes(ret.second, formatDuration, formatSize);
    }
    else if(ret.first > 0 && ret.second <= 0)
    {
        //guess mode activate (video)
        ret.second = guessMissingKiloBitRateBasedOn_KnownKiloBitRate_Duration_and_SizeBytes(ret.first, formatDuration, formatSize);
    }
    //else: they both have values > 0, so no need to guess xD

    //multiply, because the format we're converting to should be more efficient ;-)
    double audioAsDouble = (double)ret.first;
    audioAsDouble *= newBitratePercentTarget;
    ret.first = (int)audioAsDouble;

    double videoAsDouble = (double)ret.second;
    videoAsDouble *= newBitratePercentTarget;
    ret.second = (int)videoAsDouble;

    bool noAudioStream = ret.first == 0 ? true : false;
    //mins
    ret.first = qMax(ret.first, 6); //6 kb/s minimum for audio
    if(noAudioStream)
        ret.first = 0;
    ret.second = qMax(ret.second, 20); //20 kb/s minimum for video
    //maxs
    ret.first = qMin(ret.first, 32); //32 kb/s max for audio
    ret.second = qMin(ret.second, 500); //500 kb/s max for video

    return ret;
}
int ConcurrentLossyDirectoryCopier::parsedBitrateCheckForKbsStringAndGreaterThanZero_or_NegativeTwo(const QString &bitrateStringMaybe)
{
    int ret = -2;
    QString workingString = bitrateStringMaybe.toLower().trimmed();
    if(workingString.contains("kb/s"))
    {
        int firstSpaceIndex = workingString.indexOf(" ", 0, Qt::CaseSensitive);
        if(firstSpaceIndex > 0)
        {
            QString bitrateString = workingString.left(firstSpaceIndex); //TODOoptional: could make sure they are numbers only, but I think toInt does that for me...
            bool convertOk = false;
            int bitrateMaybe = bitrateString.toInt(&convertOk);
            if(convertOk && bitrateMaybe > 0)
            {
                ret = bitrateMaybe;
            }
        }
    }
    return ret;
}
//the knownBitRate input and the returned result are assumed to be in kilobits/s
int ConcurrentLossyDirectoryCopier::guessMissingKiloBitRateBasedOn_KnownKiloBitRate_Duration_and_SizeBytes(int knownKiloBitRate, int durationInSeconds, int sizeBytes)
{
    //<3 basic ass math
    double sizeInBits = (((double)8) * ((double)sizeBytes)); //when in doubt, add parenthesis
    double bitsPerSecondOfEntireFile = sizeInBits / ((double)durationInSeconds);
    double knownBitRate = (((double)1024) * ((double)knownKiloBitRate));
    double deducedBitRate = bitsPerSecondOfEntireFile - knownBitRate;
    int deducedKiloBitRate = (int)(deducedBitRate / (double)1024);
    return deducedKiloBitRate;
}
bool ConcurrentLossyDirectoryCopier::recursivelyAddFileEntriesToListAndMakeDestinationDirectories(QFileInfoList &rootFileInfoList)
{
    QListIterator<QFileInfo> it(rootFileInfoList);
    while(it.hasNext())
    {
        QFileInfo currentFileInfo = it.next();

        QString currentAbsoluteFilePath = currentFileInfo.absoluteFilePath();
        QString currentFileInfoRelativeFileNamePath = currentAbsoluteFilePath.right(currentAbsoluteFilePath.length() - m_AbsoluteSourceFolderPathWithSlashAppended_LENGTH);
        QString destinationAbsoluteFilePath = m_AbsoluteDestinationFolderPathWithSlashAppended + currentFileInfoRelativeFileNamePath;

        if(currentFileInfo.isFile())
        {
            CopyOperation newCopyOperation;
            newCopyOperation.SourceFilePath = currentAbsoluteFilePath;
            newCopyOperation.DestinationFilePath = destinationAbsoluteFilePath;
            newCopyOperation.CopyOperationType = CopyOperation::CopyFallback;

            if(fileNameEndsWithOneOfTheseExentions(currentFileInfo.fileName(), m_VideoExtensions))
            {
                newCopyOperation.CopyOperationType = CopyOperation::Video;
            }
            else if(fileNameEndsWithOneOfTheseExentions(currentFileInfo.fileName(), m_SpecialCaseRawAudioExtensions))
            {
                newCopyOperation.CopyOperationType = CopyOperation::RawPcm22050s16leAudio;
            }
            else if(fileNameEndsWithOneOfTheseExentions(currentFileInfo.fileName(), m_AudioExtensions))
            {
                newCopyOperation.CopyOperationType = CopyOperation::Audio;
            }
            else if(fileNameEndsWithOneOfTheseExentions(currentFileInfo.fileName(), m_ImageExtensions))
            {
                newCopyOperation.CopyOperationType = CopyOperation::StillImage;
            }

            m_ListOfCopyOperationsToDoConcurrently.append(newCopyOperation);
        }
        else if(currentFileInfo.isDir())
        {
            //Recurse into directories
            QDir nextDir(currentFileInfo.absoluteFilePath());
            nextDir.setFilter(m_DirFilter);
            QFileInfoList nextDirList = nextDir.entryInfoList();

            //make dest folders
            if(!nextDir.mkpath(destinationAbsoluteFilePath))
            {
                emit d("failed to make destination directory: " + destinationAbsoluteFilePath);
                return false;
            }

            if(!recursivelyAddFileEntriesToListAndMakeDestinationDirectories(nextDirList))
            {
                return false;
            }
        }
        else
        {
            emit d("ran into something not a dir or file: " + currentFileInfo.absoluteFilePath());
            return false;
        }
    }
    return true;
}
//this is going to be fun to write because i know it'll scale for a few centuries (actually the single-read/single-write design sucks but eh whatever (encoding is not IO bound!))...
void ConcurrentLossyDirectoryCopier::lossilyCopyDirectoryUsingAllAvailableCores(const QString &sourceDirToLossilyCopyString, const QString &destinationDirToLossilyCopyToString)
{
    QDir sourceDirToLossilyCopy(sourceDirToLossilyCopyString);
    if(!sourceDirToLossilyCopy.exists())
    {
        emit d("source dir does not exist");
        return;
    }
    QDir destinationDirToLossilyCopyTo(destinationDirToLossilyCopyToString);
    if(!destinationDirToLossilyCopyTo.exists())
    {
        emit d("dest dir does not exist");
        return;
    }

    //blah blah blah generate list of media files in dir (recursively) and set up destination paths
    //blah blah blah make all the directories in the destination

    m_DirFilter = sourceDirToLossilyCopy.filter();
    m_DirFilter |= QDir::Dirs;
    m_DirFilter |= QDir::Files;
    m_DirFilter |= QDir::NoSymLinks;
    m_DirFilter |= QDir::NoDotAndDotDot;
    m_DirFilter |= QDir::Hidden;

    destinationDirToLossilyCopyTo.setFilter(m_DirFilter);
    QStringList destEntryListShouldBeEmpty = destinationDirToLossilyCopyTo.entryList();
    if(destEntryListShouldBeEmpty.size() > 0)
    {
        emit d("destination is not empty");
        return;
    }

    sourceDirToLossilyCopy.setFilter(m_DirFilter);
    QFileInfoList rootEntryList = sourceDirToLossilyCopy.entryInfoList();

    if(destinationDirToLossilyCopyToString.endsWith("/"))
    {
       m_AbsoluteDestinationFolderPathWithSlashAppended = destinationDirToLossilyCopyToString;
    }
    else
    {
        m_AbsoluteDestinationFolderPathWithSlashAppended = destinationDirToLossilyCopyToString + "/";
    }

    m_AbsoluteSourceFolderPathWithSlashAppended_LENGTH = sourceDirToLossilyCopyString.length();
    if(!sourceDirToLossilyCopyString.endsWith("/"))
    {
        ++m_AbsoluteSourceFolderPathWithSlashAppended_LENGTH;
    }
    if(recursivelyAddFileEntriesToListAndMakeDestinationDirectories(rootEntryList))
    {
        emit d("Finished recursivelyAddFileEntriesToListAndMakeDestinationDirectories, so now we'll begin the blocking map...");
    }
    else
    {
        emit d("failed to recursivelyAddFileEntriesToListAndMakeDestinationDirectories, check above and stderr for errors");
        return;
    }

    m_FutureForMap = QtConcurrent::map(m_ListOfCopyOperationsToDoConcurrently, singleLossyCopyOperation);
    m_FutureForMapWatcher.setFuture(m_FutureForMap);
    emit progressMinsAndMaxesCalculated(m_FutureForMap.progressMinimum(), m_FutureForMap.progressMaximum());
}
void ConcurrentLossyDirectoryCopier::cancelAfterAllCurrentlyEncodingVideosFinish()
{
    if(m_FutureForMap.isRunning())
    {
        m_FutureForMap.cancel();
        emit d("we've called cancel on the future, now you just have to wait for current ffmpeg/etc instances to stop (send them SIGTERM if you must :-P)");
    }
}
void ConcurrentLossyDirectoryCopier::handleFutureForMapWatcherFinished()
{
    emit d("The map's future finished, so if there weren't any errors ON STDERR (not here), then everything went ok");
}
void ConcurrentLossyDirectoryCopier::handleFutureForMapCancelled()
{
    emit d("successfully cancelled");
}
