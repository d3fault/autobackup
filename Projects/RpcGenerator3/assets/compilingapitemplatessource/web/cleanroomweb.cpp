#include <Wt/WServer>
using namespace Wt;

#include "cleanroomweb.h"

#include <QCoreApplication>

#include <boost/bind.hpp>

#include "cleanroom.h"
#include "cleanroomsession.h"

#include "cleanroomwebwidget.h"

CleanRoomWeb::CleanRoomWeb(QObject *parent)
    : QObject(parent)
    , m_CleanRoom(new CleanRoom(this))
{
    m_WtServer.reset(new WServer(QCoreApplication::arguments().first()));
    m_WtServer->setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);
    m_WtServer->addEntryPoint(Application, &CleanRoomWebWidget::cleanRoomWebWidgetEntryPoint);

    connect(m_CleanRoom, &CleanRoom::readyForSessions, this, &CleanRoomWeb::handleCleanRoomReadyForSessions);
    connect(this, &CleanRoomWeb::initializeAndStartRequested, m_CleanRoom, &CleanRoom::initializeAndStart);
    emit initializeAndStartRequested();
    //QMetaObject::invokeMethod(m_CleanRoom, "initializeAndStart");
}
CleanRoomWeb::~CleanRoomWeb()
{
    if(!m_WtServer.isNull())
    {
        if(m_WtServer->isRunning())
        {
            m_WtServer->stop();
        }
        m_WtServer.reset();
    }
}
void CleanRoomWeb::handleCleanRoomReadyForSessions()
{
    if(m_WtServer.isNull() || m_WtServer->isRunning())
        return;
    m_WtServer->start();

#if 0 //TODOreq:
    //activate wt, wait for sessions, then, when there's a new session do:
    std::string wtSessionId;
    CleanRoomWebWidget *wApplication;
    CleanRoomSession::requestNewSession(m_CleanRoom, wtSessionId, boost::bind(&CleanRoomWebWidget::handleCleanRoomSessionStarted, wApplication, _1));
#endif
}
