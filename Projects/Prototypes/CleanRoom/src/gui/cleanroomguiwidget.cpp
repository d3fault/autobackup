#include "cleanroomguiwidget.h"

#include <QDebug>

#include "cleanroomsession.h"

CleanRoomGuiWidget::CleanRoomGuiWidget(QWidget *parent)
    : QWidget(parent)
{ }
void CleanRoomGuiWidget::handleNewSessionCreated(CleanRoomSession* session)
{
    m_Session = session;
    m_Session->requestNewCleanRoomFrontPageDefaultView(this, SLOT(handleFrontPageDefaultViewReceived(QStringList)), 69420);
}
void CleanRoomGuiWidget::handleFrontPageDefaultViewReceived(QStringList frontPageDocs)
{
    qDebug() << "front end received: " << frontPageDocs;
    //TODOreq: ez
}
