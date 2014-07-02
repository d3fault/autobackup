#ifndef SLOTINVOCATIONCONTEXTVARIABLES_H
#define SLOTINVOCATIONCONTEXTVARIABLES_H

#include <QString>
#include <QList>

struct SlotInvocationContextVariables
{
    QString VariableNameOfObjectInCurrentContextWhoseSlotIsAboutToBeInvoked;
    QList<QString> OrderedListOfNamesOfVariablesWithinScopeWhenSlotInvocationOccurred_ToUseForSlotInvocationArguments;
};

#endif // SLOTINVOCATIONCONTEXTVARIABLES_H
