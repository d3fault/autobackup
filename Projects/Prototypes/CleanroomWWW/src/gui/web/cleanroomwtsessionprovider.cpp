#include "cleanroomwtsessionprovider.h"

CleanRoomWtSessionProvider::CleanRoomWtSessionProvider(QObject *metaObjectToCallOnNewSession, QMetaMethod metaMethodToCallOnNewSession, QObject *parent)
    : ICleanRoomSessionProvider(metaObjectToCallOnNewSession, metaMethodToCallOnNewSession, parent)
{ }
