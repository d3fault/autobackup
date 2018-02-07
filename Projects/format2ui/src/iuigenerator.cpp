#include "iuigenerator.h"

#include <QTemporaryDir>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

bool IUIGenerator::generateUi(const UICollector &rootUiCollector)
{
    QTemporaryDir outputDir;
    outputDir.setAutoRemove(false);
    QString outputPathWithSlashAppended = appendSlashIfNeeded(outputDir.path());
    QStringList filesToGenerate2 = filesToGenerate();
    for(QStringList::const_iterator it = filesToGenerate2.constBegin(); it != filesToGenerate2.constEnd(); ++it)
    {
        const QString &currentFilePathToGenerate = *it;
        const QString &filePath = outputPathWithSlashAppended + currentFilePathToGenerate;
        if(!ensureMkPath(filePath)) //TODOoptimization: could probably call this less
            return false;
        QFile currentFileToGenerate(filePath); //TODOreq: parse the "ui name" from the inputFormat json shiz
        if(!currentFileToGenerate.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "failed to open file:" << currentFileToGenerate.fileName();
            return false;
        }
        QTextStream currentFileTextStream(&currentFileToGenerate);
        if(generateUiForFile(currentFilePathToGenerate, currentFileTextStream, rootUiCollector)) //kek
        {
            qDebug() << "Generated:" << currentFileToGenerate.fileName();
        }
        else
        {
            qCritical() << "failed to generate ui: " << currentFileToGenerate.fileName();
            return false;
        }
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
