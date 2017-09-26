#include "keyvaluestoremutation_add.h"

KeyValueStoreMutation_Add::KeyValueStoreMutation_Add(const QString &dataToAdd)
    : m_DataToAdd(dataToAdd)
{ }
void KeyValueStoreMutation_Add::appendYourselfToBulkMutationsJsonObject(const QString &key, QJsonObject &bulkMutations)
{
    QJsonObject addMutation;
    addMutation.insert(mutationType(), m_DataToAdd);
    bulkMutations.insert(key, QJsonValue(addMutation)); //NOTE: key overwriting (dupe keys) was already checked for, during StupidKeyValueContentTracker::add. TODOreq: obv need to do dupe checking for delete/modify mutations
}
QString KeyValueStoreMutation_Add::mutationType() const
{
    return "add";
}
