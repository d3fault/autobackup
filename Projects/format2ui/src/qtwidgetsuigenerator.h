#ifndef QTWIDGETSUIGENERATOR_H
#define QTWIDGETSUIGENERATOR_H

#include "iuigenerator.h"

#include "uivariable.h"

class QtWidgetsUiGenerator : public IUIGenerator
{
protected:
    QStringList filesToGenerate() const override;
    bool generateUiForFile(const QString &theRelativeFilePathInWhichToGenerate, QTextStream &currentFileTextStream, const UICollector &rootUiCollector) override;
private:
    bool generateSource(QTextStream &currentFileTextStream, const UICollector &rootUiCollector);
    void processUiCollectorForSource(const UICollector &uiCollector);
    bool generateHeader(QTextStream &currentFileTextStream, const UICollector &rootUiCollector);
    static QString lineEditMemberVariableName(const QString &variableName);
    static QString plainTextEditMemberVariableName(const QString &variableName);

    bool m_First;

    QString m_WhatToReplaceItWith0_liiueri93jrkjieruj;
    QString m_WhatToReplaceItWith1_kldsfoiure8098347824;
    QString m_WhatToReplaceItWith2_lksdjoirueo230480894;
    QString m_WhatToReplaceItWith3_lksdfjoiduf08340983409;
    QString m_WhatToReplaceItWith4_ldkjsflj238423084;
    QString m_WhatToLookFor5;
    QString m_WhatToReplaceItWith5;
};

#endif // QTWIDGETSUIGENERATOR_H
