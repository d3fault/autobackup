#ifndef SIGNALEMISSIONORSLOTINVOCATIONCONTEXTVARIABLES_H
#define SIGNALEMISSIONORSLOTINVOCATIONCONTEXTVARIABLES_H

#include <QString>
#include <QList>

//#include "designequalsimplementationclasslifeline.h"

typedef QList<QString> SignalEmissionOrSlotInvocationContextVariables;

#if 0
struct SignalEmissionOrSlotInvocationContextVariables
{
    QList<QString> OrderedListOfNamesOfVariablesWithinScopeWhenSignalEmissionOrSlotInvocationOccurrs_ToUseForSignalEmissionOrSlotInvocationArguments; //Signal emissions and slot invocations both use "variables in current context" to populate teh args
};

QDataStream &operator<<(QDataStream &out, const SignalEmissionOrSlotInvocationContextVariables &contextVariables);
QDataStream &operator>>(QDataStream &in, SignalEmissionOrSlotInvocationContextVariables &contextVariables);
#endif

#endif // SIGNALEMISSIONORSLOTINVOCATIONCONTEXTVARIABLES_H
