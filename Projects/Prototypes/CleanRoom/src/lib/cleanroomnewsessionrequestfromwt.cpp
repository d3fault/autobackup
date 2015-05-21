#include "cleanroomnewsessionrequestfromwt.h"

#include <boost/bind.hpp>

#include "cleanroomsession.h"

void CleanRoomNewSessionRequestFromWt::respond(CleanRoomSession *session)
{
    boost::bind(m_WApplicationCallback, session);
    //Wt::WServer::instance()->post(m_WtSessionId, boost::bind(m_WApplicationCallback, session));
}
