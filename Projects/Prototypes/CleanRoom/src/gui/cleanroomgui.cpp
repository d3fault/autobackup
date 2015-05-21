#include "cleanroomgui.h"

#include "cleanroom.h"
#include "cleanroomsession.h"

CleanRoomGui::CleanRoomGui(QObject *parent)
    : QObject(parent)
    , m_CleanRoom(new CleanRoom(this))
{
    connect(m_CleanRoom, SIGNAL(readyForSessions()), this, SLOT(handleCleanRoomReadyForSessions()));
    QMetaObject::invokeMethod(m_CleanRoom, "initializeAndStart");
}
void CleanRoomGui::handleCleanRoomReadyForSessions()
{
    //the problem: i put my 'cross thread' stuff in the request logic, but i can't get a request until i get a session... and i don't have one. so i want to be able to re-use the 'cross thread' code i guess... i'm just not sure i can
    m_Gui.reset(new CleanRoomGuiWidget());
    CleanRoomSession::createNewSession(m_CleanRoom, m_Gui.data(), "handleNewSessionCreated");
}
