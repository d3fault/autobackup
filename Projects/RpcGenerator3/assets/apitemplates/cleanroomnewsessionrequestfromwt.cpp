%RPC_AUTO_GENERATED_FILE_WARNING%
#include <Wt/WServer>
#include "%API_NAME_LOWERCASE%newsessionrequestfromwt.h"

#include <boost/bind.hpp>

#include "%API_NAME_LOWERCASE%session.h"

void %API_NAME%NewSessionRequestFromWt::respond(%API_NAME%Session session)
{
    boost::bind(m_WApplicationCallback, session);
    Wt::WServer::instance()->post(m_WtSessionId, boost::bind(m_WApplicationCallback, session));
}
