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
    void generateSource(QTextStream &currentFileTextStream, const UIGeneratorFormat &format);
    void generateHeader(QTextStream &currentFileTextStream, const UIGeneratorFormat &format);
};

#endif // QTWIDGETSUIGENERATOR_H
