#ifndef QTWIDGETSUIGENERATOR_H
#define QTWIDGETSUIGENERATOR_H

#include "iuigenerator.h"

#include "uivariable.h"

class QtWidgetsUiGenerator : public IUIGenerator
{
protected:
    QStringList filesToGenerate() const override;
    bool generateUiForFile(const QString &theRelativeFilePathInWhichToGenerate, QTextStream &currentFileTextStream, const UIGeneratorFormat &format) override;
private:
    bool generateSource(QTextStream &currentFileTextStream, const UIGeneratorFormat &format);
    bool generateHeader(QTextStream &currentFileTextStream, const UIGeneratorFormat &format);
    static QString lineEditMemberVariableName(const QString &variableName);
    static QString plainTextEditMemberVariableName(const QString &variableName);
};

#endif // QTWIDGETSUIGENERATOR_H
