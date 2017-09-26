#ifndef IKEYVALUESTOREMUTATION_H
#define IKEYVALUESTOREMUTATION_H

#include <QJsonObject>
#include <QJsonValue>

#include "stupidkeyvaluecontenttrackertypes.h"

class IKeyValueStoreMutation
{
public:
    virtual void appendYourselfToBulkMutationsJsonObject(const QString &key, QJsonObject &bulkMutations) const=0;
    virtual void mutateCurrentStupidKeyValueContent(const QString &key, CurrentDataType *currentStupidKeyValueContent) const=0;
protected:
    virtual QString mutationType() const=0;
};

#endif // IKEYVALUESTOREMUTATION_H
