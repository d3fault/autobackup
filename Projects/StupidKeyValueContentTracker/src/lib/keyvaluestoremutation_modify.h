#ifndef KEYVALUESTOREMUTATION_MODIFY_H
#define KEYVALUESTOREMUTATION_MODIFY_H

#include "ikeyvaluestoremutation.h"

#define KeyValueStoreMutation_Modify_MUTATIONTYPE "modify"

class KeyValueStoreMutation_Modify : public IKeyValueStoreMutation
{
public:
    KeyValueStoreMutation_Modify(const QString &newValue);
    void appendYourselfToBulkMutationsJsonObject(const QString &key, QJsonObject &bulkMutations) const override;
    void mutateCurrentStupidKeyValueContent(const QString &key, CurrentDataType *currentStupidKeyValueContent) const override;
protected:
    QString mutationType() const override;
private:
    QString m_NewValue;
};

#endif // KEYVALUESTOREMUTATION_MODIFY_H
