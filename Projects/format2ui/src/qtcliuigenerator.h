#ifndef QTCLIUIGENERATOR_H
#define QTCLIUIGENERATOR_H

#include "iuigenerator.h"

class QtCliUiGenerator : public IUIGenerator
{
protected:
    QStringList filesToGenerate() const override;
    bool generateUiForFile(const QString &theRelativeFilePathInWhichToGenerate, QTextStream &currentFileTextStream, const UIGeneratorFormat &format) override;
private:
    bool generateSource(QTextStream &currentFileTextStream, const UIGeneratorFormat &format);
    bool generateHeader(QTextStream &currentFileTextStream, const UIGeneratorFormat &format);
};

#endif // QTCLIUIGENERATOR_H
