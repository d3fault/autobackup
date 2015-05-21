#ifndef CLEANROOMGUISESSIONPROVIDER_H
#define CLEANROOMGUISESSIONPROVIDER_H

#include "icleanroomsessionprovider.h"

class CleanRoomGuiSessionProvider : public ICleanRoomSessionProvider
{
public:
    CleanRoomGuiSessionProvider(QObject *metaObjectToCallWhenNewSession, QMetaMethod metaMethodToCallWhenNewSession);
    void provideGuiSession(QObject *parent = 0);
};

#endif // CLEANROOMGUISESSIONPROVIDER_H
