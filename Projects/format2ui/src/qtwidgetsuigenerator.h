#ifndef QTWIDGETSUIGENERATOR_H
#define QTWIDGETSUIGENERATOR_H

#include "iuigenerator.h"

#include <QStack>

#include "uivariable.h"

class QtWidgetsUiGenerator : public IUIGenerator
{
protected:
    QString projectSrcDirWithSlashAppended() const override;
    QStringList filesToGenerate() const override;
    bool generateUiForFile(const QString &theRelativeFilePathInWhichToGenerate, QTextStream &currentFileTextStream, const UICollector &rootUiCollector) override;
    void addSpecialFilesContentMarkers(SpecialFilesContentsType *out_SpecialFilesContents) override;
    QString strReplaceSpecialFile(const QString &relativeFilePathOfSpecialFile, const QString &classNameToBeSubstitutedInDuringStrReplaceHacksInSpecialFile) override;
    QString getOutputFilePathFromRelativeFilePath(const QString &outputPathWithSlashAppended, const QString &relativeFilePathOfSpecialFile, const QString &classNameToBeSubstitutedInDuringStrReplaceHacksInSpecialFile) override;
private:
    bool generateSource(const QString &absoluteFilePathOfSourceFileToGenerate, QTextStream &currentFileTextStream, const UICollector &rootUiCollector);
    void recursivelyProcessUiCollectorForSource(const UICollector &uiCollector);
    bool generateHeader(QTextStream &currentFileTextStream, const UICollector &rootUiCollector);
    int getNextUnusedLayoutInt();
    QString currentParentLayoutName() const;
    void setCurrentParentLayoutName(const QString &parentLayoutName);
    static QString lineEditMemberVariableName(const QString &variableName);
    static QString listWidgetTypeName(const QString &variableName);
    static QString listWidgetMemberVariableName(const QString &variableName);

    bool m_FirstNonWidget;
    bool m_FirstWidget;
    int m_CurrentLayoutInt = 0;
    QStack<QString> m_LayoutParentStack;

    QString m_WhatToReplaceItWith0_liiueri93jrkjieruj;
    QString m_WhatToReplaceItWith1_kldsfoiure8098347824;
    QString m_WhatToReplaceItWith2_lksdjoirueo230480894;
    QString m_WhatToReplaceItWith3_lksdfjoiduf08340983409;
    QString m_WhatToReplaceItWith4_ldkjsflj238423084;
    QString m_WhatToReplaceItWith5_lksdfjodusodsfudsflkjdskl983402824;
    QString m_WhatToReplaceItWith6;
};

#endif // QTWIDGETSUIGENERATOR_H
