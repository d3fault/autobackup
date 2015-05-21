#include "cleanroomweb.h"

#include <QVariantList>

#include <boost/bind.hpp>

#include "cleanroom.h"
#include "cleanroomsession.h"

class WApplication
{ };
class CleanRoomWebWidget : public WApplication
{
public:
    void handleNewSessionCreated(CleanRoomSession* session)
    {
        m_Session = session;
        m_Session->requestNewCleanRoomFrontPageDefaultView(m_WtSessionId, boost::bind(&CleanRoomWebWidget::handleFrontPageDefaultViewReceived, this, _1));
    }
    void handleFrontPageDefaultViewReceived(QVariantList frontPageDocsInVariantList)
    {
        QStringList frontPageDocs = frontPageDocsInVariantList.first().toStringList();

        //TODOreq: ez
    }
private:
    std::string m_WtSessionId;
    CleanRoomSession *m_Session;
};

CleanRoomWeb::CleanRoomWeb(QObject *parent)
    : QObject(parent)
    , m_CleanRoom(new CleanRoom(this))
{
    connect(m_CleanRoom, SIGNAL(readyForSessions()), this, SLOT(handleCleanRoomReadyForSessions()));
    QMetaObject::invokeMethod(m_CleanRoom, "initializeAndStart");
}
void CleanRoomWeb::handleCleanRoomReadyForSessions()
{
    //activate wt, wait for sessions, then do:
    std::string wtSessionId;
    CleanRoomWebWidget *wApplication;
    CleanRoomSession::createNewSession(m_CleanRoom, wtSessionId, boost::bind(&CleanRoomWebWidget::handleNewSessionCreated, wApplication, _1));
}