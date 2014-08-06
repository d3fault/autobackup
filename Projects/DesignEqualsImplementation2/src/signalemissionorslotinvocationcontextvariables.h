#ifndef SIGNALEMISSIONORSLOTINVOCATIONCONTEXTVARIABLES_H
#define SIGNALEMISSIONORSLOTINVOCATIONCONTEXTVARIABLES_H

#include <QString>
#include <QList>

#include "designequalsimplementationclasslifeline.h"

struct SignalEmissionOrSlotInvocationContextVariables
{
    DesignEqualsImplementationClassLifeLine *ClassLifelineWhoseSlotIsAboutToBeInvoked; //Slot only. TODOoptional: move to interface so signals don't have+ignore this
    QList<QString> OrderedListOfNamesOfVariablesWithinScopeWhenSignalEmissionOrSlotInvocationOccurrs_ToUseForSignalEmissionOrSlotInvocationArguments; //Signal emissions and slot invocations both use "variables in current context" to populate teh args
};

#endif // SIGNALEMISSIONORSLOTINVOCATIONCONTEXTVARIABLES_H
