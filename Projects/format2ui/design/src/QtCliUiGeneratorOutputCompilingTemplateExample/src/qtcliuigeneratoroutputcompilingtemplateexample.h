#ifndef QTCLIUIGENERATOROUTPUTCOMPILINGTEMPLATEEXAMPLE_H
#define QTCLIUIGENERATOROUTPUTCOMPILINGTEMPLATEEXAMPLE_H

#include <QObject>

#include <QTextStream>

#include "../../uishared/firstnamelastnameqobjectcommandlineoptionparser.h"

class QtCliUiGeneratorOutputCompilingTemplateExample : public QObject
{
    Q_OBJECT
public:
    explicit QtCliUiGeneratorOutputCompilingTemplateExample(QObject *parent = 0);
    bool parseArgs();
    void collectUiVariables();
private:
    QString query(const QString &queryString, const QString &defaultValueParsedFromProcessArg);

    QTextStream m_StdIn;
    QTextStream m_StdOut;
    FirstNameLastNameQObjectCommandLineOptionParser m_ArgParser;

signals:
    void collectUiVariablesFinished(const QString &firstName, const QString &lastName);
signals: //private
    //void exitRequested(int exitCode);
};

#endif // QTCLIUIGENERATOROUTPUTCOMPILINGTEMPLATEEXAMPLE_H
