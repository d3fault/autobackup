#ifndef IUIGENERATOR_H
#define IUIGENERATOR_H

#include <QTextStream>
#include <QStringList>
#include <QMultiHash>

#include "uigeneratorformat.h"
#include "uivariable.h"

#define TAB_format2ui "    "

typedef QHash<QString /*relativeFilePath*/, QString /*fileContents*/> TriggeredFilesContentsType;

struct TriggeredFilesInstancesTypeEntry
{
    UICollector UiCollector;
    QString TypeString; //classNameToBeSubstitutedInDuringStrReplaceHacksInTriggeredFile
};
typedef QMultiHash<QString/*triggeredFileKey_aka_relativePath*/,TriggeredFilesInstancesTypeEntry> TriggeredFilesInstancesType;

class QFile;

class IUIGenerator
{
public:
    bool generateUi(const UICollector &rootUiCollector);
protected:
    virtual QString absolutePathOfCompilingTemplateExampleProjectSrcDir_WithSlashAppended() const=0;
    virtual QStringList allFilesToGenerate() const;
    bool readAllFile(const QString &filePath, QString *out_FileContents);
    virtual bool generateUiForFile(const QString &theRelativeFilePathInWhichToGenerate, QTextStream &currentFileTextStream, const UICollector &rootUiCollector)=0;
    virtual void addTriggeredFilesContentMarkers(TriggeredFilesContentsType *out_TriggeredFilesContents)=0;
    virtual QString strReplaceTriggeredFile(const QString &relativeFilePathOfTriggeredFile, const QString &classNameToBeSubstitutedInDuringStrReplaceHacksInTriggeredFile, const UICollector &uiCollector)=0;
    virtual QString getOutputFilePathForTriggeredFileFromRelativeFilePath(const QString &outputPathWithSlashAppended, const QString &relativeFilePathOfTriggeredFile, const QString &classNameToBeSubstitutedInDuringStrReplaceHacksInTriggeredFile)=0;
    void replaceSpecialCommentSection(QString *out_Source, const QString &specialIdInTheSpecialComments, const QString &whatToReplaceItWith);
    void addInstanceOfTriggeredFile(const QString &triggeredFileKey_aka_relativePath, const QString &listWidgetTypeString, const UICollector &uiCollector);
    bool generateTriggeredFiles(const UICollector &rootUiCollector, const QString &outputPathWithSlashAppended);
    const TriggeredFilesContentsType &triggeredFilesContents() const
    {
        return m_TriggeredFilesContentsToNotNecessarilyGenerateEveryTimeOrToPerhapsGenerateManyTimes;
    }

    static QString appendSlashIfNeeded(const QString &inputString)
    {
        if(inputString.endsWith("/"))
            return inputString;
        return inputString + "/";
    }
    static QString firstLetterToUpper(const QString &inputStr)
    {
        if(inputStr.isEmpty())
            return inputStr;
        QString ret = inputStr;
        QChar firstChar = inputStr.at(0);
        firstChar = firstChar.toUpper();
        ret.replace(0, 1, firstChar);
        return ret;
    }
    static QString firstLetterToLower(const QString &inputStr)
    {
        if(inputStr.isEmpty())
            return inputStr;
        QString ret = inputStr;
        QChar firstChar = inputStr.at(0);
        firstChar = firstChar.toLower();
        ret.replace(0, 1, firstChar);
        return ret;
    }
private:
    bool myOpenFileForWriting(QFile *file);
    bool ensureMkPath(const QString &filePath_toAFileNotAdir_ToMakeSureParentDirsExist);

    TriggeredFilesContentsType m_TriggeredFilesContentsToNotNecessarilyGenerateEveryTimeOrToPerhapsGenerateManyTimes;
    TriggeredFilesInstancesType m_TriggeredFilesInstances;
};

#endif // IUIGENERATOR_H
