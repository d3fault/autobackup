#ifndef KEYVALUESTOREMUTATION_ADD_H
#define KEYVALUESTOREMUTATION_ADD_H

#include "ikeyvaluestoremutation.h"

class KeyValueStoreMutation_Add : public IKeyValueStoreMutation
{
public:
    KeyValueStoreMutation_Add(const QString &dataToAdd);
    void appendYourselfToBulkMutationsJsonObject(const QString &key, QJsonObject &bulkMutations) override;
protected:
    QString mutationType() const override;
private:
    QString m_DataToAdd;
};

#endif // KEYVALUESTOREMUTATION_ADD_H
