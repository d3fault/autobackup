#ifndef IUIGENERATOR_H
#define IUIGENERATOR_H

#include <QTextStream>
#include <QStringList>
#include <QMultiHash>

#include "uigeneratorformat.h"
#include "uivariable.h"

#define TAB_format2ui "    "

typedef QHash<QString /*relativeFilePath*/, QString /*fileContents*/> SpecialFilesContentsType;
typedef QMultiHash<QString/*specialFileKey_aka_relativePath*/,QString/*classNameToBeSubstitutedInDuringStrReplaceHacksInSpecialFile*/> SpecialFilesInstancesType;

class QFile;

class IUIGenerator
{
public:
    bool generateUi(const UICollector &rootUiCollector);
protected:
    virtual QString projectSrcDirWithSlashAppended() const=0;
    virtual QStringList filesToGenerate() const=0;
    bool readAllFile(const QString &filePath, QString *out_FileContents);
    virtual bool generateUiForFile(const QString &theRelativeFilePathInWhichToGenerate, QTextStream &currentFileTextStream, const UICollector &rootUiCollector)=0;
    virtual void addSpecialFilesContentMarkers(SpecialFilesContentsType *out_SpecialFilesContents)=0;
    virtual QString strReplaceSpecialFile(const QString &relativeFilePathOfSpecialFile, const QString &classNameToBeSubstitutedInDuringStrReplaceHacksInSpecialFile)=0;
    virtual QString getOutputFilePathFromRelativeFilePath(const QString &outputPathWithSlashAppended, const QString &relativeFilePathOfSpecialFile, const QString &classNameToBeSubstitutedInDuringStrReplaceHacksInSpecialFile)=0;
    void replaceSpecialCommentSection(QString *out_Source, const QString &specialIdInTheSpecialComments, const QString &whatToReplaceItWith);
    void addInstanceOfSpecialFile(const QString &specialFileKey_aka_relativePath, const QString &classNameToBeSubstitutedInDuringStrReplaceHacksInSpecialFile);
    bool generateSpecialFilesToNotNecessarilyGenerateEveryTimeOrToPerhapsGenerateManyTimes(const UICollector &rootUiCollector, const QString &outputPathWithSlashAppended);
    const SpecialFilesContentsType &specialFilesContents() const
    {
        return m_SpecialFilesContentsToNotNecessarilyGenerateEveryTimeOrToPerhapsGenerateManyTimes;
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

    SpecialFilesContentsType m_SpecialFilesContentsToNotNecessarilyGenerateEveryTimeOrToPerhapsGenerateManyTimes;
    SpecialFilesInstancesType m_SpecialFilesInstances;
};

#endif // IUIGENERATOR_H
