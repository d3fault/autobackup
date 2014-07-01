#ifndef DESIGNEQUALSIMPLEMENTATIONSLOTINVOCATIONSTATEMENT_H
#define DESIGNEQUALSIMPLEMENTATIONSLOTINVOCATIONSTATEMENT_H

#include "idesignequalsimplementationstatement.h"

#include <QString>
#include <QList>

class DesignEqualsImplementationClassSlot;

class DesignEqualsImplementationSlotInvocationStatement : public IDesignEqualsImplementationStatement
{
public:
    explicit DesignEqualsImplementationSlotInvocationStatement(DesignEqualsImplementationClassSlot *slotToInvoke, const QList<QString> &orderedListOfNamesOfVariablesWithinScopeWhenSlotInvocationOccurred_ToUseForSlotInvocationArguments = QList<QString>());
    virtual ~DesignEqualsImplementationSlotInvocationStatement();
    virtual QString toRawCppWithoutEndingSemicolon();
private:
    DesignEqualsImplementationClassSlot *m_SlotToInvoke;
    QList<QString> m_OrderedListOfNamesOfVariablesWithinScopeWhenSlotInvocationOccurred_ToUseForSlotInvocationArguments;
};

#endif // DESIGNEQUALSIMPLEMENTATIONSLOTINVOCATIONSTATEMENT_H
