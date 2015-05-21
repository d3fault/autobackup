#include "cleanroomgui.h"

#include "cleanroom.h"
#include "cleanroomwidget.h"

class ICleanRoomFrontEndResponder;

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
    QMetaObject::invokeMethod(m_CleanRoom, "startNewSession", Q_ARG(ICleanRoomFrontEndResponder*, m_Gui->responder()));
    m_Gui->show();
}
#if 0
void CleanRoomGui::handleCleanRoomFinishedGettingFrontPageDefaultView(QList<QString> frontPageDocs)
{
    //TODOreq: ez
}
#endif
