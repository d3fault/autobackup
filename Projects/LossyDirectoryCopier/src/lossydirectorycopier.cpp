#include "lossydirectorycopier.h"

const QString LossyDirectoryCopier::m_FFmpegProcessFilePath = "/usr/local/bin/ffmpeg";
const QString LossyDirectoryCopier::m_CwebPprocessFilePath = "/usr/bin/cwebp";

LossyDirectoryCopier::LossyDirectoryCopier(QObject *parent) :
    QObject(parent), m_StopRequested(false)
{
    m_FfmpegProcess.setProcessChannelMode(QProcess::MergedChannels);
    m_cWebPpPppPprocoessces.setProcessChannelMode(QProcess::MergedChannels);

#ifdef 0
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
#endif

    m_AudioExtensions.append("wav");
    m_AudioExtensions.append("mp3");
    m_AudioExtensions.append("wma");
    m_AudioExtensions.append("ogg"); //TODOoptimization: ogg can be video/audio too (PSA: ONLY USE .OGV EXTENSION FOR VIDEO)
    m_AudioExtensions.append("flac");

    m_ImageConvertExtensions.append("jpg");
    m_ImageConvertExtensions.append("png");
    m_ImageConvertExtensions.append("bmp");
    //m_ImageConvertExtensions.append("gif");
    m_ImageConvertExtensions.append("jpeg");
    m_ImageConvertExtensions.append("tif");
    m_ImageConvertExtensions.append("tiff");

    //hack i have to use because my system has libav (fork) libs and also custom compiled ffmpeg/vp9 enabled libav libs, and the ffmpeg executable finds the system (non-vp9) ones first and errors out (LOL 2013)
    QProcessEnvironment ffmpegProcessEnvironenment = m_FfmpegProcess.processEnvironment();
    QString ldLibraryPath = ffmpegProcessEnvironenment.value("LD_LIBRARY_PATH");
    QString ldPathToMakeHighPriority = "/usr/local/lib/";
    if(!ldLibraryPath.trimmed().isEmpty())
    {
        ldPathToMakeHighPriority = ldPathToMakeHighPriority + ":"; //hurr search our path first, but still search all the old ones :)
    }
    ldLibraryPath = ldPathToMakeHighPriority + ldLibraryPath;
    ffmpegProcessEnvironenment.insert("LD_LIBRARY_PATH", ldLibraryPath);
    m_FfmpegProcess.setProcessEnvironment(ffmpegProcessEnvironenment);

    //WARNING: if you change these params, you need to fix the indexes for the placeholders in recursivelyLossilyConvertOrCopyDirectoryEntries
    m_FfMpegProcessVideoArgs << "-i" << "placeholderForInputFilename" << "-acodec" << "opus" << "-b:a" << "32k" << "-vcodec" << "vp9" << "-f" << "webm" << "-strict" << "-2" << "placeholderForOutputFileName";
    m_FfMpegProcessAudioArgs << "-i" << "placeholderForInputFilename" << "-acodec" << "opus" << "-b:a" << "32k" << "-vn" << "-f" << "opus" << "placeholderForOutputFileName";
    m_FfMpegProcessImageToPNGArgs << "-i" << "placeholderForInputFilename" << "/run/shm/tempLossyDirectoryCopierPng.png" << "-y";
    m_CwebPprocessArgs << "/run/shm/tempLossyDirectoryCopierPng.png" << "-o" << "placeholderForOutputFileName";
}
bool LossyDirectoryCopier::recursivelyLossilyConvertOrCopyDirectoryEntries(QFileInfoList &rootFileInfoList)
{
    QListIterator<QFileInfo> it(rootFileInfoList);
    while(it.hasNext())
    {
        QFileInfo currentFileInfo = it.next();

        QString currentAbsoluteFilePath = currentFileInfo.absoluteFilePath();
        QString currentFileInfoRelativeFileNamePath = currentAbsoluteFilePath.right(currentAbsoluteFilePath.length() - m_AbsoluteSourceFolderPathWithSlashAppended_LENGTH); //fileName? filePath? fuck it, fileNamePath it is :). Also TODOreq maybe possible off by one here
        QString destinationAbsoluteFilePath = m_AbsoluteDestinationFolderPathWithSlashAppended + currentFileInfoRelativeFileNamePath;

        if(currentFileInfo.isFile())
        {
            bool converted = false;            

            emit d("Starting Conversion (with fallback to copy) of: " + currentAbsoluteFilePath);

            //try to convert [if convertable type], and only upon success set to true
            //if(convertableType(in)) -- would be better API, but i'd still have to separate vid/aud/image later...
            if(fileNameEndsWithOneOfTheseExentions(currentFileInfo.fileName(), m_VideoConvertExtensions))
            {
                //Video
                m_FfMpegProcessVideoArgs.replace(1, currentAbsoluteFilePath);
                QString convertedDestinationAbsoluteFilePath = destinationAbsoluteFilePath + ".webm";
                m_FfMpegProcessVideoArgs.replace(12, convertedDestinationAbsoluteFilePath);
                if(executeFfMpeg(m_FfMpegProcessVideoArgs))
                {
                    converted = true;
                }
                else
                {
                    deleteFileIfExists(destinationAbsoluteFilePath);
                }

            }
            else if(fileNameEndsWithOneOfTheseExentions(currentFileInfo.fileName(), m_AudioConvertExtensions))
            {
                //Audio

                m_FfMpegProcessAudioArgs.replace(1, currentAbsoluteFilePath);
                QString convertedDestinationAbsoluteFilePath = destinationAbsoluteFilePath + ".opus";
                m_FfMpegProcessAudioArgs.replace(9, convertedDestinationAbsoluteFilePath);

                //hack for raw pcm
                QStringList argsWithRawAccountedFor = m_FfMpegProcessAudioArgs; //cow = cheap
                if(currentAbsoluteFilePath.toLower().endsWith(".pcm")) //raw needs more specifiers
                {
                    //ffmpeg -f s16le -ar 22050 -ac 1 -i sampleRawAudio.pcm -acodec opus -vn -b:a 64k -f opus sampleRawToOpus.opus
                    argsWithRawAccountedFor.insert(0, "1");
                    argsWithRawAccountedFor.insert(0, "-ac");
                    argsWithRawAccountedFor.insert(0, "22050");
                    argsWithRawAccountedFor.insert(0, "-ar");
                    argsWithRawAccountedFor.insert(0, "s16le");
                    argsWithRawAccountedFor.insert(0, "-f");
                }

                if(executeFfMpeg(argsWithRawAccountedFor))
                {
                    converted = true;
                }
                else
                {
                    deleteFileIfExists(convertedDestinationAbsoluteFilePath);
                }

            }
            else if(fileNameEndsWithOneOfTheseExentions(currentFileInfo.fileName(), m_ImageConvertExtensions))
            {
                //Image

                //first convert to .png, so cwebp can handle it
                m_FfMpegProcessImageToPNGArgs.replace(1, currentAbsoluteFilePath);
                if(executeFfMpeg(m_FfMpegProcessImageToPNGArgs))
                {
                    //now if convert to png was successful, call cwebp
                    QString convertedDestinationAbsoluteFilePath = destinationAbsoluteFilePath + ".webp";
                    m_CwebPprocessArgs.replace(2, convertedDestinationAbsoluteFilePath);
                    m_cWebPpPppPprocoessces.start(m_CwebPprocessFilePath, m_CwebPprocessArgs);
                    if(m_cWebPpPppPprocoessces.waitForStarted())
                    {
                        if(m_cWebPpPppPprocoessces.waitForFinished(-1))
                        {
                            int cWebPexitCode = m_cWebPpPppPprocoessces.exitCode();
                            if(cWebPexitCode == 0 && m_cWebPpPppPprocoessces.exitStatus() == QProcess::NormalExit)
                            {
                                converted = true;
                            }
                            else
                            {
                                emit d(m_cWebPpPppPprocoessces.readAll());
                                emit d("cwebp did not exit properly and returned: " + QString::number(cWebPexitCode));
                                deleteFileIfExists(convertedDestinationAbsoluteFilePath);
                            }
                        }
                        else
                        {
                            emit d(m_cWebPpPppPprocoessces.readAll());
                            emit d("cwebp failed to finish");
                            deleteFileIfExists(convertedDestinationAbsoluteFilePath);
                        }
                    }
                    else
                    {
                        emit d(m_cWebPpPppPprocoessces.readAll());
                        emit d("failed to start cwebp");
                        deleteFileIfExists(convertedDestinationAbsoluteFilePath);
                    }
                }
                else
                {
                    emit d("ffmpeg failed to convert an image to png");
                    //no need to delete the /run/shm temp png xD
                }
            }


            if(!converted)
            {
                //plain copy
                emit d("...Falling back to Copy...");
                if(!QFile::copy(currentAbsoluteFilePath, destinationAbsoluteFilePath))
                {
                    emit d("failed to copy: " + currentAbsoluteFilePath);
                    return false;
                }
            }
            emit d("Converted [or Copied]: " + currentAbsoluteFilePath);
        }
        else if(currentFileInfo.isDir())
        {
            //Recurse into directories
            QDir nextDir(currentFileInfo.absoluteFilePath());
            nextDir.setFilter(m_DirFilter);
            QFileInfoList nextDirList = nextDir.entryInfoList();

            //just before we recurse into it (the source), we make it (the dest). This doesn't need to occur for the "root" folder
            if(!nextDir.mkpath(destinationAbsoluteFilePath))
            {
                emit d("failed to make destination directory: " + destinationAbsoluteFilePath);
                return false;
            }

            if(!recursivelyLossilyConvertOrCopyDirectoryEntries(nextDirList))
            {
                return false;
            }
        }
        else
        {
            emit d("ran into something not a dir or file: " + currentFileInfo.absoluteFilePath());
            return false;
        }
        QCoreApplication::processEvents(); //too lazy to implement the proper async design that can terminate a running ffmpeg :-P
        if(m_StopRequested)
        {
            emit d("stopping because stop requested");
            m_StopRequested = false;
            return false;
        }
    }
    return true;
}
bool LossyDirectoryCopier::fileNameEndsWithOneOfTheseExentions(QString fileName, QList<QString> fileNameExtensions)
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
bool LossyDirectoryCopier::executeFfMpeg(QStringList ffmpegArguments)
{
    m_FfmpegProcess.start(m_FFmpegProcessFilePath, ffmpegArguments);
    bool ret = true;
    if(!m_FfmpegProcess.waitForStarted())
    {
        emit d("failed to start ffmpeg");
        ret = false;
    }
    if(!m_FfmpegProcess.waitForFinished(-1))
    {
        emit d("ffmpeg failed to finish");
        ret = false;
    }
    int ffmpegExitCode = m_FfmpegProcess.exitCode();
    if(ffmpegExitCode != 0 || m_FfmpegProcess.exitStatus() != QProcess::NormalExit)
    {
        emit d("ffmpeg did not exit properly and returned: " + QString::number(ffmpegExitCode));
        ret = false;
    }
    if(!ret)
    {
        emit d(m_FfmpegProcess.readAll());
    }
    return ret;
}
void LossyDirectoryCopier::deleteFileIfExists(const QString &filePath)
{
    if(QFile::exists(filePath))
    {
        if(!QFile::remove(filePath))
        {
            emit d("failed to delete a failed conversion output");
        }
    }
}
void LossyDirectoryCopier::lossilyCopyEveryDirectoryEntryAndJustCopyWheneverCantCompress(const QString &sourceDirString, const QString &destDirString)
{
    QDir sourceDir(sourceDirString);
    if(!sourceDir.exists())
    {
        emit d("source dir does not exist");
        return;
    }
    QDir destDir(destDirString);
    if(!destDir.exists())
    {
        emit d("dest dir does not exist");
        return;
    }

    m_DirFilter = sourceDir.filter();
    m_DirFilter |= QDir::Dirs;
    m_DirFilter |= QDir::Files;
    m_DirFilter |= QDir::NoSymLinks;
    m_DirFilter |= QDir::NoDotAndDotDot;
    m_DirFilter |= QDir::Hidden;

    destDir.setFilter(m_DirFilter);
    QStringList destEntryListShouldBeEmpty = destDir.entryList();
    if(destEntryListShouldBeEmpty.size() > 0)
    {
        emit d("destination is not empty");
        return;
    }

    sourceDir.setFilter(m_DirFilter);
    QFileInfoList rootEntryList = sourceDir.entryInfoList();

    if(destDirString.endsWith("/"))
    {
       m_AbsoluteDestinationFolderPathWithSlashAppended = destDirString;
    }
    else
    {
        m_AbsoluteDestinationFolderPathWithSlashAppended = destDirString + "/";
    }

    m_AbsoluteSourceFolderPathWithSlashAppended_LENGTH = sourceDirString.length();
    if(!sourceDirString.endsWith("/"))
    {
        ++m_AbsoluteSourceFolderPathWithSlashAppended_LENGTH;
    }
    if(recursivelyLossilyConvertOrCopyDirectoryEntries(rootEntryList))
    {
        emit d("Finished recursivelyLossilyConvertOrCopyDirectoryEntries, and if there were no errors then everything went ok");
    }
    else
    {
        emit d("failed to recursivelyLossilyConvertOrCopyDirectoryEntries, check above for errors");
    }
}
void LossyDirectoryCopier::requestStopAfterCurrentFileIsDone()
{
    m_StopRequested = true;
}
