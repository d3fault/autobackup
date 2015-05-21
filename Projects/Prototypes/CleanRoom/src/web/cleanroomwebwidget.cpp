#include "cleanroomwebwidget.h"

#include <boost/bind.hpp>

#include "cleanroomfrontpagedefaultviewrequestfromwt.h"

class CleanRoomSession;

void CleanRoomWebWidget::handleCleanRoomSessionStarted(CleanRoomSession *session)
{
    m_Session = session;
    std::string wtSessionId; //TODOreq: ez
    CleanRoomFrontPageDefaultViewRequestFromWt *request = new CleanRoomFrontPageDefaultViewRequestFromWt(m_Session, wtSessionId, boost::bind(&CleanRoomWebWidget::handleFrontPageDefaultViewReceived, this, _1));
    request->invokeSlotThatHandlesRequest();
    //boost::bind(boost::bind(&CleanRoomWebWidget::handleFrontPageDefaultViewReceived, static_cast<CleanRoomWebWidget*>(this), _1, _2, _3), couchbaseStoreKeyInput, lcbOpSuccess, dbError);
}
//void CleanRoomWebWidget::handleFrontPageDefaultViewReceived(QStringList frontPageDocs)
void CleanRoomWebWidget::handleFrontPageDefaultViewReceived(QVariantList frontPageDocsVariantList)
{
    QStringList frontPageDocs = frontPageDocsVariantList.first().toStringList(); //TODOmb: is there a more elegant way/place to do this? i'd prefer my method arg is the QStringList

    //TODOreq: ez
}
