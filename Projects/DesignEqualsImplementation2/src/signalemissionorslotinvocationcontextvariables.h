#ifndef SIGNALEMISSIONORSLOTINVOCATIONCONTEXTVARIABLES_H
#define SIGNALEMISSIONORSLOTINVOCATIONCONTEXTVARIABLES_H

#include "designequalsimplementationtype.h"

class SignalEmissionOrSlotInvocationContextVariables : public QList<TypeInstance*>
{
public:
    static void streamIn(DesignEqualsImplementationProject *project, SignalEmissionOrSlotInvocationContextVariables *contextVariables, QDataStream &in);
    static void streamOut(DesignEqualsImplementationProject *project, SignalEmissionOrSlotInvocationContextVariables *contextVariables, QDataStream &out);
};

#endif // SIGNALEMISSIONORSLOTINVOCATIONCONTEXTVARIABLES_H
