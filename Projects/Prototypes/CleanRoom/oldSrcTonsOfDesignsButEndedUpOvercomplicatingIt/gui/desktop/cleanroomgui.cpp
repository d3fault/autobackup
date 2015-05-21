#include "cleanroomgui.h"

#include "cleanroom.h"
#include "cleanroomguisessionprovider.h"

CleanRoomGui::CleanRoomGui(QObject *parent)
    : QObject(parent)
    , m_CleanRoom(new CleanRoom(this))
{
    CleanRoomGuiSessionProvider *guiSessionProvider = new CleanRoomGuiSessionProvider(this);
    m_CleanRoom->registerSessionProvider(guiSessionProvider);
    guiSessionProvider->provideGuiSession(this); //Gui only has 1x session, but this code is compatible with Wt's N sessions
}
