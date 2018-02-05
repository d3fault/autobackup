#ifndef IUIGENERATOR_H
#define IUIGENERATOR_H

#include <QTextStream>
#include <QStringList>

#include "uivariable.h"

#define TAB_format2ui "    "

class IUIGenerator
{
public:
    bool generateUi(const QList<UIVariable> &uiVariables);
protected:
    virtual QStringList filesToGenerate() const=0;
    virtual bool generateUiForFile(const QString &theRelativeFilePathInWhichToGenerate, QTextStream &currentFileTextStream, const QList<UIVariable> &uiVariables)=0;
    static QString appendSlashIfNeeded(const QString &inputString)
    {
        if(inputString.endsWith("/"))
            return inputString;
        return inputString + "/";
    }
};

#endif // IUIGENERATOR_H
