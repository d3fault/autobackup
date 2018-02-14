#include "iuigenerator.h"

#include <QTemporaryDir>
#include <QFileInfo>
#include <QDir>
#include <QMultiHash>
#include <QDirIterator>
#include <QDebug>

bool IUIGenerator::generateUi(const UICollector &rootUiCollector)
{
    QTemporaryDir outputDir;
    outputDir.setAutoRemove(false);
    QString outputPathWithSlashAppended = appendSlashIfNeeded(outputDir.path());
    const QStringList filesToGenerate = allFilesToGenerate();
    addTriggeredFilesContentMarkers(&m_TriggeredFilesContentsToNotNecessarilyGenerateEveryTimeOrToPerhapsGenerateManyTimes);
    //for(QStringList::const_iterator it = filesToGenerate.constBegin(); it != filesToGenerate.constEnd(); ++it)
    for(auto &&currentRelativeFilePathToGenerate : filesToGenerate) //my first range-based for loop :-D
    {
        if(m_TriggeredFilesContentsToNotNecessarilyGenerateEveryTimeOrToPerhapsGenerateManyTimes.contains(currentRelativeFilePathToGenerate))
        {
            QString fileContents;
            QString fileSourceFilePath = projectSrcDirWithSlashAppended() + currentRelativeFilePathToGenerate;
            if(!readAllFile(fileSourceFilePath, &fileContents))
                return false;
            m_TriggeredFilesContentsToNotNecessarilyGenerateEveryTimeOrToPerhapsGenerateManyTimes.insert(currentRelativeFilePathToGenerate, fileContents);
            continue;
        }
        const QString &outputFilePath = outputPathWithSlashAppended + currentRelativeFilePathToGenerate;
        if(!ensureMkPath(outputFilePath)) //TODOoptimization: could probably call this less
            return false;
        QFile currentFileToGenerate(outputFilePath);
        if(!myOpenFileForWriting(&currentFileToGenerate)) //TODOreq: parse the "ui name" from the inputFormat json shiz
            return false;
        QTextStream currentFileTextStream(&currentFileToGenerate);
        if(generateUiForFile(currentRelativeFilePathToGenerate, currentFileTextStream, rootUiCollector)) //kek
        {
            qDebug() << "Generated:" << currentFileToGenerate.fileName();
        }
        else
        {
            qCritical() << "failed to generate ui: " << currentFileToGenerate.fileName();
            return false;
        }
    }
    if(!generateTriggeredFiles(rootUiCollector, outputPathWithSlashAppended))
    {
        qCritical() << "failed to generate triggered file(s)";
        return false;
    }
    return true;
}
QStringList IUIGenerator::allFilesToGenerate() const
{
    //return QStringList { QtWidgetsUiGenerator_SOURCE_FILEPATH, QtWidgetsUiGenerator_HEADER_FILEPATH };
    QStringList ret;
    QDir dir(projectSrcDirWithSlashAppended());
    QDirIterator dirIterator(dir, QDirIterator::Subdirectories); //TODOoptimization: cache the dir iteration results
    while(dirIterator.hasNext())
    {
        dirIterator.next();
        const QFileInfo &fileInfo = dirIterator.fileInfo();
        if(fileInfo.isFile())
        {
            if(fileInfo.completeSuffix().toLower().endsWith("pro.user"))
                continue;
            QString filePath = fileInfo.absoluteFilePath();
            //QString relativeFilePath = filePath.right(filePath.length() - absolutePathOfCompilingTemplateExample_DirWithSlashAppended.length());
            QString relativeFilePath = dir.relativeFilePath(filePath);
            ret << relativeFilePath;
        }
    }
    return ret;
}
bool IUIGenerator::readAllFile(const QString &filePath, QString *out_FileContents)
{
    QFile compilingTemplateExampleSourceFile(filePath);
    if(!compilingTemplateExampleSourceFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << "failed to open file for reading:" << compilingTemplateExampleSourceFile.fileName();
        return false;
    }
    QTextStream compilingTemplateExampleSourceTextStream(&compilingTemplateExampleSourceFile);
    *out_FileContents = compilingTemplateExampleSourceTextStream.readAll();
    return true;
}
void IUIGenerator::replaceSpecialCommentSection(QString *out_Source, const QString &specialIdInTheSpecialComments, const QString &whatToReplaceItWith)
{
    int indexOfBegin = out_Source->indexOf("/*format2ui-compiling-template-example_BEGIN_" + specialIdInTheSpecialComments + "*/");
    QString endKey = "/*format2ui-compiling-template-example_END_" + specialIdInTheSpecialComments + "*/";
    int indexOfEnd = out_Source->indexOf(endKey);
    int indexAfterLastCharOfEnd = indexOfEnd + endKey.size();
    int len = indexAfterLastCharOfEnd-indexOfBegin;
    out_Source->replace(indexOfBegin, len, whatToReplaceItWith);
}
void IUIGenerator::addInstanceOfTriggeredFile(const QString &triggeredFileKey_aka_relativePath, const QString &listWidgetTypeString, const UICollector &uiCollector)
{
    m_TriggeredFilesInstances.insert(triggeredFileKey_aka_relativePath, TriggeredFilesInstancesTypeEntry { uiCollector, listWidgetTypeString });
}
bool IUIGenerator::generateTriggeredFiles(const UICollector &rootUiCollector /*I don't need to, but I could have iterated rootUiCollector here and now and done the checking/generating...*/, const QString &outputPathWithSlashAppended)
{
    for(TriggeredFilesInstancesType::const_iterator it = m_TriggeredFilesInstances.constBegin(); it != m_TriggeredFilesInstances.constEnd(); ++it)
    {
        const QString &relativeFilePathOfTriggeredFile = it.key();
        const TriggeredFilesInstancesTypeEntry &entry = it.value();
        QString classNameToBeSubstitutedInDuringStrReplaceHacksInTriggeredFile = entry.TypeString;
        //QString outputFilePath = outputPathWithSlashAppended + classNameToBeSubstitutedInDuringStrReplaceHacksInTriggeredFile.toLower() + ".h";
        QString outputFilePath = getOutputFilePathFromRelativeFilePath(outputPathWithSlashAppended, relativeFilePathOfTriggeredFile, classNameToBeSubstitutedInDuringStrReplaceHacksInTriggeredFile);
        if(!ensureMkPath(outputFilePath))
            return false;
        QString strReplacedTriggeredFile = strReplaceTriggeredFile(relativeFilePathOfTriggeredFile, classNameToBeSubstitutedInDuringStrReplaceHacksInTriggeredFile, entry.UiCollector);
        QFile outputFile(outputFilePath);
        if(!myOpenFileForWriting(&outputFile))
            return false;
        QTextStream textStream(&outputFile);
        textStream << strReplacedTriggeredFile;
        qDebug() << "Generated file with triggered filename (relative to src dir):" << outputFilePath;
    }
    m_TriggeredFilesInstances.clear();
    return true;
}
bool IUIGenerator::myOpenFileForWriting(QFile *file)
{
    if(!file->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning() << "failed to open file:" << file->fileName();
        return false;
    }
    return true;
}
bool IUIGenerator::ensureMkPath(const QString &filePath_toAFileNotAdir_ToMakeSureParentDirsExist)
{
    QFileInfo fileInfo(filePath_toAFileNotAdir_ToMakeSureParentDirsExist);
    QString dirPath = fileInfo.absolutePath();
    QDir dir(dirPath);
    if(!dir.mkpath(dirPath))
    {
        qCritical() << "failed to mkpath:" << dirPath;
        return false;
    }
    return true;
}
