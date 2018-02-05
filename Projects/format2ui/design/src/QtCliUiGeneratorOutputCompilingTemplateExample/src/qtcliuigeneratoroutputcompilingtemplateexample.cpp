#include "qtcliuigeneratoroutputcompilingtemplateexample.h"

#include <QCoreApplication>
#include <QStringList>
#include <QMutableListIterator>
#include <QDebug>

//TODOmb: only go into "interactive" (query->readLine) mode if --interactive is given. this is how most *nix apps are written. "fail loudly". tons of scripts would break [SILENTLY] if the default routine when an arg is _forgotten_ was to drop into query mode. but actually a test of istty could tell us whether or not we should drop to query mode or if the process should simply show cliUsage and return 1; TODOreq: think hard on this, it's a great idea but not portable xD. currently I always go into interactive mode, even if there are no args to collect. maybe a (or some) configuration var inside "format" (a la format2ui) could decide whether to always do interactive mode, to skip interactive mode when all args are given, to only query for args without default values, to never go into interactive mode ever, etc. maybe it should instead be a runtime switch given to forum2ui (or maybe both, with the runtime switch overriding the conf var)? tbh I'd say stick with (aka mimic) the *nix apps and NOT do interactive by default (but istty mbyes!) -- but provide tons of flexibility on an opt-in basis. I like the "proceed if all args have default values" and "don't query for args with default values" modes, but I wouldn't want to be forced into using them!!
QtCliUiGeneratorOutputCompilingTemplateExample::QtCliUiGeneratorOutputCompilingTemplateExample(QObject *parent)
    : QObject(parent)
    , m_StdIn(stdin)
    , m_StdOut(stdout)
{
    //connect(this, &QtCliUiGeneratorOutputCompilingTemplateExample::exitRequested, qApp, &QCoreApplication::exit, Qt::QueuedConnection);
}
bool QtCliUiGeneratorOutputCompilingTemplateExample::parseArgs()
{
    return m_ArgParser.parseArgs();
}
//instead of having a 'bool success' in my 'finished' signal, we simply do not emit if there was an error. we have not yet engaged the business object, so it is better to not engage it than to engage it only to report an error to it (might change this in the future, KISS for now). maybe s/engaged/invoked
void QtCliUiGeneratorOutputCompilingTemplateExample::collectUiVariables()
{
    QString firstName = query("First Name", m_ArgParser.firstNameDefaultValueParsedFromProcessArg());
    QString lastName = query("Last Name", m_ArgParser.lastNameDefaultValueParsedFromProcessArg());
    emit collectUiVariablesFinished(firstName, lastName);
}
//TODOreq: maybe some [proper] way to quit during a query xD?
QString QtCliUiGeneratorOutputCompilingTemplateExample::query(const QString &queryString, const QString &defaultValueParsedFromProcessArg)
{
    m_StdOut << queryString << " [" << defaultValueParsedFromProcessArg << "]: ";
    m_StdOut.flush();
    QString lineMaybe;
    do
    {
        lineMaybe = m_StdIn.readLine(); //TODOoptimization: readLineInto is better, but I don't have a recent enough version of Qt :(
        if(!defaultValueParsedFromProcessArg.isEmpty())
        {
            if(lineMaybe.isEmpty())
                return defaultValueParsedFromProcessArg; //or break; would have worked -- wait no it wouldn't have
        }
    }
    while(lineMaybe.isEmpty() || lineMaybe.isNull());
    return lineMaybe;
}
