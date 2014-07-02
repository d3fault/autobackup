#ifndef DESIGNEQUALSIMPLEMENTATIONSIGNALEMISSIONSTATEMENT_H
#define DESIGNEQUALSIMPLEMENTATIONSIGNALEMISSIONSTATEMENT_H

#include "idesignequalsimplementationstatement.h"

#include "designequalsimplementationclasssignal.h"
#include "signalemissionorslotinvocationcontextvariables.h"

class DesignEqualsImplementationSignalEmissionStatement : public IDesignEqualsImplementationStatement
{
public:
    explicit DesignEqualsImplementationSignalEmissionStatement(DesignEqualsImplementationClassSignal *signalToEmit, SignalEmissionOrSlotInvocationContextVariables signalEmissionContextVariablesForSignalArguments);
    virtual ~DesignEqualsImplementationSignalEmissionStatement();
    virtual QString toRawCppWithoutEndingSemicolon();
private:
    DesignEqualsImplementationClassSignal *m_SignalToEmit;
    SignalEmissionOrSlotInvocationContextVariables m_SignalEmissionContextVariablesForSignalArguments;
};

#endif // DESIGNEQUALSIMPLEMENTATIONSIGNALEMISSIONSTATEMENT_H
