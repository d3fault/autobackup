#ifndef IUIGENERATOR_H
#define IUIGENERATOR_H

#include <QJsonObject>
#include <QList>
#include <QTextStream>

#include "uivariable.h"

#define TAB_format2ui "    "

class IUIGenerator
{
public:
    virtual bool generateUi(const QJsonObject &jsonUiFormatInput)=0;
protected:
    void populateUiVariables(const QJsonObject &inputJsonObject, QList<UIVariable> *out_uiVariables);
    static QString appendSlashIfNeeded(const QString &inputString)
    {
        if(inputString.endsWith("/"))
            return inputString;
        return inputString + "/";
    }
};

#endif // IUIGENERATOR_H
