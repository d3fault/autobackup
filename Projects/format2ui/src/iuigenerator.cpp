#include "iuigenerator.h"

#include <QTemporaryDir>
#include <QFileInfo>
#include <QDir>
#include <QMultiHash>
#include <QDebug>

bool IUIGenerator::generateUi(const UICollector &rootUiCollector)
{
    QTemporaryDir outputDir;
    outputDir.setAutoRemove(false);
    QString outputPathWithSlashAppended = appendSlashIfNeeded(outputDir.path());
    QStringList filesToGenerate2 = filesToGenerate();
    addSpecialFilesContentMarkers(&m_SpecialFilesContentsToNotNecessarilyGenerateEveryTimeOrToPerhapsGenerateManyTimes);
    for(QStringList::const_iterator it = filesToGenerate2.constBegin(); it != filesToGenerate2.constEnd(); ++it)
    {
        const QString &currentRelativeFilePathToGenerate = *it;
        if(m_SpecialFilesContentsToNotNecessarilyGenerateEveryTimeOrToPerhapsGenerateManyTimes.contains(currentRelativeFilePathToGenerate))
        {
            QString fileContents;
            QString fileSourceFilePath = projectSrcDirWithSlashAppended() + currentRelativeFilePathToGenerate;
            if(!readAllFile(fileSourceFilePath, &fileContents))
                return false;
            m_SpecialFilesContentsToNotNecessarilyGenerateEveryTimeOrToPerhapsGenerateManyTimes.insert(currentRelativeFilePathToGenerate, fileContents);
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
    if(!generateSpecialFilesToNotNecessarilyGenerateEveryTimeOrToPerhapsGenerateManyTimes(rootUiCollector, outputPathWithSlashAppended))
    {
        qCritical() << "failed to generate special file(s)";
        return false;
    }
    return true;
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
void IUIGenerator::addInstanceOfSpecialFile(const QString &specialFileKey_aka_relativePath, const QString &listWidgetTypeString, const UICollector &uiCollector)
{
    m_SpecialFilesInstances.insert(specialFileKey_aka_relativePath, SpecialFilesInstancesTypeEntry { uiCollector, listWidgetTypeString });
}
bool IUIGenerator::generateSpecialFilesToNotNecessarilyGenerateEveryTimeOrToPerhapsGenerateManyTimes(const UICollector &rootUiCollector /*I don't need to, but I could have iterated rootUiCollector here and now and done the checking/generating...*/, const QString &outputPathWithSlashAppended)
{
    for(SpecialFilesInstancesType::const_iterator it = m_SpecialFilesInstances.constBegin(); it != m_SpecialFilesInstances.constEnd(); ++it)
    {
        const QString &relativeFilePathOfSpecialFile = it.key();
        const SpecialFilesInstancesTypeEntry &entry = it.value();
        QString classNameToBeSubstitutedInDuringStrReplaceHacksInSpecialFile = entry.TypeString;
        //QString outputFilePath = outputPathWithSlashAppended + classNameToBeSubstitutedInDuringStrReplaceHacksInSpecialFile.toLower() + ".h";
        QString outputFilePath = getOutputFilePathFromRelativeFilePath(outputPathWithSlashAppended, relativeFilePathOfSpecialFile, classNameToBeSubstitutedInDuringStrReplaceHacksInSpecialFile);
        if(!ensureMkPath(outputFilePath))
            return false;
        QString strReplacedSpecialFile = strReplaceSpecialFile(relativeFilePathOfSpecialFile, classNameToBeSubstitutedInDuringStrReplaceHacksInSpecialFile, entry.UiCollector);
        QFile outputFile(outputFilePath);
        if(!myOpenFileForWriting(&outputFile))
            return false;
        QTextStream textStream(&outputFile);
        textStream << strReplacedSpecialFile;
        qDebug() << "Generated file with special filename (relative to src dir):" << outputFilePath;
    }
    m_SpecialFilesInstances.clear();
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
