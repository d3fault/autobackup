#include "keyvaluestoremutation_remove.h"


void KeyValueStoreMutation_Remove::appendYourselfToBulkMutationsJsonObject(const QString &key, QJsonObject &bulkMutations) const
{
    QJsonObject removeMutation;
    removeMutation.insert(mutationType(), true); //remove=true. the 'value' (bool=true) is used in add/modify, but is pretty much useless here for 'remove'
    bulkMutations.insert(key, QJsonValue(removeMutation)); //NOTE: key overwriting (dupe keys) was already checked for, during StupidKeyValueContentTracker::removeKey
}
void KeyValueStoreMutation_Remove::mutateCurrentStupidKeyValueContent(const QString &key, CurrentDataType *currentStupidKeyValueContent) const
{
    currentStupidKeyValueContent->remove(key);
}
QString KeyValueStoreMutation_Remove::mutationType() const
{
    return KeyValueStoreMutation_Remove_MUTATIONTYPE;
}
