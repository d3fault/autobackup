#ifndef QTCLIUIGENERATOR_H
#define QTCLIUIGENERATOR_H

#include "iuigenerator.h"

class QtCliUiGenerator : public IUIGenerator
{
protected:
    QStringList filesToGenerate() const override;
    bool generateUiForFile(const QString &theRelativeFilePathInWhichToGenerate, QTextStream &currentFileTextStream, const QList<UIVariable> &uiVariables) override;
private:
    bool generateSource(QTextStream &currentFileTextStream, const QList<UIVariable> &uiVariables);
    void generateHeader(QTextStream &currentFileTextStream, const QList<UIVariable> &uiVariables);
};

#endif // QTCLIUIGENERATOR_H
