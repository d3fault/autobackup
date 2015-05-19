#include "cleanroomwww.h"

#include "cleanroom.h"
#include "cleanroomwtsessionprovider.h"

CleanRoomWWW::CleanRoomWWW(QObject *parent)
    : QObject(parent)
    , m_CleanRoom(new CleanRoom(this))
{
    m_CleanRoom->registerSessionProvider(new CleanRoomWtSessionProvider(this)); //Note: "session" (wt-app-session, gui-app-duration, etc) != "logged in session"
}
