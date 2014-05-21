#include "dasbutton.h"

#include <QDir>
#include <QStringList>
#include <QProcess>
#include <QFile>
#include <QDirIterator>
#include <QTextStream>

#define DAS_BUTTON_DEFAULT_HOME_DIR "/home/user/"
#define DAS_BUTTON_TEMP_EXTRACT_DIR m_PrefixAkaHomeDir + "temp/extractDirDELETEMELATERFUCKITFORNOW/"
#define DAS_BUTTON_HVBS_WEB m_PrefixAkaHomeDir + "hvbsWeb/"

#define DAS_BUTTON_DEFAULT_COPYRIGHT_HEADER_RELATIVE_PATH "../../d3faultPublicLicense/header.prepend.dpl.v3.d3fault.launch.distribution.txt"

DasButton::DasButton(QObject *parent)
    : QObject(parent)
{
    //copyright header un-and-re-prepending
    m_FileExtensionsWithCstyleComments << "c" << "cpp" << "h" << "java" << "cs"; //h would be included, but since not dpl yet is not
    m_FileExtensionsWithSimpleHashStyleComments << "pro" << "asm" << "s";
    m_FileExtensionsWithPhpStyleComments << "php" << "phps";
    m_FileExtensionsWithNoComments << "txt";
    m_FileExtensionsWithXmlStyleComments << "svg";


    //filenames that will not match exactly the timestamp entry :-/
    //the following media extensions are copy/pasted from ConcurrentLossyDirectoryCopier
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

    m_SpecialCaseRawAudioExtensions.append("pcm");

    m_AudioExtensions.append("m4a");
    m_AudioExtensions.append("wav");
    m_AudioExtensions.append("mp3");
    m_AudioExtensions.append("wma");
    m_AudioExtensions.append("ogg");
    m_AudioExtensions.append("flac");

    m_ImageExtensions.append("jpg");
    m_ImageExtensions.append("png");
    m_ImageExtensions.append("bmp");
    m_ImageExtensions.append("jpeg");
    m_ImageExtensions.append("tif");
    m_ImageExtensions.append("tiff");
}
bool DasButton::readInCopyrightHeader()
{
    {
        QFile copyrightHeaderFile(m_FilepathOfCopyrightHeaderToBothUnprependAndPrependAgainLater);
        if(!copyrightHeaderFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            emit e("failed to open for reading: " + m_FilepathOfCopyrightHeaderToBothUnprependAndPrependAgainLater);
            return false;
        }
        QTextStream copyrightHeaderTextStream;
        bool firstLine = true;
        while(!copyrightHeaderTextStream.atEnd())
        {
            const QString &currentLine = copyrightHeaderTextStream.readLine();
            m_CopyrightHeaderUnmodified += (firstLine ? "" : "\n") + currentLine;
            firstLine = false;
            m_CopyrightHeader_WithHashStyleComments += "# " + currentLine + "\n";
        }
    }
    m_CopyrightHeader_WithCstyleComments = "/*\n" + m_CopyrightHeaderUnmodified + "\n*/\n\n";
    m_CopyrightHeader_WithHashStyleComments += "\n"; //.sh files have an extra newline here (fuckit)
    m_CopyrightHeader_WithPhpStyleComments = "<?php /*\n" + m_CopyrightHeaderUnmodified + "\n*/ ?>\n\n";
    m_CopyrightHeader_WithNoComments = m_CopyrightHeaderUnmodified + "\n\n";
    m_CopyrightHeader_WithXmlStyleCommentsErrorStyle = "<!-- " + m_CopyrightHeaderUnmodified + " -->\n\n\n";

    m_CopyrightHeader_WithXmlStyleComments = m_CopyrightHeaderUnmodified;
    //replace all "--" in the copyright header, since apparently they can't be in an xml comment -_- (NOOBS)
    QString tempForComparingReplacementProgress;
    do
    {
        tempForComparingReplacementProgress = m_CopyrightHeader_WithXmlStyleComments;
        m_CopyrightHeader_WithXmlStyleComments.replace("--", "__");
    }
    while(m_CopyrightHeader_WithXmlStyleComments != tempForComparingReplacementProgress);

    return true;
}
bool DasButton::extractArchive()
{
    QString extractDir(DAS_BUTTON_TEMP_EXTRACT_DIR);
    QDir dumbDir(extractDir);
    if(!dumbDir.mkpath(extractDir))
    {
        emit e("failed to make dir: " + extractDir);
        return false;
    }
    QStringList p7zipProcessArgs;
    p7zipProcessArgs << "x" << ("-o" + extractDir) << "/home/rtorrentuser/containers/dist/MyBrain-PublicFiles.7z.001";
    QProcess p7zipProcess;
    p7zipProcess.start("/usr/bin/7za", p7zipProcessArgs);
    if(!p7zipProcess.waitForStarted())
    {
        emit e("failed to start 7za");
        return false;
    }
    if(!p7zipProcess.waitForFinished(-1))
    {
        emit e("7za failed to finish");
        return false;
    }
    int exitCode = p7zipProcess.exitCode();
    if(exitCode != 0)
    {
        emit e("7za didn't exit with code 0: " + QString::number(exitCode));
        return false;
    }
    if(p7zipProcess.exitStatus() != QProcess::NormalExit)
    {
        emit e("7za crashed");
        return false;
    }
    emit o("extractArchive complete");
    return true;
}
bool DasButton::moveSomeStuffFromExtractedArchive()
{
    QDir dumbDir(DAS_BUTTON_HVBS_WEB);
    if(!dumbDir.mkpath(DAS_BUTTON_HVBS_WEB))
    {
        emit e("failed to make dir: " + DAS_BUTTON_HVBS_WEB);
        return false;
    }
    if(!myRename(DAS_BUTTON_TEMP_EXTRACT_DIR "data/oldUnversionedArchive", DAS_BUTTON_HVBS_WEB "oldUnversionedArchive"))
        return false;

    //TODOreq: just molest instead?
    //if(!myRename(DAS_BUTTON_TEMP_EXTRACT_DIR "data/oldUnversionedArchiveEasyTreeHashFile.txt", DAS_BUTTON_HVBS_WEB + "oldUnversionedArchive/oldUnversionedArchiveEasyTreeHashFile.txt")) return false;

    if(!myRename(DAS_BUTTON_TEMP_EXTRACT_DIR "data/semiOldSemiUnversionedArchive", DAS_BUTTON_HVBS_WEB "semiOldSemiUnversionedArchive"))
        return false;

    //TODOreq: semiOldSemiUnversionedArchive easy tree file as well

    if(!myRename(DAS_BUTTON_TEMP_EXTRACT_DIR "data/scan2013", DAS_BUTTON_HVBS_WEB "scan2013")) //TODOreq: no timestamps, need to gen i guess (fuck accuracy in this case?)
        return false;

    return true;
}
bool DasButton::unPrependCopyrightHeader()
{
    QDirIterator dirIterator(DAS_BUTTON_HVBS_WEB, (QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden), QDirIterator::Subdirectories);
    QFileInfo copyrightHeaderFileInfo(m_FilepathOfCopyrightHeaderToBothUnprependAndPrependAgainLater);
    const QString &copyrightHeaderFilenameOnly = copyrightHeaderFileInfo.fileName();

    while(dirIterator.hasNext())
    {
        dirIterator.next();
        const QFileInfo &currentEntry = dirIterator.fileInfo();
        if(currentEntry.isDir() || currentEntry.fileName() == copyrightHeaderFilenameOnly)
            continue;

        const QString &ext = currentEntry.suffix().toLower();

        if(fileExtensionIsInListOfExtensions(ext, m_FileExtensionsWithCstyleComments))
        {
            if(!replaceInFile(currentEntry.absoluteFilePath(), m_CopyrightHeader_WithCstyleComments, ""))
                return false;
            continue;
        }
        if(fileExtensionIsInListOfExtensions(ext, m_FileExtensionsWithSimpleHashStyleComments))
        {
            if(!replaceInFile(currentEntry.absoluteFilePath(), m_CopyrightHeader_WithHashStyleComments, ""))
                return false;
            continue;
        }
        if(fileExtensionIsInListOfExtensions(ext, m_FileExtensionsWithPhpStyleComments))
        {
            if(!replaceInFile(currentEntry.absoluteFilePath(), m_CopyrightHeader_WithPhpStyleComments, ""))
                return false;
            continue;
        }
        if(fileExtensionIsInListOfExtensions(ext, m_FileExtensionsWithNoComments))
        {
            if(!replaceInFile(currentEntry.absoluteFilePath(), m_CopyrightHeader_WithNoComments, ""))
                return false;
            continue;
        }
        if(fileExtensionIsInListOfExtensions(ext, m_FileExtensionsWithXmlStyleComments))
        {
            //replace the erroneous style (with "--") AND the fixed style (__), since it fails nicely and this makes it more compatible/future-proof
            if(!replaceInFile(currentEntry.absoluteFilePath(), m_CopyrightHeader_WithXmlStyleCommentsErrorStyle, ""))
                return false;
            if(!replaceInFile(currentEntry.absoluteFilePath(), m_CopyrightHeader_WithXmlStyleComments, ""))
                return false;
            continue;
        }
    }
    return true;
}
bool DasButton::convertEasyTreeToSimplifiedLastModifiedTimestampsAndThenMolestThoseDirsAfterFixingFilenameShitz()
{
    //cursor was here TODO LEFT OFF
}
bool DasButton::fileExtensionIsInListOfExtensions(const QString &extension, const QStringList &extensionsList)
{
    foreach(const QString &currentExtension, extensionsList)
    {
        if(currentExtension == extension)
            return true;
    }
    return false;
}
bool DasButton::replaceInFile(const QString &absoluteFilePath, const QString &stringToReplace, const QString &stringToReplaceWith)
{
    QFile lehFile(absoluteFilePath);
    bool ret = replaceInIOdevice(&lehFile);
    if(!ret)
    {
        emit e("failed to replace string in file: " + absoluteFilePath);
    }
    return ret;
}
bool DasButton::replaceInIOdevice(QIODevice *ioDeviceToReplaceStringIn, const QString &stringToReplace, const QString &stringToReplaceWith)
{
    //read
    if(!ioDeviceToReplaceStringIn->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit e("failed to open file for reading for string replacing" /*+ absoluteFilePath*/);
        return false;
    }
    QString fileContents = ioDeviceToReplaceStringIn->readAll();
    ioDeviceToReplaceStringIn->close();

    //replace
    fileContents.replace(stringToReplace, stringToReplaceWith);

    //write
    if(!ioDeviceToReplaceStringIn->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        emit e("failed to open file for writing for string replacing" /*+ absoluteFilePath*/);
        return false;
    }
    ioDeviceToReplaceStringIn->write(fileContents);
    ioDeviceToReplaceStringIn->close();
    return true;
}
void DasButton::press(const QString &prefixAkaHomeDir, const QString &filepathOfCopyrightHeaderToBothUnprependAndPrependAgainLater)
{
    m_PrefixAkaHomeDir = appendSlashIfNeeded(prefixAkaHomeDir);
    if(prefixAkaHomeDir.isEmpty())
        m_PrefixAkaHomeDir = DAS_BUTTON_DEFAULT_HOME_DIR;

    m_FilepathOfCopyrightHeaderToBothUnprependAndPrependAgainLater = filepathOfCopyrightHeaderToBothUnprependAndPrependAgainLater;
    if(filepathOfCopyrightHeaderToBothUnprependAndPrependAgainLater.isEmpty())
        m_FilepathOfCopyrightHeaderToBothUnprependAndPrependAgainLater = DAS_BUTTON_DEFAULT_COPYRIGHT_HEADER_RELATIVE_PATH;

    if(!readInCopyrightHeader())
        return;

    if(!extractArchive())
        return;
    if(!moveSomeStuffFromExtractedArchive())
        return;
    if(!unPrependCopyrightHeader())
        return;
    if(!convertEasyTreeToSimplifiedLastModifiedTimestampsAndThenMolestThoseDirsAfterFixingFilenameShitz())
        return;
}
