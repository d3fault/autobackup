#include <Wt/WServer>
using namespace Wt;

#include "cleanroomweb.h"

#include <QCoreApplication>

#include <boost/bind.hpp>

#include "cleanroom.h"
#include "cleanroomsession.h"

#include "cleanroomwebwidget.h"

CleanRoomWeb::CleanRoomWeb(int argc, char *argv[], QObject *parent)
    : QObject(parent)
    , m_CleanRoom(new CleanRoom(this))
{
    QString firstArg = QCoreApplication::arguments().first();
    m_WtServer.reset(new WServer(firstArg.toStdString()));
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
    CleanRoomWebWidget::s_CleanRoom = m_CleanRoom; //TODOoptional: I know there's a way to do it better than this (without a static), but I'm too lazy to do it atm
    m_WtServer->start();
}
