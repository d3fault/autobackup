#ifndef QTWIDGETSUIGENERATOR_H
#define QTWIDGETSUIGENERATOR_H

#include "iuigenerator.h"

#include <QStack>
#include <QSet>

#include "uivariable.h"

typedef QStack<QString> LayoutParentStackType;

class QtWidgetsUiGenerator : public IUIGenerator
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
    bool generateHeader(const QString &absoluteFilePathOfHeaderFileToGenerate, QTextStream &currentFileTextStream, const UICollector &rootUiCollector);
    void recursivelyProcessUiCollectorForHeader(const UICollector &uiCollector);
    int getNextUnusedLayoutInt();
    QString currentParentLayoutName() const;
    QString addSpaceForEachLayoutDepth() const;
    static QString lineEditMemberVariableName(const QString &variableName);
    static QString listWidgetTypeName(const QString &variableName);
    static QString listWidgetMemberVariableName(const QString &variableName);

    bool m_FirstNonWidget;
    bool m_FirstWidget;
    int m_CurrentLayoutInt = 0;
    LayoutParentStackType m_LayoutParentStack;

    //source
    QString m_WhatToReplaceItWith0_liiueri93jrkjieruj;
    QString m_WhatToReplaceItWith1_kldsfoiure8098347824;
    QString m_WhatToReplaceItWith2_lksdjoirueo230480894;
    QString m_WhatToReplaceItWith3_lksdfjoiduf08340983409;
    QString m_WhatToReplaceItWith4_ldkjsflj238423084;
    QString m_WhatToReplaceItWith5_lksdfjodusodsfudsflkjdskl983402824;
    QString m_WhatToReplaceItWith6;

    //header
    QString m_WhatToReplaceItWith2_lskjdfouewr08084097342098;
    QString m_WhatToReplaceItWith3_sdlfkjsdklfjoure978234978234;
    QSet<QString> m_WhatToReplaceItWith4_forwardDeclareClasses_dsfsflsjdflkjowe0834082934;
};

#endif // QTWIDGETSUIGENERATOR_H
