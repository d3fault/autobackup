#ifndef QTCLIUIGENERATOROUTPUTCOMPILINGTEMPLATEEXAMPLE_H
#define QTCLIUIGENERATOROUTPUTCOMPILINGTEMPLATEEXAMPLE_H

#include <QObject>

#include <QTextStream>

class QtCliUiGeneratorOutputCompilingTemplateExample : public QObject
{
    Q_OBJECT
public:
    explicit QtCliUiGeneratorOutputCompilingTemplateExample(QObject *parent = 0);
    bool parseArgs();
    void collectUiVariables();
private:
    int parseOptionalArg(QStringList *argz, const QString &optionalArg, QString *out_String);
    bool parseOptionalArgAndHandleErrors(QStringList *argz, const QString &optionalArg, QString *out_String);
    QString query(const QString &queryString, const QString &defaultValueParsedFromProcessArg);

    QTextStream m_StdIn;
    QTextStream m_StdOut;
    QTextStream m_StdErr;
    void showUsage();

    QString m_FirstNameDefaultValueParsedFromProcessArg;
    QString m_LastNameDefaultValueParsedFromProcessArg;
signals:
    void collectUiVariablesFinished(const QString &firstName, const QString &lastName);
signals: //private
    //void exitRequested(int exitCode);
};

#endif // QTCLIUIGENERATOROUTPUTCOMPILINGTEMPLATEEXAMPLE_H
