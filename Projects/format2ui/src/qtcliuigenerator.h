#ifndef QTCLIUIGENERATOR_H
#define QTCLIUIGENERATOR_H

#include "iuigenerator.h"

class QtCliUiGenerator : public IUIGenerator
{
protected:
    QString projectSrcDirWithSlashAppended() const override;
    QStringList filesToGenerate() const override;
    bool generateUiForFile(const QString &theRelativeFilePathInWhichToGenerate, QTextStream &currentFileTextStream, const UICollector &rootUiCollector) override;
    void addSpecialFilesContentMarkers(SpecialFilesContentsType *out_SpecialFilesContents) override;
    QString strReplaceSpecialFile(const QString &relativeFilePathOfSpecialFile, const QString &classNameToBeSubstitutedInDuringStrReplaceHacksInSpecialFile) override;
    QString getOutputFilePathFromRelativeFilePath(const QString &outputPathWithSlashAppended, const QString &relativeFilePathOfSpecialFile, const QString &classNameToBeSubstitutedInDuringStrReplaceHacksInSpecialFile) override;
private:
    bool generateSource(QTextStream &currentFileTextStream, const UICollector &rootUiCollector);
    bool generateHeader(QTextStream &currentFileTextStream, const UICollector &rootUiCollector);
};

#endif // QTCLIUIGENERATOR_H
