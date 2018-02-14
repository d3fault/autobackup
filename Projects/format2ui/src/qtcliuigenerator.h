#ifndef QTCLIUIGENERATOR_H
#define QTCLIUIGENERATOR_H

#include "iuigenerator.h"

class QtCliUiGenerator : public IUIGenerator
{
protected:
    QString projectSrcDirWithSlashAppended() const override;
    //QStringList filesToGenerate() const override;
    bool generateUiForFile(const QString &theRelativeFilePathInWhichToGenerate, QTextStream &currentFileTextStream, const UICollector &rootUiCollector) override;
    void addTriggeredFilesContentMarkers(TriggeredFilesContentsType *out_TriggeredFilesContents) override;
    QString strReplaceTriggeredFile(const QString &relativeFilePathOfTriggeredFile, const QString &classNameToBeSubstitutedInDuringStrReplaceHacksInTriggeredFile, const UICollector &uiCollector) override;
    QString getOutputFilePathFromRelativeFilePath(const QString &outputPathWithSlashAppended, const QString &relativeFilePathOfTriggeredFile, const QString &classNameToBeSubstitutedInDuringStrReplaceHacksInTriggeredFile) override;
private:
    bool generateSource(QTextStream &currentFileTextStream, const UICollector &rootUiCollector);
    bool generateHeader(QTextStream &currentFileTextStream, const UICollector &rootUiCollector);
};

#endif // QTCLIUIGENERATOR_H
