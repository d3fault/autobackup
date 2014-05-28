#include "dasbutton.h"

#include <QDir>
#include <QStringList>
#include <QProcess>
#include <QFile>
#include <QDirIterator>
#include <QTextStream>

#include "lastmodifieddateheirarchymolester.h"
#include "easytree.h"

#define DAS_BUTTON_DEFAULT_HOME_DIR "/home/user/"
#define DAS_BUTTON_TEMP_EXTRACT_DIR m_PrefixAkaHomeDir + "temp/extractDirDELETEMELATERFUCKITFORNOW/"
#define DAS_BUTTON_HVBS_WEB_SRC m_PrefixAkaHomeDir + "hvbs/webSrc/"
//#define DAS_BUTTON_TEMP_EXTRACT_OLD_UNVERSIONED_ARCHIVE_EASY_TREE
#define DAS_BUTTON_HVBS_WEB_SRC_OLD_UNVERSIONED_ARCHIVE \
                                DAS_BUTTON_HVBS_WEB_SRC "oldUnversionedArchivePerm" //if changing these dirs, update postLaunch script
#define DAS_BUTTON_HVBS_WEB_SRC_SEMI_OLD_SEMI_UNVERSIONED_ARCHIVE \
                                DAS_BUTTON_HVBS_WEB_SRC "semiOldSemiUnversionedArchivePerm"

#define DAS_BUTTON_DEFAULT_COPYRIGHT_HEADER_RELATIVE_PATH "../../d3faultPublicLicense/header.prepend.dpl.v3.d3fault.launch.distribution.txt"

DasButton::DasButton(QObject *parent)
    : QObject(parent)
{
    //copyright header un-and-re-prepending
    m_FileExtensionsWithCstyleComments << "c" << "cpp" << "h" << "java" << "cs"; //h would be included, but since not dpl yet is not
    m_FileExtensionsWithSimpleHashStyleComments << "pro" << "asm" << "s" << "sh";
    m_FileExtensionsWithPhpStyleComments << "php" << "phps";
    m_FileExtensionsWithNoComments << "txt";
    m_FileExtensionsWithXmlStyleComments << "svg";

    m_HeaderExtensions << "h" << "hpp";


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
bool DasButton::pressPrivate(const QString &prefixAkaHomeDir, const QString &filepathOfCopyrightHeaderToBothUnprependAndPrependAgainLater)
{
    m_PrefixAkaHomeDir = appendSlashIfNeeded(prefixAkaHomeDir);
    if(prefixAkaHomeDir.isEmpty())
        m_PrefixAkaHomeDir = DAS_BUTTON_DEFAULT_HOME_DIR;

    m_FilepathOfCopyrightHeaderToBothUnprependAndPrependAgainLater = filepathOfCopyrightHeaderToBothUnprependAndPrependAgainLater;
    if(filepathOfCopyrightHeaderToBothUnprependAndPrependAgainLater.isEmpty())
        m_FilepathOfCopyrightHeaderToBothUnprependAndPrependAgainLater = DAS_BUTTON_DEFAULT_COPYRIGHT_HEADER_RELATIVE_PATH;

    if(!readInCopyrightHeader())
        return false;

    if(!extractArchive())
        return false;
    emit o("archive extracted");
    if(!moveOldAndSemiOldArchivesFromTempExtractArea())
        return false;
    emit o("moved old and semi-old archives out of extracted temp area");
    if(!unPrependCopyrightHeader())
        return false;
    emit o("copyright header un-prepended from old and semi-old archives");
    if(!molestUsingEasyTreeFilesWhileAccountingForCompressedFilenameChanges())
        return false;
    emit o("old and semi-old archives molested");
    if(!makeSimplifiedLastModifiedTimestampsFileForOldAndSemiOldArchives())
        return false;
    emit o("old and semi-old archives last modified timestamps files created");

    return true;
}
bool DasButton::readInCopyrightHeader()
{
    //TODOoptional: if ever un-prepending on autobackup (or text once merged), an exception needs to be made for the copyright header filename so it isn't unprepended (but actually, so long as plaintext has extra newlines, it won't be unprepended!)
    {
        QFile copyrightHeaderFile(m_FilepathOfCopyrightHeaderToBothUnprependAndPrependAgainLater);
        if(!copyrightHeaderFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            emit e("failed to open for reading: " + m_FilepathOfCopyrightHeaderToBothUnprependAndPrependAgainLater);
            return false;
        }
        QTextStream copyrightHeaderTextStream(&copyrightHeaderFile);
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
    QString temp = m_CopyrightHeader_WithXmlStyleComments;
    m_CopyrightHeader_WithXmlStyleComments = "<!--\n" + temp + "\n-->\n\n";

    m_HeaderAllRightsReservedVariants << "/*\nThis file is part of the d3fault launch distribution\nCopyright (C) 2014 Steven Curtis Wieler II\nAll Rights Reserved\n*/\n\n";
    //way better in hindsight: TODOoptional (needs unprepend-variants-and-blah-just-busywork): m_HeaderAllRightsReservedVariants << "/*\nThis file is part of the d3fault's brain\nCopyright (C) 2014 Steven Curtis Wieler II\nAll Rights Reserved\n*/\n\n";

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
    p7zipProcessArgs << "x" << ("-o" + extractDir) << "/home/rtorrentuser/containers/dist/MyBrain-PublicFiles/MyBrain-PublicFiles.7z.001";
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
bool DasButton::moveOldAndSemiOldArchivesFromTempExtractArea()
{
    QDir dumbDir(DAS_BUTTON_HVBS_WEB_SRC);
    if(!dumbDir.mkpath(DAS_BUTTON_HVBS_WEB_SRC))
    {
        emit e("failed to make dir: " + DAS_BUTTON_HVBS_WEB_SRC);
        return false;
    }
    if(!myRename(DAS_BUTTON_TEMP_EXTRACT_DIR "data/oldUnversionedArchive", DAS_BUTTON_HVBS_WEB_SRC_OLD_UNVERSIONED_ARCHIVE))
        return false;
    if(!myRename(DAS_BUTTON_TEMP_EXTRACT_DIR "data/semiOldSemiUnversionedArchive", DAS_BUTTON_HVBS_WEB_SRC_SEMI_OLD_SEMI_UNVERSIONED_ARCHIVE))
        return false;
    //if(!myRename(DAS_BUTTON_TEMP_EXTRACT_DIR "data/scan2013", DAS_BUTTON_HVBS_WEB "scan2013")) //TODOreq: no timestamps, need to gen i guess (fuck accuracy in this case?)
        //return false;
    return true;
}
bool DasButton::unPrependCopyrightHeader()
{
    QDirIterator dirIterator(DAS_BUTTON_HVBS_WEB_SRC, (QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden), QDirIterator::Subdirectories);
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
        if(fileExtensionIsInListOfExtensions(ext, m_HeaderExtensions))
        {
            foreach(const QString &headerAllRightsReservedVariant, m_HeaderAllRightsReservedVariants)
            {
                if(!replaceInFile(currentEntry.absoluteFilePath(), headerAllRightsReservedVariant, ""))
                    return false;
            }
            continue;
        }
    }
    return true;
}
bool DasButton::molestUsingEasyTreeFilesWhileAccountingForCompressedFilenameChanges()
{
    //OT: sometimes the fastest (not easiest) way to the top of a mountain is to go straight up the side of it, ignoring any flatter/easier paths you cross in the process

    if(!myHackyMolestUsingEasyTreeFile(DAS_BUTTON_HVBS_WEB_SRC_OLD_UNVERSIONED_ARCHIVE, DAS_BUTTON_TEMP_EXTRACT_DIR "data/oldUnversionedArchiveEasyTreeHashFile.txt"))
        return false;
    if(!myHackyMolestUsingEasyTreeFile(DAS_BUTTON_HVBS_WEB_SRC_SEMI_OLD_SEMI_UNVERSIONED_ARCHIVE, DAS_BUTTON_TEMP_EXTRACT_DIR "data/semiOldSemiUnversionedArchiveEasyTreeHashFile.txt"))
        return false;
    return true;
}
bool DasButton::myHackyMolestUsingEasyTreeFile(const QString &dirCorrespondingToEasyTreeFile, const QString &easyTreeFile)
{
    LastModifiedDateHeirarchyMolester easyTreeMolester;
    connect(&easyTreeMolester, SIGNAL(d(QString)), this, SIGNAL(o(QString)));
    if(!easyTreeMolester.loadFromEasyTreeFile(dirCorrespondingToEasyTreeFile, easyTreeFile, true))
        return false;
    easyTreeMolester.hack_AppendStringOntoFilesInInternalTablesWithAnyOfTheseExtensions(m_VideoExtensions, ".LLQ.webm");
    easyTreeMolester.hack_AppendStringOntoFilesInInternalTablesWithAnyOfTheseExtensions(m_AudioExtensions, ".LLQ.opus");
    easyTreeMolester.hack_AppendStringOntoFilesInInternalTablesWithAnyOfTheseExtensions(m_SpecialCaseRawAudioExtensions, ".LLQ.opus");
    easyTreeMolester.hack_AppendStringOntoFilesInInternalTablesWithAnyOfTheseExtensions(m_ImageExtensions, ".LLQ.webp");
    if(!easyTreeMolester.molestUsingInternalTables())
        return false;
    return true;
}
bool DasButton::makeSimplifiedLastModifiedTimestampsFileForOldAndSemiOldArchives()
{
    if(!myHackyMakeSimplifiedLastModifiedTimestampsFileForOldAndSemiOldArchives(DAS_BUTTON_HVBS_WEB_SRC_OLD_UNVERSIONED_ARCHIVE))
        return false;
    if(!myHackyMakeSimplifiedLastModifiedTimestampsFileForOldAndSemiOldArchives(DAS_BUTTON_HVBS_WEB_SRC_SEMI_OLD_SEMI_UNVERSIONED_ARCHIVE))
        return false;
    return true;
}
bool DasButton::myHackyMakeSimplifiedLastModifiedTimestampsFileForOldAndSemiOldArchives(const QString &absoluteFilePathToTree)
{
    QString absoluteFilePathToTreeWithSlashAppended = appendSlashIfNeeded(absoluteFilePathToTree);
    EasyTree lastModifiedTimestampsFileMaker; //funny how the namings on the above molester and this tree'er are backwards!
    QString lastModifiedTimestampsFilenameOnly(".lastModifiedTimestamps");
    QFile lastModifiedTimestampsFile(absoluteFilePathToTreeWithSlashAppended + lastModifiedTimestampsFilenameOnly);
    if(!lastModifiedTimestampsFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        emit e("failed to open for writing: " + absoluteFilePathToTreeWithSlashAppended + lastModifiedTimestampsFilenameOnly);
        return false;
    }
    QStringList filenameIgnoreList;
    filenameIgnoreList << lastModifiedTimestampsFilenameOnly;
    QStringList emptyIgnoreList;
    lastModifiedTimestampsFileMaker.generateTreeText(absoluteFilePathToTreeWithSlashAppended, &lastModifiedTimestampsFile, EasyTree::DontCalculateMd5Sums /*does this even work with new simplified format? idk lol probably yes and/or undefined ;-P*/, &emptyIgnoreList, &filenameIgnoreList, &emptyIgnoreList, &emptyIgnoreList, EasyTree::SimplifiedLastModifiedTimestamps); //this never fails ;-)
    return true;
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
    bool ret = replaceInIOdevice(&lehFile, stringToReplace, stringToReplaceWith);
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
    QString fileContents;
    {
        QTextStream readTextStream(ioDeviceToReplaceStringIn);
        fileContents = readTextStream.readAll();
    }
    ioDeviceToReplaceStringIn->close();

    //replace
    fileContents.replace(stringToReplace, stringToReplaceWith);

    //write
    if(!ioDeviceToReplaceStringIn->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        emit e("failed to open file for writing for string replacing" /*+ absoluteFilePath*/);
        return false;
    }
    {
        QTextStream writeTextStream(ioDeviceToReplaceStringIn);
        writeTextStream << fileContents;
    }
    ioDeviceToReplaceStringIn->close();
    return true;
}
void DasButton::press(const QString &prefixAkaHomeDir, const QString &filepathOfCopyrightHeaderToBothUnprependAndPrependAgainLater)
{
    emit pressFinished(pressPrivate(prefixAkaHomeDir, filepathOfCopyrightHeaderToBothUnprependAndPrependAgainLater));
}
