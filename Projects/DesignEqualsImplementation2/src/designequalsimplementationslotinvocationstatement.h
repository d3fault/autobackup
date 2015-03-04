#ifndef DESIGNEQUALSIMPLEMENTATIONSLOTINVOCATIONSTATEMENT_H
#define DESIGNEQUALSIMPLEMENTATIONSLOTINVOCATIONSTATEMENT_H

#include "idesignequalsimplementationstatement.h"

#include <QString>
#include <QList>

#include "signalemissionorslotinvocationcontextvariables.h"


class DesignEqualsImplementationClassSlot;
class DesignEqualsImplementationClassLifeLine;

class DesignEqualsImplementationSlotInvocationStatement : public IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements
{
public:
    explicit DesignEqualsImplementationSlotInvocationStatement() : IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements(SlotInvokeStatementType) { }
    explicit DesignEqualsImplementationSlotInvocationStatement(DesignEqualsImplementationClassLifeLine *classLifelineWhoseSlotIsAboutToBeInvoked, DesignEqualsImplementationClassSlot *slotToInvoke, const SignalEmissionOrSlotInvocationContextVariables &slotInvocationContextVariables);
    virtual ~DesignEqualsImplementationSlotInvocationStatement();
    virtual QString toRawCppWithoutEndingSemicolon();
    virtual bool isSlotInvoke() { return true; }
    const SignalEmissionOrSlotInvocationContextVariables &slotInvocationContextVariables();
    DesignEqualsImplementationClassSlot *slotToInvoke();

    virtual void streamIn(DesignEqualsImplementationProject *project, QDataStream &in);
    virtual void streamOut(DesignEqualsImplementationProject *project, QDataStream &out);
    DesignEqualsImplementationClassLifeLine *classLifelineWhoseSlotIsToBeInvoked() const;
private:
    DesignEqualsImplementationClassLifeLine *m_ClassLifelineWhoseSlotIsToBeInvoked;
    DesignEqualsImplementationClassSlot *m_SlotToInvoke;
    SignalEmissionOrSlotInvocationContextVariables m_SlotInvocationContextVariables;
};

#endif // DESIGNEQUALSIMPLEMENTATIONSLOTINVOCATIONSTATEMENT_H
