#include "keyvaluestoremutation_modify.h"

KeyValueStoreMutation_Modify::KeyValueStoreMutation_Modify(const QString &newValue)
    : m_NewValue(newValue)
{ }
void KeyValueStoreMutation_Modify::appendYourselfToBulkMutationsJsonObject(const QString &key, QJsonObject &bulkMutations) const
{
    QJsonObject modifyMutation;
    modifyMutation.insert(mutationType(), m_NewValue);
    bulkMutations.insert(key, QJsonValue(modifyMutation)); //NOTE: key overwriting (dupe keys) was already checked for, during StupidKeyValueContentTracker::modify
}
void KeyValueStoreMutation_Modify::mutateCurrentStupidKeyValueContent(const QString &key, CurrentDataType *currentStupidKeyValueContent) const
{
    currentStupidKeyValueContent->insert(key, m_NewValue);
}
QString KeyValueStoreMutation_Modify::mutationType() const
{
    return KeyValueStoreMutation_Modify_MUTATIONTYPE;
}
