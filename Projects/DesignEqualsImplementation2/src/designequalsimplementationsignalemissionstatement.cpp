#include "designequalsimplementationsignalemissionstatement.h"

#include <QDataStream>

DesignEqualsImplementationSignalEmissionStatement::DesignEqualsImplementationSignalEmissionStatement()
    : IDesignEqualsImplementationStatement(SignalEmitStatementType) //trivial constructor required for deserializing only. don't use elsewhere
{ }
DesignEqualsImplementationSignalEmissionStatement::DesignEqualsImplementationSignalEmissionStatement(DesignEqualsImplementationClassSignal *signalToEmit, SignalEmissionOrSlotInvocationContextVariables signalEmissionContextVariablesForSignalArguments)
    : IDesignEqualsImplementationStatement(IDesignEqualsImplementationStatement::SignalEmitStatementType)
    , m_SignalToEmit(signalToEmit)
    , m_SignalEmissionContextVariablesForSignalArguments(signalEmissionContextVariablesForSignalArguments)
{ }
DesignEqualsImplementationSignalEmissionStatement::~DesignEqualsImplementationSignalEmissionStatement()
{ }
QString DesignEqualsImplementationSignalEmissionStatement::toRawCppWithoutEndingSemicolon()
{
    QString ret("emit " + m_SignalToEmit->Name + "(");
    bool firstArg = true;
    Q_FOREACH(const QString &currentArgVarName, m_SignalEmissionContextVariablesForSignalArguments)
    {
        if(!firstArg)
            ret.append(", ");
        ret.append(currentArgVarName);
        firstArg = false;
    }
    ret.append(")");
    return ret;
}
DesignEqualsImplementationClassSignal *DesignEqualsImplementationSignalEmissionStatement::signalToEmit() const
{
    return m_SignalToEmit;
}
void DesignEqualsImplementationSignalEmissionStatement::streamIn(QDataStream &in)
{
#if 0 //TODOreq: signal REFERENCE
    in >> *m_SignalToEmit;
#endif
    in >> m_SignalEmissionContextVariablesForSignalArguments;
}
void DesignEqualsImplementationSignalEmissionStatement::streamOut(QDataStream &out)
{
#if 0
    out << *m_SignalToEmit; //TODOreq: __REFERENCE to m_SignalToEmit (or use compression on known-to-compact-nicely ;-)) repeatetive uses of the same signal? keep in mind that you definitely still need the signal to be defined, but that is of course done when serializing the signal itself!. for now idgaf about dupes
#endif
    out << m_SignalEmissionContextVariablesForSignalArguments;
}
