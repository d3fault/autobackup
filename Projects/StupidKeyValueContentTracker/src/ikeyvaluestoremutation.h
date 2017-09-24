#ifndef IKEYVALUESTOREMUTATION_H
#define IKEYVALUESTOREMUTATION_H

#include <QJsonObject>
#include <QJsonValue>

class IKeyValueStoreMutation
{
public:
    virtual void appendYourselfToBulkMutationsJsonObject(const QString &key, QJsonObject &bulkMutations)=0;
protected:
    virtual QString mutationType() const=0;
};

#endif // IKEYVALUESTOREMUTATION_H
