#include "qtcliuigeneratoroutputcompilingtemplateexample.h"

#include <QCoreApplication>
#include <QStringList>
#include <QMutableListIterator>
#include <QDebug>

//TODOmb: only go into "interactive" (query->readLine) mode if --interactive is given. this is how most *nix apps are written. "fail loudly". tons of scripts would break if the default routine when an arg is _forgotten_ was to drop into query mode. but actually a test of istty could tell us whether or not we should drop to query mode or if the process should simply show cliUsage and return 1; TODOreq: think hard on this, it's a great idea but not portable xD. currently I always go into interactive mode, even if there are no args to collect. maybe a (or some) configuration var inside "format" (a la format2ui) could decide whether to always do interactive mode, to skip interactive mode when all args are given, to only query for args without default values, to never go into interactive mode ever, etc. maybe it should instead be a runtime switch given to forum2ui (or maybe both, with the runtime switch overriding the conf var)? tbh I'd say stick with (aka mimic) the *nix apps and NOT do interactive by default (but istty mbyes!) -- but provide tons of flexibility on an opt-in basis. I like the "proceed if all args have default values" and "don't query for args with default values" modes, but I wouldn't want to be forced into using them!!
QtCliUiGeneratorOutputCompilingTemplateExample::QtCliUiGeneratorOutputCompilingTemplateExample(QObject *parent)
    : QObject(parent)
    , m_StdIn(stdin)
    , m_StdOut(stdout)
    , m_StdErr(stderr)
{
    //connect(this, &QtCliUiGeneratorOutputCompilingTemplateExample::exitRequested, qApp, &QCoreApplication::exit, Qt::QueuedConnection);
}
bool QtCliUiGeneratorOutputCompilingTemplateExample::parseArgs()
{
    //TODOoptional: use QCommandLineParser. but I think since I'm a code gen that the benefits of QCommandLineParser will become moot. they are for non code-gen application writing

    //TODOreq: [optionally] parse firstName and lastName, use as default values in query prompt
    QStringList argz = qApp->arguments();
    argz.removeFirst(); //fileName of app

#if 0
    while(!argz.isEmpty())
    {
        QString currentArg = argz.takeFirst();

        if(currentArg == "--firstname")
        {
            m_FirstNameDefaultValueParsedFromProcessArg = currentArg.at(1); //TODOreq: this is unssafe!
            argz.removeFirst(); //--firstname
            argz.removeFirst(); //the first name //TODOreq: this is unsafe!
        }
        else if(currentArg == "--lastname")
        {
            m_LastDefaultValueParsedFromProcessArg = currentArg.at(1); //TODOreq: this is unssafe!
            argz.removeFirst(); //--firstname
            argz.removeFirst(); //the first name //TODOreq: this is unsafe!
        }

    }
#endif

    //TODOmb: put default values into a QHash<QString /*key_aka_FirstName*/, QString /*value_aka_FirstNameDefaultValue*/>. actually come to think of it, that will make it iteratable and that means no code duplication _here_ (below) -- still it's practically wasted effort (FOR NOW) because this code is being generated, so "dupe code" is not an issue (but since the end code might be looked at, keeping it in a QHash has a benefit and I should implement this later on)
    if(!parseOptionalArgAndHandleErrors(&argz, "firstName", &m_FirstNameDefaultValueParsedFromProcessArg))
    {
        showUsage();
        return false;
    }
    if(!parseOptionalArgAndHandleErrors(&argz, "lastName", &m_LastNameDefaultValueParsedFromProcessArg))
    {
        showUsage();
        return false;
    }
    return true;
}
//instead of having a 'bool success' in my 'finished' signal, we simply do not emit if there was an error. we have not yet engaged the business object, so it is better to not engage it than to engage it only to report an error to it (might change this in the future, KISS for now). maybe s/engaged/invoked
void QtCliUiGeneratorOutputCompilingTemplateExample::collectUiVariables()
{
    QString firstName = query("First Name", m_FirstNameDefaultValueParsedFromProcessArg);
    QString lastName = query("Last Name", m_LastNameDefaultValueParsedFromProcessArg);
    emit collectUiVariablesFinished(firstName, lastName);
}
int QtCliUiGeneratorOutputCompilingTemplateExample::parseOptionalArg(QStringList *argz, const QString &optionalArg, QString *out_String)
{
    int ret = 1; //1 == NotFound, 0 == Found, -1 == Error
    QString argThingToLookFor = "--" + optionalArg.toLower();
    QMutableListIterator<QString> it(*argz);
    while(it.hasNext())
    {
        const QString &currentArg = it.next();
        if(currentArg.toLower() == argThingToLookFor)
        {
            if(!it.hasNext())
            {
                //ERROR: TODOreq:
                qCritical() << "you did not provide a mandatory value for: " << currentArg; //the arg value is mandatory when the arg key is present. that's not to say that the arg is mandatory on the whole; it isn't
                return -1;
            }
            it.remove();
            *out_String = it.next();
            it.remove();
            return 0;
        }
    }
    return ret;
}
//TODOreq: maybe some [proper] way to quit during a query xD?
QString QtCliUiGeneratorOutputCompilingTemplateExample::query(const QString &queryString, const QString &defaultValueParsedFromProcessArg)
{
    m_StdOut << queryString << "[" << defaultValueParsedFromProcessArg << "]: ";
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
void QtCliUiGeneratorOutputCompilingTemplateExample::showUsage() //used to be called showUsageAndErrorOut
{
    m_StdErr << "Usage: QtCliUiGeneratorOutputCompilingTemplateExample [--firstname Joe] [--lastname Schmoe]" << endl;
    //emit exitRequested(1); //TODOmb: just set some m_Success property to false and let the instantiator of this object do the actual exitting?
}
bool QtCliUiGeneratorOutputCompilingTemplateExample::parseOptionalArgAndHandleErrors(QStringList *argz, const QString &optionalArg, QString *out_String)
{
    int success = parseOptionalArg(argz, optionalArg, out_String);
    if(success == -1)
        return false;
    return true;
}
