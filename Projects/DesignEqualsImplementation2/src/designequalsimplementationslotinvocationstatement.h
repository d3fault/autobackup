#ifndef DESIGNEQUALSIMPLEMENTATIONSLOTINVOCATIONSTATEMENT_H
#define DESIGNEQUALSIMPLEMENTATIONSLOTINVOCATIONSTATEMENT_H

#include "idesignequalsimplementationstatement.h"

#include <QString>
#include <QList>

#include "slotinvocationcontextvariables.h"

class DesignEqualsImplementationClassSlot;

class DesignEqualsImplementationSlotInvocationStatement : public IDesignEqualsImplementationStatement
{
public:
    explicit DesignEqualsImplementationSlotInvocationStatement(DesignEqualsImplementationClassSlot *slotToInvoke, const SlotInvocationContextVariables &slotInvocationContextVariables);
    virtual ~DesignEqualsImplementationSlotInvocationStatement();
    virtual QString toRawCppWithoutEndingSemicolon();
private:
    DesignEqualsImplementationClassSlot *m_SlotToInvoke;
    SlotInvocationContextVariables m_SlotInvocationContextVariables;
};

#endif // DESIGNEQUALSIMPLEMENTATIONSLOTINVOCATIONSTATEMENT_H
