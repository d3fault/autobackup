#include "idesignequalsimplementationmethod.h"

IDesignEqualsImplementationMethod::IDesignEqualsImplementationMethod()
{ }
IDesignEqualsImplementationMethod::~IDesignEqualsImplementationMethod()
{
    qDeleteAll(Arguments);
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
    Q_FOREACH(DesignEqualsImplementationClassMethodArgument *currentArgument, Arguments)
    {
        if(!firstArg)
            argString.append(", ");
        argString.append(currentArgument->preferredTextualRepresentation());
        firstArg = false;
    }
    return argString;
}
