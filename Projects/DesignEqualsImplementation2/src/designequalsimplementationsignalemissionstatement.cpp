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
void DesignEqualsImplementationSignalEmissionStatement::streamIn(DesignEqualsImplementationProject *project, QDataStream &in)
{
    m_SignalToEmit = DesignEqualsImplementationClassSignal::streamInSignalReference(project, in);
    in >> m_SignalEmissionContextVariablesForSignalArguments;
}
void DesignEqualsImplementationSignalEmissionStatement::streamOut(DesignEqualsImplementationProject *project, QDataStream &out)
{
    DesignEqualsImplementationClassSignal::streamOutSignalReference(project, m_SignalToEmit, out);
    out << m_SignalEmissionContextVariablesForSignalArguments;
}
