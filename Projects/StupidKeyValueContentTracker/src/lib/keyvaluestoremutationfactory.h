#ifndef KEYVALUESTOREMUTATIONFACTORY_H
#define KEYVALUESTOREMUTATIONFACTORY_H

#include "stupidkeyvaluecontenttrackertypes.h"

class KeyValueStoreMutationFactory
{
public:
    static StupidKeyValueContentTracker_StagedMutationsValueType createKeyValueStoreMutation(const QString &mutationType, const QString &mutationValue);
    KeyValueStoreMutationFactory()=delete;
};

#endif // KEYVALUESTOREMUTATIONFACTORY_H
