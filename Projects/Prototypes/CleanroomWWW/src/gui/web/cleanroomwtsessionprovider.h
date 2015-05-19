#ifndef CLEANROOMWTSESSIONPROVIDER_H
#define CLEANROOMWTSESSIONPROVIDER_H

#include <QObject>

#include "icleanroomsessionprovider.h"

class CleanRoomWtSessionProvider : public ICleanRoomSessionProvider
{
    Q_OBJECT
public:
    explicit CleanRoomWtSessionProvider(QObject *metaObjectToCallOnNewSession, QMetaMethod metaMethodToCallOnNewSession, QObject *parent = 0);
};

#endif // CLEANROOMWTSESSIONPROVIDER_H
