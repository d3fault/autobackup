#include "keyvaluestoremutationfactory.h"

#include "keyvaluestoremutation_add.h"
#include "keyvaluestoremutation_modify.h"
#include "keyvaluestoremutation_remove.h"

StupidKeyValueContentTracker_StagedMutationsValueType KeyValueStoreMutationFactory::createKeyValueStoreMutation(const QString &mutationType, const QString &mutationValue)
{
    if(mutationType == KeyValueStoreMutation_Add_MUTATIONTYPE)
    {
        return StupidKeyValueContentTracker_StagedMutationsValueType(new KeyValueStoreMutation_Add(mutationValue));
    }
    else if(mutationType == KeyValueStoreMutation_Modify_MUTATIONTYPE)
    {
        return StupidKeyValueContentTracker_StagedMutationsValueType(new KeyValueStoreMutation_Modify(mutationValue));
    }
    else if(mutationType == KeyValueStoreMutation_Remove_MUTATIONTYPE)
    {
        Q_UNUSED(mutationValue);
        return StupidKeyValueContentTracker_StagedMutationsValueType(new KeyValueStoreMutation_Remove());
    }
    else
    {
        qFatal("unknown mutation type");
        return StupidKeyValueContentTracker_StagedMutationsValueType(new KeyValueStoreMutation_Add("wont-get-here"));
    }
}
