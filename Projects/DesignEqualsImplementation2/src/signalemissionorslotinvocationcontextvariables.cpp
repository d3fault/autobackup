#include "signalemissionorslotinvocationcontextvariables.h"

#if 0
#include <QDataStream>

#define SignalEmissionOrSlotInvocationContextVariables_QDS(qds, direction, signalEmissionOrSlotInvocationContextVariables) \
qds direction signalEmissionOrSlotInvocationContextVariables.OrderedListOfNamesOfVariablesWithinScopeWhenSignalEmissionOrSlotInvocationOccurrs_ToUseForSignalEmissionOrSlotInvocationArguments; \
return qds;

QDataStream &operator<<(QDataStream &out, const SignalEmissionOrSlotInvocationContextVariables &contextVariables)
{
    SignalEmissionOrSlotInvocationContextVariables_QDS(out, <<, contextVariables)
}
QDataStream &operator>>(QDataStream &in, SignalEmissionOrSlotInvocationContextVariables &contextVariables)
{
    SignalEmissionOrSlotInvocationContextVariables_QDS(in, >>, contextVariables)
}
#endif
