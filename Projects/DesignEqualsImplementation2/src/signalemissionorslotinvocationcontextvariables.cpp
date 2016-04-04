#include "signalemissionorslotinvocationcontextvariables.h"

#include <QDataStream>

#ifndef TEMP_DONT_SERIALIZE_CONTEXTVARIABLES
void streamOutSignalEmissionOrSlotInvocationContextVariables(SignalEmissionOrSlotInvocationContextVariables *contextVariables, QDataStream &out)
{
    qint32 size = contextVariables->size();
    out << size;
    Q_FOREACH(const TypeInstance *currentContextVariable, contextVariables)
    {
        out << currentContextVariable->p
    }
}
#endif
