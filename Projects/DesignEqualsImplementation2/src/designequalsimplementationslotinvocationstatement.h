#ifndef DESIGNEQUALSIMPLEMENTATIONSLOTINVOCATIONSTATEMENT_H
#define DESIGNEQUALSIMPLEMENTATIONSLOTINVOCATIONSTATEMENT_H

#include "idesignequalsimplementationstatement.h"

#include <QString>
#include <QList>

#include "signalemissionorslotinvocationcontextvariables.h"


class DesignEqualsImplementationClassSlot;
class DesignEqualsImplementationClassLifeLine;

class DesignEqualsImplementationSlotInvocationStatement : public IDesignEqualsImplementationStatement
{
public:
    explicit DesignEqualsImplementationSlotInvocationStatement() : IDesignEqualsImplementationStatement(SlotInvokeStatementType) { }
    explicit DesignEqualsImplementationSlotInvocationStatement(DesignEqualsImplementationClassLifeLine *classLifelineWhoseSlotIsAboutToBeInvoked, DesignEqualsImplementationClassSlot *slotToInvoke, const SignalEmissionOrSlotInvocationContextVariables &slotInvocationContextVariables);
    virtual ~DesignEqualsImplementationSlotInvocationStatement();
    virtual QString toRawCppWithoutEndingSemicolon();
    virtual bool isSlotInvoke() { return true; }
    const SignalEmissionOrSlotInvocationContextVariables &slotInvocationContextVariables();
    DesignEqualsImplementationClassSlot *slotToInvoke();

    virtual void streamIn(QDataStream &in);
    virtual void streamOut(QDataStream &out);
    DesignEqualsImplementationClassLifeLine *classLifelineWhoseSlotIsToBeInvoked() const;
private:
    DesignEqualsImplementationClassLifeLine *m_ClassLifelineWhoseSlotIsToBeInvoked;
    DesignEqualsImplementationClassSlot *m_SlotToInvoke;
    SignalEmissionOrSlotInvocationContextVariables m_SlotInvocationContextVariables;
};

#endif // DESIGNEQUALSIMPLEMENTATIONSLOTINVOCATIONSTATEMENT_H
