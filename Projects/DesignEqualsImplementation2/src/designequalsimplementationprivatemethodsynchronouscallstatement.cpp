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
void DesignEqualsImplementationPrivateMethodSynchronousCallStatement::streamIn(QDataStream &in)
{
#if 0 //TODOreq: private method REFERENCE
    in >> *m_PrivateMethodToCall;
#endif
    in >> m_PrivateMethodArguments;
}
void DesignEqualsImplementationPrivateMethodSynchronousCallStatement::streamOut(QDataStream &out)
{
#if 0
    out << *m_PrivateMethodToCall;
#endif
    out << m_PrivateMethodArguments;
}
