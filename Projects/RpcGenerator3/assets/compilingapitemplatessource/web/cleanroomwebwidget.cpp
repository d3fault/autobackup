#include "cleanroomwebwidget.h"

#include <boost/bind.hpp>

#include "cleanroomsession.h"

void CleanRoomWebWidget::handleCleanRoomSessionStarted(CleanRoomSession *session)
{
    m_Session = session;
    std::string wtSessionId; //TODOreq
    m_Session->requestNewCleanRoomFrontPageDefaultView(wtSessionId, boost::bind(&CleanRoomWebWidget::handleFrontPageDefaultViewReceived, this, _1), 69420);
}
void CleanRoomWebWidget::handleFrontPageDefaultViewReceived(QStringList frontPageDocs)
{
    //TODOreq: ez
}
