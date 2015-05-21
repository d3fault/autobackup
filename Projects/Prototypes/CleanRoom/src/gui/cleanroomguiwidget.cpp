#include "cleanroomguiwidget.h"

#include <QSharedPointer>

#include "cleanroomfrontpagedefaultviewrequestfromqt.h"

CleanRoomGuiWidget::CleanRoomGuiWidget(QWidget *parent)
    : QWidget(parent)
{ }
void CleanRoomGuiWidget::handleCleanRoomSessionStarted(CleanRoomSession *session)
{
    m_Session = session;
    QSharedPointer<CleanRoomFrontPageDefaultViewRequestFromQt> *frontPageDefaultViewRequest(new CleanRoomFrontPageDefaultViewRequestFromQt(m_Session, SLOT(handleFrontPageDefaultViewReceived(QList<QString>))));
}
void CleanRoomGuiWidget::handleFrontPageDefaultViewReceived(QStringList frontPageDocs)
{
    //TODOreq: ez
}

