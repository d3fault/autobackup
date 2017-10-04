#include "keyvaluestoremutationfactory.h"

#include "keyvaluestoremutation_add.h"

StupidKeyValueContentTracker_StagedMutationsValueType KeyValueStoreMutationFactory::createKeyValueStoreMutation(const QString &mutationType, const QString &mutationValue)
{
    if(mutationType == KeyValueStoreMutation_Add_MUTATIONTYPE)
    {
        return StupidKeyValueContentTracker_StagedMutationsValueType(new KeyValueStoreMutation_Add(mutationValue));
    }
    //TODOreq: else if modify delete etc
    else
    {
        qFatal("unknown mutation type");
        return StupidKeyValueContentTracker_StagedMutationsValueType(new KeyValueStoreMutation_Add("wont-get-here"));
    }
}
