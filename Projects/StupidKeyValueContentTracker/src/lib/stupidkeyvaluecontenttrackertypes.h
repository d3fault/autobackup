#ifndef STUPIDKEYVALUECONTENTTRACKERTYPES_H
#define STUPIDKEYVALUECONTENTTRACKERTYPES_H

#include <QHash>
#include <QSharedPointer>

class IKeyValueStoreMutation;
typedef QSharedPointer<IKeyValueStoreMutation> StupidKeyValueContentTracker_StagedMutationsValueType;
typedef QHash<QString /*key*/, StupidKeyValueContentTracker_StagedMutationsValueType /*value*/> StagedMutationsType;
typedef QHash<QString /*key*/, QString /*value*/> CurrentDataType;

#endif // STUPIDKEYVALUECONTENTTRACKERTYPES_H
