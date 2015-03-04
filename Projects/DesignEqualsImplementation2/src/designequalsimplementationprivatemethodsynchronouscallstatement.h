#ifndef DESIGNEQUALSIMPLEMENTATIONPRIVATEMETHODSYNCHRONOUSCALLSTATEMENT_H
#define DESIGNEQUALSIMPLEMENTATIONPRIVATEMETHODSYNCHRONOUSCALLSTATEMENT_H

#include "idesignequalsimplementationstatement.h"

#include "designequalsimplementationclassprivatemethod.h"
#include "signalemissionorslotinvocationcontextvariables.h"

class DesignEqualsImplementationPrivateMethodSynchronousCallStatement : public IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements
{
public:
    explicit DesignEqualsImplementationPrivateMethodSynchronousCallStatement() : IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements(PrivateMethodSynchronousCallStatementType) { }
    explicit DesignEqualsImplementationPrivateMethodSynchronousCallStatement(DesignEqualsImplementationClassPrivateMethod *privateMethodToCall, SignalEmissionOrSlotInvocationContextVariables privateMethodArguments);
protected:
    virtual QString toRawCppWithoutEndingSemicolon();

    virtual void streamIn(DesignEqualsImplementationProject *project, QDataStream &in);
    virtual void streamOut(DesignEqualsImplementationProject *project, QDataStream &out);
private:
    DesignEqualsImplementationClassPrivateMethod *m_PrivateMethodToCall;
    SignalEmissionOrSlotInvocationContextVariables m_PrivateMethodArguments;
};

#endif // DESIGNEQUALSIMPLEMENTATIONPRIVATEMETHODSYNCHRONOUSCALLSTATEMENT_H
