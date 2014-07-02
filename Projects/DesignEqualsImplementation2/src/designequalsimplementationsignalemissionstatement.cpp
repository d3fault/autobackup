#include "designequalsimplementationsignalemissionstatement.h"

DesignEqualsImplementationSignalEmissionStatement::DesignEqualsImplementationSignalEmissionStatement(DesignEqualsImplementationClassSignal *signalToEmit, SignalEmissionOrSlotInvocationContextVariables signalEmissionContextVariablesForSignalArguments)
    : IDesignEqualsImplementationStatement()
    , m_SignalToEmit(signalToEmit)
    , m_SignalEmissionContextVariablesForSignalArguments(signalEmissionContextVariablesForSignalArguments)
{ }
DesignEqualsImplementationSignalEmissionStatement::~DesignEqualsImplementationSignalEmissionStatement()
{ }
QString DesignEqualsImplementationSignalEmissionStatement::toRawCppWithoutEndingSemicolon()
{
    QString ret("emit " + m_SignalToEmit->Name + "(");
    bool firstArg = true;
    Q_FOREACH(const QString &currentArgVarName, m_SignalEmissionContextVariablesForSignalArguments.OrderedListOfNamesOfVariablesWithinScopeWhenSlotInvocationOccurred_ToUseForSlotInvocationArguments)
    {
        if(!firstArg)
            ret.append(", ");
        ret.append(currentArgVarName);
        firstArg = false;
    }
    ret.append(")");
    return ret;
}
