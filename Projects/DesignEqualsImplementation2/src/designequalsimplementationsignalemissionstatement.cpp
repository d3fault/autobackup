#include "designequalsimplementationsignalemissionstatement.h"

#include <QDataStream>

DesignEqualsImplementationSignalEmissionStatement::DesignEqualsImplementationSignalEmissionStatement()
    : IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements(SignalEmitStatementType) //trivial constructor required for deserializing only. don't use elsewhere
{ }
DesignEqualsImplementationSignalEmissionStatement::DesignEqualsImplementationSignalEmissionStatement(DesignEqualsImplementationClassSignal *signalToEmit, SignalEmissionOrSlotInvocationContextVariables signalEmissionContextVariablesForSignalArguments)
    : IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements(IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements::SignalEmitStatementType)
    , m_SignalToEmit(signalToEmit)
    , m_SignalEmissionContextVariablesForSignalArguments(signalEmissionContextVariablesForSignalArguments)
{ }
DesignEqualsImplementationSignalEmissionStatement::~DesignEqualsImplementationSignalEmissionStatement()
{ }
QString DesignEqualsImplementationSignalEmissionStatement::toRawCppWithoutEndingSemicolon()
{
    QString ret("emit " + m_SignalToEmit->Name + "(");
    bool firstArg = true;
    Q_FOREACH(const TypeInstance *currentArg, m_SignalEmissionContextVariablesForSignalArguments)
    {
        if(!firstArg)
            ret.append(", ");
        ret.append(currentArg->VariableName);
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
    SignalEmissionOrSlotInvocationContextVariables::streamIn(project, &m_SignalEmissionContextVariablesForSignalArguments, in);
}
void DesignEqualsImplementationSignalEmissionStatement::streamOut(DesignEqualsImplementationProject *project, QDataStream &out)
{
    DesignEqualsImplementationClassSignal::streamOutSignalReference(project, m_SignalToEmit, out);
    SignalEmissionOrSlotInvocationContextVariables::streamOut(project, &m_SignalEmissionContextVariablesForSignalArguments, out);
}
