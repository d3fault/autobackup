#include "%API_NAME_LOWERCASE%frontpagedefaultviewrequestfromwt.h"

#include <boost/bind.hpp>

%API_NAME%FrontPageDefaultViewRequestFromWt::%API_NAME%FrontPageDefaultViewRequestFromWt(%API_NAME% *cleanRoom, const std::string &wtSessionId, boost::function<void (QStringList)> wApplicationCallback)
    : I%API_NAME%FrontPageDefaultViewRequest(cleanRoom)
    , m_WtSessionId(wtSessionId)
    , m_WApplicationCallback(wApplicationCallback)
{ }
void %API_NAME%FrontPageDefaultViewRequestFromWt::respond(QStringList frontPageDocs)
{
    boost::bind(m_WApplicationCallback, frontPageDocs);
    //Wt::WServer::instance()->post(m_WtSessionId, boost::bind(m_WApplicationCallback, frontPageDocs));
}
