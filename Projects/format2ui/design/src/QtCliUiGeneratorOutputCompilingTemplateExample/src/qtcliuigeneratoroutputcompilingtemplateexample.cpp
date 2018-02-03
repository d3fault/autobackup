#include "qtcliuigeneratoroutputcompilingtemplateexample.h"

QtCliUiGeneratorOutputCompilingTemplateExample::QtCliUiGeneratorOutputCompilingTemplateExample(QObject *parent)
    : QObject(parent)
    , m_StdIn(stdin)
    , m_StdOut(stdout)
{ }
void QtCliUiGeneratorOutputCompilingTemplateExample::collectUiVariables()
{
    QString firstName = query("First Name");
    QString lastName = query("Last Name");
    emit finishedCollectingUiVariables(firstName, lastName);
}
QString QtCliUiGeneratorOutputCompilingTemplateExample::query(const QString &queryString)
{
    m_StdOut << queryString << ": ";
    m_StdOut.flush();
    QString lineMaybe;
    do
    {
        lineMaybe = m_StdIn.readLine(); //TODOoptimization: readLineInto is better, but I don't have a recent enough version of Qt :(
    }
    while(lineMaybe.isEmpty() || lineMaybe.isNull());
    return lineMaybe;
}
