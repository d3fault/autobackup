#ifndef QTWIDGETSUIGENERATOR_H
#define QTWIDGETSUIGENERATOR_H

#include "iuigenerator.h"

#include "uivariable.h"

class QtWidgetsUiGenerator : public IUIGenerator
{
protected:
    QStringList filesToGenerate() const override;
    bool generateUiForFile(const QString &theRelativeFilePathInWhichToGenerate, QTextStream &currentFileTextStream, const QList<UIVariable> &uiVariables) override;
private:
    void generateSource(QTextStream &currentFileTextStream, const QList<UIVariable> &uiVariables);
    void generateHeader(QTextStream &currentFileTextStream, const QList<UIVariable> &uiVariables);
};

#endif // QTWIDGETSUIGENERATOR_H
