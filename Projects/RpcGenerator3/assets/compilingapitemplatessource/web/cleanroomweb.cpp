#include "cleanroomweb.h"

#include <boost/bind.hpp>

#include "cleanroom.h"
#include "cleanroomsession.h"

#include "cleanroomwebwidget.h"

CleanRoomWeb::CleanRoomWeb(QObject *parent)
    : QObject(parent)
    , m_CleanRoom(new CleanRoom(this))
{
    connect(m_CleanRoom, &CleanRoom::readyForSessions, this, &CleanRoomWeb::handleCleanRoomReadyForSessions);
    connect(this, &CleanRoomWeb::initializeAndStartRequested, m_CleanRoom, &CleanRoom::initializeAndStart);
    emit initializeAndStartRequested();
    //QMetaObject::invokeMethod(m_CleanRoom, "initializeAndStart");
}
void CleanRoomWeb::handleCleanRoomReadyForSessions()
{
    //activate wt, wait for sessions, then do:
    std::string wtSessionId;
    CleanRoomWebWidget *wApplication;
    CleanRoomSession::requestNewSession(m_CleanRoom, wtSessionId, boost::bind(&CleanRoomWebWidget::handleCleanRoomSessionStarted, wApplication, _1));
}
