#ifndef QTCLIUIGENERATOR_H
#define QTCLIUIGENERATOR_H

#include "iuigenerator.h"

class QtCliUiGenerator : public IUIGenerator
{
protected:
    QString absolutePathOfCompilingTemplateExampleProjectSrcDir_WithSlashAppended() const override;
    bool generateUiForFile(const QString &theRelativeFilePathInWhichToGenerate, QTextStream &currentFileTextStream, const UICollector &rootUiCollector) override;
    void addTriggeredFilesContentMarkers(TriggeredFilesContentsType *out_TriggeredFilesContents) override;
    QString strReplaceTriggeredFile(const QString &relativeFilePathOfTriggeredFile, const QString &classNameToBeSubstitutedInDuringStrReplaceHacksInTriggeredFile, const UICollector &uiCollector) override;
    QString getOutputFilePathForTriggeredFileFromRelativeFilePath(const QString &outputPathWithSlashAppended, const QString &relativeFilePathOfTriggeredFile, const QString &classNameToBeSubstitutedInDuringStrReplaceHacksInTriggeredFile) override;
private:
    bool generateSource(const QString &absoluteFilePathOfSourceFileToGenerate, QTextStream &currentFileTextStream, const UICollector &rootUiCollector);
    void recursivelyProcessUiCollectorForSource(const UICollector &uiCollector);
    bool generateHeader(QTextStream &currentFileTextStream, const UICollector &rootUiCollector);

    bool m_FirstNonWidget;

    //source
    QString m_WhatToReplaceItWith0_sdlkfjouedsflkjsdlf0983048324;
    QString m_WhatToReplaceItWith1_lsdkjflkdjsfouerooiuwerlkjou89234098234;

    //header

};

#endif // QTCLIUIGENERATOR_H
