#include "cleanroomguiwidget.h"

#include "cleanroomfrontpagedefaultviewrequestfromqt.h"

CleanRoomGuiWidget::CleanRoomGuiWidget(QWidget *parent)
    : QWidget(parent)
{ }
void CleanRoomGuiWidget::handleCleanRoomSessionStarted(CleanRoomSession *session)
{
    m_Session = session;
    CleanRoomFrontPageDefaultViewRequestFromQt *request = new CleanRoomFrontPageDefaultViewRequestFromQt(m_Session, this, SLOT(handleFrontPageDefaultViewReceived(QList<QString>)));
    request->invokeSlotThatHandlesRequest();
    //TODOoptional: ideally: m_Session->createNewCleanRoomFrontPageDefaultViewRequested(this, SLOT(handleFrontPageDefaultViewReceived(QList<QString>))); the reason i'm not impl'ing it now is just to KISS. the reason i WANT it is so that NO memory management is  left to the client of the api (atm i am calling 'new' on a request)... and additionally the session could call invoke() on it too, eliminating 1 extra line the api caller has to type. 1 line (excl callback ofc) is good
}
//void CleanRoomGuiWidget::handleFrontPageDefaultViewReceived(QStringList frontPageDocs)
void CleanRoomGuiWidget::handleFrontPageDefaultViewReceived(QVariantList frontPageDocsVariantList)
{
    QStringList frontPageDocs = frontPageDocsVariantList.first().toStringList();

    //TODOreq: ez
}
