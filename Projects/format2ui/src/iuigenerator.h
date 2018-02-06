#ifndef IUIGENERATOR_H
#define IUIGENERATOR_H

#include <QTextStream>
#include <QStringList>

#include "uigeneratorformat.h"
#include "uivariable.h"

#define TAB_format2ui "    "

class IUIGenerator
{
public:
    bool generateUi(const UICollector &rootUiCollector);
protected:
    virtual QStringList filesToGenerate() const=0;
    virtual bool generateUiForFile(const QString &theRelativeFilePathInWhichToGenerate, QTextStream &currentFileTextStream, const UICollector &rootUiCollector)=0;
    void replaceSpecialCommentSection(QString *out_Source, const QString &specialIdInTheSpecialComments, const QString &whatToReplaceItWith);

    static QString appendSlashIfNeeded(const QString &inputString)
    {
        if(inputString.endsWith("/"))
            return inputString;
        return inputString + "/";
    }
    static QString firstLetterToUpper(const QString &inputStr)
    {
        if(inputStr.isEmpty())
            return inputStr;
        QString ret = inputStr;
        QChar firstChar = inputStr.at(0);
        firstChar = firstChar.toUpper();
        ret.replace(0, 1, firstChar);
        return ret;
    }
};

#endif // IUIGENERATOR_H
