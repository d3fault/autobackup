#include "qtcliuigeneratoroutputcompilingtemplateexample.h"

QtCliUiGeneratorOutputCompilingTemplateExample::QtCliUiGeneratorOutputCompilingTemplateExample(QObject *parent)
    : QObject(parent)
    , m_StdIn(stdin)
    , m_StdOut(stdout)
{ }
void QtCliUiGeneratorOutputCompilingTemplateExample::collectUiVariables()
{
    m_FirstName = query("First Name");
    m_LastName = query("Last Name");
    emit finishedCollectingUiVariables(true);
}
QString QtCliUiGeneratorOutputCompilingTemplateExample::firstName() const
{
    return m_FirstName;
}
QString QtCliUiGeneratorOutputCompilingTemplateExample::lastName() const
{
    return m_LastName;
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
