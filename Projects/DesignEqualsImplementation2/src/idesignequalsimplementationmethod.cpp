#include "idesignequalsimplementationmethod.h"

IDesignEqualsImplementationMethod::IDesignEqualsImplementationMethod()
{ }
IDesignEqualsImplementationMethod::~IDesignEqualsImplementationMethod()
{
    qDeleteAll(m_Arguments);
}
DesignEqualsImplementationClassMethodArgument *IDesignEqualsImplementationMethod::createNewArgument(const QString &argumentType, const QString &argumentVariableName)
{
    DesignEqualsImplementationClassMethodArgument *newArgument = new DesignEqualsImplementationClassMethodArgument(); //TODOreq: can't set parent because i'm not a qobject even though all inheriters are
    newArgument->Type = argumentType;
    newArgument->VariableName = argumentVariableName;
    m_Arguments.append(newArgument);
    return newArgument;
}
QList<DesignEqualsImplementationClassMethodArgument *> IDesignEqualsImplementationMethod::arguments()
{
    return m_Arguments;
}
QString IDesignEqualsImplementationMethod::methodSignatureWithoutReturnType()
{
    return Name + "(" + argumentsToCommaSeparatedString() + ")";
}
//TODOmb: default variables for slots (only in header obviously), BUT not compatible with Qt5's new connect syntax anyways so.... (so don't use default variables i guess)
QString IDesignEqualsImplementationMethod::argumentsToCommaSeparatedString()
{
    QString argString;
    bool firstArg = true;
    Q_FOREACH(DesignEqualsImplementationClassMethodArgument *currentArgument, arguments())
    {
        if(!firstArg)
            argString.append(", ");
        argString.append(currentArgument->preferredTextualRepresentation());
        firstArg = false;
    }
    return argString;
}
