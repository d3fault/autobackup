#include "cleanroomwebwidget.h"

#include <QSharedPointer>

#include <boost/bind.hpp>

#include "cleanroomfrontpagedefaultviewrequestfromwt.h"

class CleanRoomSession;

void CleanRoomWebWidget::handleCleanRoomSessionStarted(CleanRoomSession *session)
{
    m_Session = session;
    QSharedPointer<CleanRoomFrontPageDefaultViewRequestFromWt> *frontPageDefaultViewRequest(new CleanRoomFrontPageDefaultViewRequestFromWt(m_Session, boost::bind(&CleanRoomWebWidget::handleFrontPageDefaultViewReceived, static_cast<CleanRoomWebWidget*>(this), _1)));
    //boost::bind(boost::bind(&CleanRoomWebWidget::handleFrontPageDefaultViewReceived, static_cast<CleanRoomWebWidget*>(this), _1, _2, _3), couchbaseStoreKeyInput, lcbOpSuccess, dbError);
}
void CleanRoomWebWidget::handleFrontPageDefaultViewReceived(QStringList frontPageDocs)
{
    //TODOreq: ez
}
