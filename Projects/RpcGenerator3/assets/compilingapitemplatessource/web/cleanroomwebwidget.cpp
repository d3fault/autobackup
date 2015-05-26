#include <Wt/WEnvironment>
using namespace Wt;
#include "cleanroomwebwidget.h"

#include <Wt/WLabel>
#include <Wt/WBreak>

#include <boost/bind.hpp>

#include "cleanroomsession.h"

ICleanRoom *CleanRoomWebWidget::s_CleanRoom = NULL;

CleanRoomWebWidget::CleanRoomWebWidget(const WEnvironment &myEnv, WtLibVersion version)
    : WApplication(myEnv, version)
{
    for(int i = 0; i < NUM_CLEAN_ROOM_DOCS_ON_FRONT_PAGE; ++i)
    {
        WLabel *newLabel = new WLabel(root());
        new WBreak(root());
        m_CleanRoomDocsWidgets.insert(i, newLabel);
    }

    if(environment.ajax())
        this->enableUpdates(true);
    else
        this->deferRendering();

    CleanRoomSession::requestNewSession(s_CleanRoom, this->sessionId(), boost::bind(&CleanRoomWebWidget::handleCleanRoomSessionStarted, this, _1));
}
WApplication *CleanRoomWebWidget::cleanRoomWebWidgetEntryPoint(const WEnvironment &myEnv)
{
    return new CleanRoomWebWidget(myEnv);
}
void CleanRoomWebWidget::handleCleanRoomSessionStarted(CleanRoomSession *session)
{
    m_Session = session;
    m_Session->requestNewCleanRoomFrontPageDefaultView(this->sessionId(), boost::bind(&CleanRoomWebWidget::handleFrontPageDefaultViewReceived, this, _1), 69420);
}
void CleanRoomWebWidget::handleFrontPageDefaultViewReceived(QStringList frontPageDocs)
{
    int max = NUM_CLEAN_ROOM_DOCS_ON_FRONT_PAGE;
    int numDocs = qMin(frontPageDocs.size(), max);
    for(int i = 0; i < numDocs; ++i)
    {
        QString frontPageDoc = frontPageDocs.at(i);
        m_CleanRoomDocsWidgets.at(i)->setText(frontPageDoc.toStdString());
    }

    if(environment().ajax())
        this->triggerUpdate();
    else
        this->resumeRendering();
}
