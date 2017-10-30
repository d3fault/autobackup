#ifndef KEYVALUESTOREMUTATION_REMOVE_H
#define KEYVALUESTOREMUTATION_REMOVE_H

#include "ikeyvaluestoremutation.h"

#define KeyValueStoreMutation_Remove_MUTATIONTYPE "remove"

class KeyValueStoreMutation_Remove : public IKeyValueStoreMutation
{
public:
    void appendYourselfToBulkMutationsJsonObject(const QString &key, QJsonObject &bulkMutations) const override;
    void mutateCurrentStupidKeyValueContent(const QString &key, CurrentDataType *currentStupidKeyValueContent) const override;
protected:
    QString mutationType() const override;
};

#endif // KEYVALUESTOREMUTATION_REMOVE_H
