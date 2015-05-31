#include "cleanroomgui.h"

#include "cleanroom.h"
#include "cleanroomsession.h"

//TODOmb: "AutoDrillDown", which is basically like "default GUI(s)" for a generated API. It's the rpc generator's best guess at how to present the API to a given view. Getting ahead of myself with this, but is a good idea (that I've had before). For example,
CleanRoomGui::CleanRoomGui(QObject *parent)
    : QObject(parent)
    , m_CleanRoom(new CleanRoom(this))
{
    connect(m_CleanRoom, &ICleanRoom::readyForSessions, this, &CleanRoomGui::handleCleanRoomReadyForSessions);
    connect(this, &CleanRoomGui::initializeAndStartCleanRoomRequested, m_CleanRoom, &ICleanRoom::initializeAndStart);
    emit initializeAndStartCleanRoomRequested();
}
void CleanRoomGui::handleCleanRoomReadyForSessions()
{
    m_Gui.reset(new CleanRoomGuiWidget());
    m_Gui->show();
    CleanRoomSession::requestNewSession(m_CleanRoom, m_Gui.data(), SLOT(handleNewSessionCreated(CleanRoomSession)));
}
