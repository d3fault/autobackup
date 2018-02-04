#include "firstnamelastnameqobjectcommandlineoptionparser.h"

#include <QCoreApplication>
#include <QStringList>
#include <QDebug>

FirstNameLastNameQObjectCommandLineOptionParser::FirstNameLastNameQObjectCommandLineOptionParser()
    : m_StdErr(stderr)
{ }

bool FirstNameLastNameQObjectCommandLineOptionParser::parseArgs()
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
int FirstNameLastNameQObjectCommandLineOptionParser::parseOptionalArg(QStringList *argz, const QString &optionalArg, QString *out_String)
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
bool FirstNameLastNameQObjectCommandLineOptionParser::parseOptionalArgAndHandleErrors(QStringList *argz, const QString &optionalArg, QString *out_String)
{
    int success = parseOptionalArg(argz, optionalArg, out_String);
    if(success == -1)
        return false;
    return true;
}
QString FirstNameLastNameQObjectCommandLineOptionParser::firstNameDefaultValueParsedFromProcessArg() const
{
    return m_FirstNameDefaultValueParsedFromProcessArg;
}
QString FirstNameLastNameQObjectCommandLineOptionParser::lastNameDefaultValueParsedFromProcessArg() const
{
    return m_LastNameDefaultValueParsedFromProcessArg;
}
void FirstNameLastNameQObjectCommandLineOptionParser::showUsage() //used to be called showUsageAndErrorOut
{
    m_StdErr << "Usage: ./app [--firstname Joe] [--lastname Schmoe]" << endl;
    //emit exitRequested(1); //TODOmb: just set some m_Success property to false and let the instantiator of this object do the actual exitting?
}
