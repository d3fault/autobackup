#include "recursivevideolengthandsizecalculator.h"

const QString RecursiveVideoLengthAndSizeCalculator::m_FfProbeProcessFilePath = "/usr/local/bin/ffprobe";

RecursiveVideoLengthAndSizeCalculator::RecursiveVideoLengthAndSizeCalculator(QObject *parent) :
    QObject(parent)
{
    m_FfProbeProcess.setProcessChannelMode(QProcess::MergedChannels);

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

    QProcessEnvironment ffprobeProcessEnvironenment = m_FfProbeProcess.processEnvironment();
    QString ldLibraryPath = ffprobeProcessEnvironenment.value("LD_LIBRARY_PATH");
    QString ldPathToMakeHighPriority = "/usr/local/lib/";
    if(!ldLibraryPath.trimmed().isEmpty())
    {
        ldPathToMakeHighPriority = ldPathToMakeHighPriority + ":";
    }
    ldLibraryPath = ldPathToMakeHighPriority + ldLibraryPath;
    ffprobeProcessEnvironenment.insert("LD_LIBRARY_PATH", ldLibraryPath);
    m_FfProbeProcess.setProcessEnvironment(ffprobeProcessEnvironenment);
}
bool RecursiveVideoLengthAndSizeCalculator::privateRecursivelyCalculateVideoLengthsAndSizes(const QFileInfoList &rootFileInfoList)
{
    QListIterator<QFileInfo> it(rootFileInfoList);
    while(it.hasNext())
    {
        QFileInfo currentFileInfo = it.next();
        if(currentFileInfo.isFile())
        {
            if(fileNameEndsWithOneOfTheseExentions(currentFileInfo.fileName(), m_VideoExtensions))
            {
                //Video
                QStringList ffProbeArgs;
                ffProbeArgs << "-show_format" << currentFileInfo.absoluteFilePath();

                m_FfProbeProcess.start(m_FfProbeProcessFilePath, ffProbeArgs);
                if(!m_FfProbeProcess.waitForStarted())
                {
                    emit d("failed to start ffprobe");
                }
                if(!m_FfProbeProcess.waitForFinished(-1))
                {
                    emit d("ffprobe failed to finish");
                }

                int ffprobeExitCode = m_FfProbeProcess.exitCode();
                if(ffprobeExitCode != 0 || m_FfProbeProcess.exitStatus() != QProcess::NormalExit)
                {
                    emit d(m_FfProbeProcess.readAll());
                    emit d("ffprobe did not exit properly and returned: " + QString::number(ffprobeExitCode));
                    return false;
                }

                QByteArray ffProbeResults = m_FfProbeProcess.readAll();
                QBuffer ffProbeBuffer(&ffProbeResults);
                ffProbeBuffer.open(QIODevice::ReadOnly | QIODevice::Text);
                QTextStream ffProbeTextStream(&ffProbeBuffer);
                QString currentOutputLine;
                bool foundDuration = false;
                bool foundSize = false;
                while(!ffProbeTextStream.atEnd())
                {
                    currentOutputLine = ffProbeTextStream.readLine();

                    //DEBUG:
                    //emit d(currentOutputLine);

                    if(currentOutputLine.startsWith("duration="))
                    {
                        if(foundDuration)
                        {
                            emit d("found 2x durations: " + currentFileInfo.absoluteFilePath());
                            return false;
                        }
                        foundDuration = true;

                        if(currentOutputLine.contains("N/A"))
                        {
                            emit d("returned duration N/A: " + currentFileInfo.absoluteFilePath());
                            return false;
                        }

                        double currentLineValue = getValueFromOutputLine(currentOutputLine);
                        if(currentLineValue <= 0.0)
                        {
                            emit d("duration <= 0: " + currentFileInfo.absoluteFilePath());
                            return false;
                        }
                        m_RunningTotalDuration += currentLineValue;
                    }
                    else if(currentOutputLine.startsWith("size="))
                    {
                        if(foundSize)
                        {
                            emit d("found 2x sizes: " + currentFileInfo.absoluteFilePath());
                            return false;
                        }
                        foundSize = true;

                        if(currentOutputLine.contains("N/A"))
                        {
                            emit d("returned size N/A: " + currentFileInfo.absoluteFilePath());
                            return false;
                        }

                        double currentLineValue = getValueFromOutputLine(currentOutputLine);
                        if(currentLineValue <= 0.0)
                        {
                            emit d("size <= 0: " + currentFileInfo.absoluteFilePath());
                            return false;
                        }
                        m_RunningTotalSize += currentLineValue;
                    }
                }
                if(!foundDuration || !foundSize)
                {
                    emit d("didn't find one of either duration or size for: " + currentFileInfo.absoluteFilePath());
                    return false;
                }
            }
        }
        else if(currentFileInfo.isDir())
        {
            //Recurse into directories
            QDir nextDir(currentFileInfo.absoluteFilePath());
            nextDir.setFilter(m_DirFilter);
            QFileInfoList nextDirList = nextDir.entryInfoList();
            if(!privateRecursivelyCalculateVideoLengthsAndSizes(nextDirList))
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
bool RecursiveVideoLengthAndSizeCalculator::fileNameEndsWithOneOfTheseExentions(const QString &fileName, const QList<QString> &fileNameExtensions)
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
double RecursiveVideoLengthAndSizeCalculator::getValueFromOutputLine(const QString &outputLine)
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
void RecursiveVideoLengthAndSizeCalculator::recursivelyCalculateVideoLengthsAndSizes(const QString &dirString)
{
    QDir aDir(dirString);
    recursivelyCalculateVideoLengthsAndSizes(aDir);
}
void RecursiveVideoLengthAndSizeCalculator::recursivelyCalculateVideoLengthsAndSizes(const QDir &theDir)
{
    QDir aDir = theDir; //modifiable
    if(!aDir.exists())
    {
        emit d("the dir does not exist");
        return;
    }

    m_DirFilter = aDir.filter();
    m_DirFilter |= QDir::Dirs;
    m_DirFilter |= QDir::Files;
    m_DirFilter |= QDir::NoSymLinks;
    m_DirFilter |= QDir::NoDotAndDotDot;
    m_DirFilter |= QDir::Hidden;
    aDir.setFilter(m_DirFilter);

    QFileInfoList rootEntryList = aDir.entryInfoList();
    m_RunningTotalDuration = 0.0;
    m_RunningTotalSize = 0.0;
    if(privateRecursivelyCalculateVideoLengthsAndSizes(rootEntryList))
    {
        emit d("Finished recursivelyCalculateVideoLengthsAndSizes, and if there were no errors then everything went ok");
        emit d("Total Duration: " + QString::number(m_RunningTotalDuration, 'f', 4));
        emit d("Total Size: " + QString::number(m_RunningTotalSize, 'f', 4));
    }
    else
    {
        emit d("failed to recursivelyCalculateVideoLengthsAndSizes, check above for errors");
    }
}
