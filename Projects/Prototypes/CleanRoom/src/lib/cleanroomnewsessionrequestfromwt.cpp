#include "cleanroomnewsessionrequestfromwt.h"

#include <boost/bind.hpp>

#include "cleanroomsession.h"

#if 0
void CleanRoomNewSessionRequestFromWt::regainContextPossiblyInOtherThread_aka_respondActual(QVariantList responseArgs)
{
    CleanRoomSession *session = responseArgs.first().value<CleanRoomSession*>();

    boost::bind(m_WApplicationCallback, session);
    //Wt::WServer::instance()->post(m_WtSessionId, boost::bind(m_WApplicationCallback, responseArgs));
}
#endif
void CleanRoomNewSessionRequestFromWt::respond(CleanRoomSession *session)
{
    boost::bind(m_WApplicationCallback, session);
    //Wt::WServer::instance()->post(m_WtSessionId, boost::bind(m_WApplicationCallback, session));
}
