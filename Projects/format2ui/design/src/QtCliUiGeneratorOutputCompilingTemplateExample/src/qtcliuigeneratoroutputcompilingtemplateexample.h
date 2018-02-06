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
    QString queryString(const QString &queryString, const QString &defaultValueParsedFromProcessArg);
    QStringList queryStringList(const QString &stringListQueryString, const QStringList &defaultValueParsedFromProcessArg);

    QTextStream m_StdIn;
    QTextStream m_StdOut;
    FirstNameLastNameQObjectCommandLineOptionParser m_ArgParser;

signals:
    void collectUiVariablesFinished(const QString &firstName, const QString &lastName, const QStringList &top5Movies);
signals: //private
    //void exitRequested(int exitCode);
};

#endif // QTCLIUIGENERATOROUTPUTCOMPILINGTEMPLATEEXAMPLE_H
