#include "cleanroomgui.h"

CleanRoomGui::CleanRoomGui(QObject *parent)
    : QObject(parent)
    , m_CleanRoom(new CleanRoom(this))
    //, m_WidgetFactory(new CleanRoomWidgetFactory(this))
{
    connect(m_CleanRoom, SIGNAL(readyForSessions()), this, SLOT(handleCleanRoomReadyForSessions()));
    QMetaObject::invokeMethod(m_CleanRoom, "initializeAndStart");
}
void CleanRoomGui::handleCleanRoomReadyForSessions()
{
    //m_WidgetFactory.makeWidget
    m_Gui.reset(new CleanRoomWidget());
    QMetaObject::invokeMethod(m_CleanRoom, "handleNewSession", Q_ARG(ICleanRoomSession*, m_Gui.data()));
    m_Gui->show();
}
