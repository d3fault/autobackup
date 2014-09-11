#include "designequalsimplementationprivatemethodsynchronouscallstatement.h"

#include <QDataStream>

//TODOoptional: maybe ctrl modifier when drawing a signal (or hell, even a signal->slot invocation... but nah) makes it synchronous private method call
//TODOreq: utilize in GUI. drop-down mode would ofc be nice, but that's post-1.0 imo
DesignEqualsImplementationPrivateMethodSynchronousCallStatement::DesignEqualsImplementationPrivateMethodSynchronousCallStatement(DesignEqualsImplementationClassPrivateMethod *privateMethodToCall, SignalEmissionOrSlotInvocationContextVariables privateMethodArguments)
    : IDesignEqualsImplementationStatement(PrivateMethodSynchronousCallStatementType)
    , m_PrivateMethodToCall(privateMethodToCall)
    , m_PrivateMethodArguments(privateMethodArguments)
{ }
QString DesignEqualsImplementationPrivateMethodSynchronousCallStatement::toRawCppWithoutEndingSemicolon()
{
    //like a slot invoke or signal emit, but a good old c++ call
    QString ret(m_PrivateMethodToCall->Name + "(");
    bool firstArg = true;
    Q_FOREACH(const QString &currentArgVarName, m_PrivateMethodArguments)
    {
        if(!firstArg)
            ret.append(", ");
        ret.append(currentArgVarName);
        firstArg = false;
    }
    ret.append(")");
    return ret;
}
void DesignEqualsImplementationPrivateMethodSynchronousCallStatement::streamIn(DesignEqualsImplementationProject *project, QDataStream &in)
{
    DesignEqualsImplementationClassPrivateMethod::streamInPrivateMethodReference(project, in);
    in >> m_PrivateMethodArguments;
}
void DesignEqualsImplementationPrivateMethodSynchronousCallStatement::streamOut(DesignEqualsImplementationProject *project, QDataStream &out)
{
    DesignEqualsImplementationClassPrivateMethod::streamOutPrivateMethodReference(project, m_PrivateMethodToCall, out);
    out << m_PrivateMethodArguments;
}
