#include "cleanroomguiwidget.h"

#include "cleanroomfrontpagedefaultviewrequestfromqt.h"

CleanRoomGuiWidget::CleanRoomGuiWidget(QWidget *parent)
    : QWidget(parent)
{ }
void CleanRoomGuiWidget::handleCleanRoomSessionStarted(CleanRoomSession *session)
{
    m_Session = session;
    new CleanRoomFrontPageDefaultViewRequestFromQt(m_Session, this, SLOT(handleFrontPageDefaultViewReceived(QList<QString>)));
}
//void CleanRoomGuiWidget::handleFrontPageDefaultViewReceived(QStringList frontPageDocs)
void CleanRoomGuiWidget::handleFrontPageDefaultViewReceived(QVariantList frontPageDocsVariantList)
{
    QStringList frontPageDocs = frontPageDocsVariantList.first().toStringList();

    //TODOreq: ez
}
