#include <Wt/WEnvironment>
using namespace Wt;
#include "cleanroomwebwidget.h"

#include <Wt/WLabel>
#include <Wt/WBreak>

#include <boost/bind.hpp>

ICleanRoom *CleanRoomWebWidget::s_CleanRoom = NULL;

CleanRoomWebWidget::CleanRoomWebWidget(const WEnvironment &environment, WtLibVersion version)
    : WApplication(environment, version)
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
void CleanRoomWebWidget::handleCleanRoomSessionStarted(CleanRoomSession session)
{
    if(!environment().ajax())
        this->resumeRendering(); //resume because we got session
    m_Session.reset(new CleanRoomSession(session)); //we do a 'new' here because we want to +1 the implicitly shared session (notice we're using the copy constructor of CleanRoomSession). If we did "&session" instead, then the pointer in the m_Session scoped pointer would be a dangling pointer. I had to use a scoped pointer around CleanRoomSession in the first place because CleanRoomSession has no default constructor
    m_Session->requestNewCleanRoomFrontPageDefaultView(this->sessionId(), boost::bind(&CleanRoomWebWidget::handleFrontPageDefaultViewReceived, this, _1), 69420);
    if(!environment().ajax())
        this->deferRendering(); //defer again because we're making a request
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
