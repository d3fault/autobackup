#ifndef QTWIDGETSUIGENERATOR_H
#define QTWIDGETSUIGENERATOR_H

#include "iuigenerator.h"

#include <QTextStream>

#include "uivariable.h"

class QtWidgetsUiGenerator : public IUIGenerator
{
protected:
    bool generateUi(const QJsonObject &jsonUiFormatInput) override;
private:
    static QString appendSlashIfNeeded(const QString &inputString)
    {
        if(inputString.endsWith("/"))
            return inputString;
        return inputString + "/";
    }
private:
    void streamOutUiVariableInstantiationCode(QTextStream &outputSourceFileTextStream, const UIVariable &uiVariable);
};

#endif // QTWIDGETSUIGENERATOR_H
