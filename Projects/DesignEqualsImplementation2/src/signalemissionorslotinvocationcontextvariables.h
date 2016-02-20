#ifndef SIGNALEMISSIONORSLOTINVOCATIONCONTEXTVARIABLES_H
#define SIGNALEMISSIONORSLOTINVOCATIONCONTEXTVARIABLES_H

#include <QString>
#include <QList>

//#include "designequalsimplementationclasslifeline.h"

typedef QList<QString> SignalEmissionOrSlotInvocationContextVariables; //TODOreq: fuck, if a signal emit 'arg' is a member of the class, and we rename that member later, finding it in all use cases' signal slot connection activations for a given class will be a bitch. I should have had all classes/properties/defined-elsewhere-types/ETC all inherit from the same type: Type... or some such

#if 0
struct SignalEmissionOrSlotInvocationContextVariables
{
    QList<QString> OrderedListOfNamesOfVariablesWithinScopeWhenSignalEmissionOrSlotInvocationOccurrs_ToUseForSignalEmissionOrSlotInvocationArguments; //Signal emissions and slot invocations both use "variables in current context" to populate teh args
};

QDataStream &operator<<(QDataStream &out, const SignalEmissionOrSlotInvocationContextVariables &contextVariables);
QDataStream &operator>>(QDataStream &in, SignalEmissionOrSlotInvocationContextVariables &contextVariables);
#endif

#endif // SIGNALEMISSIONORSLOTINVOCATIONCONTEXTVARIABLES_H
