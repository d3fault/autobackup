#include "cleanroomwebwidget.h"

#include <boost/bind.hpp>

#include "cleanroomsession.h"

CleanRoomWebWidget::CleanRoomWebWidget(const WEnvironment &myEnv)
    : WApplication(myEnv)
{
    for(int i = 0; i < NUM_CLEAN_ROOM_DOCS_ON_FRONT_PAGE; ++i)
    {
        QLabel *newLabel = new QLabel(root());
        new WBreak(root());
        m_CleanRoomDocsWidgets.insert(i, newLabel);
    }
}
WApplication *CleanRoomWebWidget::cleanRoomWebWidgetEntryPoint(const WEnvironment &myEnv)
{
    return new CleanRoomWebWidget(myEnv);
}
void CleanRoomWebWidget::handleCleanRoomSessionStarted(CleanRoomSession *session)
{
    m_Session = session;
    std::string wtSessionId; //TODOreq
    m_Session->requestNewCleanRoomFrontPageDefaultView(wtSessionId, boost::bind(&CleanRoomWebWidget::handleFrontPageDefaultViewReceived, this, _1), 69420);
}
void CleanRoomWebWidget::handleFrontPageDefaultViewReceived(QStringList frontPageDocs)
{
    int max = NUM_CLEAN_ROOM_DOCS_ON_FRONT_PAGE;
    int numDocs = qMin(frontPageDocs.size(), max);
    for(int i = 0; i < numDocs; ++i)
    {
        m_CleanRoomDocsWidgets.at(i)->setText(frontPageDocs.at(i));
    }
}
