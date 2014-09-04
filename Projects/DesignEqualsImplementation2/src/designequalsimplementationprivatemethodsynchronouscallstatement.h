#ifndef DESIGNEQUALSIMPLEMENTATIONPRIVATEMETHODSYNCHRONOUSCALLSTATEMENT_H
#define DESIGNEQUALSIMPLEMENTATIONPRIVATEMETHODSYNCHRONOUSCALLSTATEMENT_H

#include "idesignequalsimplementationstatement.h"

#include "designequalsimplementationclassprivatemethod.h"
#include "signalemissionorslotinvocationcontextvariables.h"

class DesignEqualsImplementationPrivateMethodSynchronousCallStatement : public IDesignEqualsImplementationStatement
{
public:
    explicit DesignEqualsImplementationPrivateMethodSynchronousCallStatement() : IDesignEqualsImplementationStatement(PrivateMethodSynchronousCallStatementType) { }
    explicit DesignEqualsImplementationPrivateMethodSynchronousCallStatement(DesignEqualsImplementationClassPrivateMethod *privateMethodToCall, SignalEmissionOrSlotInvocationContextVariables privateMethodArguments);
protected:
    virtual QString toRawCppWithoutEndingSemicolon();

    virtual void streamIn(QDataStream &in);
    virtual void streamOut(QDataStream &out);
private:
    DesignEqualsImplementationClassPrivateMethod *m_PrivateMethodToCall;
    SignalEmissionOrSlotInvocationContextVariables m_PrivateMethodArguments;
};

#endif // DESIGNEQUALSIMPLEMENTATIONPRIVATEMETHODSYNCHRONOUSCALLSTATEMENT_H
