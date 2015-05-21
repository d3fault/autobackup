#include "cleanroomnewsessionrequestfromwt.h"

#include <boost/bind.hpp>

#include "cleanroomsession.h"

void CleanRoomNewSessionRequestFromWt::regainContextPossiblyInOtherThread_aka_respondActual(QVariantList responseArgs)
{
    CleanRoomSession *session = responseArgs.first().value<CleanRoomSession*>();

    boost::bind(m_WApplicationCallback, session);
    //Wt::WServer::instance()->post(m_WtSessionId, boost::bind(m_WApplicationCallback, responseArgs));
}
