#include "cleanroomguisessionprovider.h"

#include "cleanroomwidget.h"

CleanRoomGuiSessionProvider::CleanRoomGuiSessionProvider(QObject *metaObjectToCallWhenNewSession, QMetaMethod metaMethodToCallWhenNewSession)
    : ICleanRoomSessionProvider(metaObjectToCallWhenNewSession, metaMethodToCallWhenNewSession)
{ }
void CleanRoomGuiSessionProvider::provideGuiSession(QObject *parent)
{
    handleNewSession(new CleanRoomWidget(parent));
}
